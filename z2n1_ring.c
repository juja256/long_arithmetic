#include "long_ar.h"
#include <stdio.h>
#include <stdlib.h>

static L_NUMBER base;

Z2N1_AR_FUNCTION void z2n1_init_base(WORD N) {
	l_init(&base, N+1);
	base.words[0]=1; base.words[N]=1;
}

Z2N1_AR_FUNCTION void z2n1_destroy_base() {
	l_free(&base);
}

Z2N1_AR_FUNCTION void z2n1_normalize(const L_NUMBER* n, WORD N, L_NUMBER* res) { // || n || >= 2N
	L_NUMBER norm={0,0};
	l_init(&norm, N+1);
	WORD m = n->len - N;
	L_NUMBER lo={n->words, N};
	L_NUMBER hi={n->words+N, m};
	l_copy(&norm, &lo);
	if (m/N) {
		hi.len = N;
		if ( l_sub(&lo, &hi, &norm) ) {
			L_NUMBER norm_N = {norm.words, N};
			L_NUMBER unity = {base.words, N};
			l_add(&norm_N, &unity, &norm);
		}
	}
	int mm = m%N;
	if (mm) {
		L_NUMBER hi2 = {n->words+2*N, mm};
		L_NUMBER norm_rest = {norm.words, mm};
		if ( l_add(&hi2, &norm_rest, &norm_rest) ) {
			L_NUMBER norm_rest2 = {norm.words+mm, N+1-mm};
			l_propagate_carry(&norm_rest2);
		};
	}

	l_copy(res, &norm);
	l_free(&norm);
}

Z2N1_AR_FUNCTION void z2n1_mul(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
	L_NUMBER tmp = {0,0};
	l_mul(n1, n2, &tmp);
	z2n1_normalize(&tmp, n1->len-1, res);
}

Z2N1_AR_FUNCTION void z2n1_mul_by_two_power(const L_NUMBER* n, WORD ex, L_NUMBER* res) {
	WORD Nbits = (n->len - 1)*ARCH;
	WORD q = ex / Nbits;
	WORD r = ex % Nbits;
	if (q%2) {
		l_sub(&base, n, res);
	}
	else {
		l_copy(res, n);
	}
	if (r) {
		L_NUMBER tmp;
		l_init(&tmp, 2*n->len);
		l_shift_l(res, r, &tmp);
		z2n1_normalize(&tmp, (n->len - 1), res);
	}
}

Z2N1_AR_FUNCTION void z2n1_div_by_two_power(const L_NUMBER* n, WORD ex, L_NUMBER* res) {
	WORD Nbits = (n->len - 1)*ARCH;
	ex %= (2*Nbits);
	z2n1_mul_by_two_power(n, 2*Nbits-ex, res);
}

Z2N1_AR_FUNCTION void z2n1_add(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
	WORD N = n1->len-1;
	l_add(n1, n2, res);
	int c = res->words[N];
	res->words[N] = res->words[0] < res->words[N];
	if (!(res->words[N])) {
		res->words[0]-=c;
	}
	else {
		L_NUMBER tmp;
		l_init(&tmp, N+1); tmp.words[0]=c-1;
		l_sub(res, &tmp, res);
		l_free(&tmp);
	}
}

Z2N1_AR_FUNCTION void z2n1_sub(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
	WORD N = n1->len-1;
	L_NUMBER tmp; l_init(&tmp, N+1);
	l_sub(&base, n2, &tmp);
	z2n1_add(n1, &tmp, res);
	l_free(&tmp);
}


static void _z2n1_fft(L_NUMBER* inp, L_NUMBER* out, L_NUMBER* W, WORD K, WORD N, WORD s) {
	if (s < K) {
		_z2n1_fft(out,   inp,   W, K, N, 2*s);
		_z2n1_fft(out+s, inp+s, W, K, N, 2*s);
		WORD t;
		for (int i = 0; i < K; i += 2 * s) {
			t = (N*ARCH)*i/K;
			z2n1_mul_by_two_power(&(out[i+s]), t, W);
			z2n1_add(&(out[i]), W, &(inp[i/2])    );
			z2n1_sub(&(out[i]), W, &(inp[i/2+K/2]));
		}
	}
}

static void _z2n1_fft_inv(L_NUMBER* inp, L_NUMBER* out, L_NUMBER* W, WORD K, WORD N, WORD s) {
	if (s < K) {
		_z2n1_fft_inv(out,   inp,   W, K, N, 2*s);
		_z2n1_fft_inv(out+s, inp+s, W, K, N, 2*s);
		WORD t;
		for (int i = 0; i < K; i += 2 * s) {
			t = (N*ARCH)*i/K;
			z2n1_div_by_two_power(&(out[i+s]), t, W);
			z2n1_add(&(out[i]), W, &(inp[i/2])    );
			z2n1_sub(&(out[i]), W, &(inp[i/2+K/2]));
		}
	}
}

Z2N1_AR_FUNCTION void z2n1_fft(L_NUMBER* inp, L_NUMBER* _mem, WORD K) {

	for (u32 i=0;i<K;i++)
		l_copy(&(_mem[i]), &(inp[i]));

	WORD N = inp[0].len-1;
	L_NUMBER W; l_init(&W, N+1);
	_z2n1_fft(inp, _mem, &W, K, N, 1);

	l_free(&W);

}

Z2N1_AR_FUNCTION void z2n1_fft_inv(L_NUMBER* inp, L_NUMBER* _mem, WORD K) {

	WORD k = word_bit_len(K)-1;
	for (u32 i=0;i<K;i++)
		l_copy(&(_mem[i]), &(inp[i]));
	WORD N = inp[0].len-1;
	L_NUMBER W; l_init(&W, N+1);
	_z2n1_fft_inv(inp, _mem, &W, K, N, 1);
	for (u32 i=0; i<K; i++) {
		z2n1_div_by_two_power(&(inp[i]), k, &(inp[i]));
	}
	l_free(&W);
}

Z2N1_AR_FUNCTION void z2n1_weighted_fft(L_NUMBER* inp, L_NUMBER* _mem, WORD K) {
	WORD N = inp[0].len-1;
	for (u32 i=0;i<K;i++) {
		z2n1_mul_by_two_power(&(inp[i]), (N*ARCH)*i/K, &(inp[i]));
		l_copy(&(_mem[i]), &(inp[i]));
	}

	L_NUMBER W; l_init(&W, N+1);
	_z2n1_fft(inp, _mem, &W, K, N, 1);
	l_free(&W);
}

Z2N1_AR_FUNCTION void z2n1_weighted_fft_inv(L_NUMBER* inp, L_NUMBER* _mem, WORD K) {
	WORD N = inp[0].len-1;
	WORD k = word_bit_len(K)-1;
	for (u32 i=0;i<K;i++) {
		l_copy(&(_mem[i]), &(inp[i]));
	}

	L_NUMBER W; l_init(&W, N+1);
	_z2n1_fft_inv(inp, _mem, &W, K, N, 1);
	l_free(&W);
	for (u32 i=0;i<K;i++) {
		z2n1_div_by_two_power(&(inp[i]), (N*ARCH)*i/K + k, &(inp[i]));
	}
}

Z2N1_AR_FUNCTION void z2n1_dft_ordinary(L_NUMBER* inp, WORD K, L_NUMBER* out) {
	L_NUMBER W;
	WORD N = inp[0].len-1;
	l_init(&W, N+1);
	for (u32 i=0; i<K; i++) {
		for (u32 j=0; j<K; j++) {
			z2n1_mul_by_two_power(&(inp[j]), 2*(N*ARCH)*i*j/K, &W );
			z2n1_add( &(out[i]), &W, &(out[i]) );
		}
	}
	l_free(&W);
}

Z2N1_AR_FUNCTION void z2n1_dft_inv_ordinary(L_NUMBER* inp, WORD K, L_NUMBER* out) {
	L_NUMBER W;
	WORD N = inp[0].len-1;
	WORD k = word_bit_len(K)-1;
	l_init(&W, N+1);
	for (u32 i=0; i<K; i++) {
		for (u32 j=0; j<K; j++) {
			z2n1_div_by_two_power(&(inp[j]), 2*(N*ARCH)*i*j/K, &W );
			z2n1_add( &(out[i]), &W, &(out[i]) );
		}
		z2n1_div_by_two_power(&(out[i]), k, &(out[i]) );
	}
	l_free(&W);
}

LONG_AR_FUNC double l_mul_shonhage_strassen(const L_NUMBER* n1, const L_NUMBER* n2, AUTO_SIZE L_NUMBER* res) {
	WORD Nbit = n1->len*ARCH;
	WORD logNbits = word_bit_len(Nbit) - 1;
	WORD k = (logNbits >> 1) + 1;
	WORD K = (1 << k);
	WORD n = 1 << word_bit_len(2*Nbit / K + k);
	WORD N = n/ARCH;
	z2n1_init_base(N);
	WORD M = (Nbit >> k)/ARCH;
	L_NUMBER* bufA = malloc(sizeof(L_NUMBER)*K);
	L_NUMBER* bufB = malloc(sizeof(L_NUMBER)*K);
	L_NUMBER* mem = malloc(sizeof(L_NUMBER)*K);
	double eff = ((2.0*Nbit / K) + k) / n;
	printf("n: %d\nK: %d\nM: %d\n", n, K, M*ARCH);
	for (u32 i=0; i<K; i++) {
		bufA[i].words = 0; bufA[i].len = 0;
		bufB[i].words = 0; bufB[i].len = 0;
		l_init(&(bufA[i]), N+1);
		l_init(&(bufB[i]), N+1);
		l_init(&(mem[i]), N+1);
		L_NUMBER curA = {n1->words+i*M, M};
		L_NUMBER curB = {n2->words+i*M, M};
		l_copy(&(bufA[i]), &curA);
		l_copy(&(bufB[i]), &curB);
	}

	/* Applying weighted DFT */
	z2n1_weighted_fft(bufA, mem, K);
	z2n1_weighted_fft(bufB, mem, K);

	/* Negacyclic convolution */
	for (u32 i=0; i<K; i++) {
		z2n1_mul( &(bufA[i]), &(bufB[i]), &(bufA[i]) );
	}

	/* Applying inverse weighted DFT */
	z2n1_weighted_fft_inv(bufA, mem, K);

	/* Recompose (glucks present definetely) */
	L_NUMBER r = {0,0};

	if (res->len == 0) {
		l_init(&r, 2*n1->len);
		*res = r;
	}

	for (u32 i=0; i<K; i++) {
		printf("C[%i]: ", i);
		l_dump(&bufA[i], 'h');
		L_NUMBER prt1 = {r.words+i*M, 2*M+k};
		L_NUMBER prt2 = {bufA[i].words, 2*M+k};
		//l_add(&prt1, &prt2, &prt1);

		l_free(&(bufA[i]));
		l_free(&(bufB[i]));
		l_free(&(mem[i]));
	}
	free(bufA);
	free(bufB);
	free(mem);

	z2n1_destroy_base();

	if (res->len != 0) {
		l_copy(res, &r);
	}
	return eff;
}
