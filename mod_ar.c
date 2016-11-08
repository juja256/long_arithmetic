#include "long_ar.h"
#include <stdlib.h>
#include <stdio.h>

extern int l_init(L_NUMBER* n, u32 words_len);

MODULAR_AR_FUNC void m_pre_barret(u32 l, const L_NUMBER* n, L_NUMBER* mu) {
	L_NUMBER r, b, t, N;
	l_init(&t, l+1);
	l_init(&r, l+1);
	l_init(&b, l+1); b.words[0] = 1;
	l_init(&N, l+1); l_copy(&N, n);
	l_shift_l(&b, l*ARCH, &b);
	l_div(&b, &N, &t, &r);
	for (u32 i=0; i<mu->len; i++)
		mu->words[i] = t.words[i];
	l_free(&b);
	l_free(&r);
	l_free(&N);
	l_free(&t);
}

MODULAR_AR_FUNC void m_redc_barret(const L_NUMBER* a, const L_NUMBER* n, L_NUMBER* mu, L_NUMBER* res) {
	L_NUMBER q; 
	l_init(&q, 2*a->len);
	L_NUMBER r; 
	l_init(&r, n->len);
	l_copy(&q, a);

	l_shift_r(&q, (S_WORD)((a->len / 2) - 1)*ARCH, &q);
	l_mul(&q, mu, &q);
	l_shift_r(&q, ((a->len / 2) + 1)*ARCH, &q);
	l_mul(&q, n, &q);
	l_sub(a, &q, &q);

	if (q.words[n->len]) {
		l_sub(&q, n, &q);
	} // Preventing overflow
    l_copy(&r, &q);

	while (l_cmp(&r, n) != -1) {
		l_sub(&r, n, &r);
	}
	l_copy(res, &r);
    l_free(&q);
    l_free(&r);
}

MODULAR_AR_FUNC void m_gcd(const L_NUMBER* a, const L_NUMBER* b, L_NUMBER* res) {
	res->words[0] = 1;
	L_NUMBER r = {0, 0};
	L_NUMBER q = {0, 0};
	L_NUMBER d;
	L_NUMBER zero;
	l_copy(&q, a);
	l_copy(&r, b);
	l_init(&zero, b->len);
	l_init(&d, b->len);
	int c;
	while (!( (r.words[0] | q.words[0]) & 1 )) {
		l_shift_r(&r, 1, &r);
		l_shift_r(&q, 1, &q);
		l_shift_l(res, 1, res);
	}

	while (!(q.words[0] & 1)) {
		l_shift_r(&q, 1, &q);
	}

	while(l_cmp(&r, &zero)) {
		while (!(r.words[0] & 1)) 
			l_shift_r(&r, 1, &r);
		c = l_cmp(&q, &r);
		if (c == 1) {
			l_copy(&d, &r);
			l_sub(&q, &r, &r);
			l_copy(&q, &d);
		}
		else {
			l_sub(&r, &q, &r);
		}
	}
	l_mul(res, &q, res);

	l_free(&d);
	l_free(&zero);
	l_free(&r);
	l_free(&q);
}

MODULAR_AR_FUNC void m_lcm(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
	L_NUMBER g, tmp;
	l_init(&g, res->len);
	l_init(&tmp, res->len);

	l_mul(n1, n2, res);
	m_gcd(n1, n2, &g);
	l_div(res, &g, res, &tmp);

	l_free(&g);
	l_free(&tmp);
}

MODULAR_AR_FUNC void m_add(const L_NUMBER* n1, const L_NUMBER* n2, const L_NUMBER* n, L_NUMBER* res) {
	l_add(n1, n2, res);
	l_div(res, n, NULL, res);
}

MODULAR_AR_FUNC void m_sub(const L_NUMBER* n1, const L_NUMBER* n2, const L_NUMBER* n, L_NUMBER* res) {
	l_sub(n1, n2, res);
	l_div(res, n, NULL, res);
}

MODULAR_AR_FUNC void m_sqr(const L_NUMBER* a, const L_NUMBER* n, L_NUMBER* mu, L_NUMBER* res) {
    L_NUMBER t;
    l_init(&t, 2*a->len);
	l_sqr(a, &t);
	m_redc_barret(&t, n, mu, res);
    l_free(&t);
}

MODULAR_AR_FUNC void m_mul(const L_NUMBER* n1, const L_NUMBER* n2, const L_NUMBER* n, L_NUMBER* mu, L_NUMBER* res) {
    L_NUMBER t;
    l_init(&t, 2*n1->len);
	l_mul(n1, n2, &t);
	m_redc_barret(&t, n, mu, res);
    l_free(&t);
}

MODULAR_AR_FUNC void m_mul_blakley(const L_NUMBER* n1, const L_NUMBER* n2, const L_NUMBER* n, L_NUMBER* res) {
    L_NUMBER r;
    l_init(&r, n->len);
    u32 k = l_bit_len(n1);
    for (int i=k-1; i>=0; i--) {
        l_shift_l(&r, 1, &r);
        if (n1->words[i/ARCH] & (1L << (i%ARCH)))
            l_add(&r, n2, &r);

        while (l_cmp(&r, n) != -1) 
            l_sub(&r, n, &r);
    }
    l_copy(res, &r);
    l_free(&r);
}

MODULAR_AR_FUNC void m_pow(const L_NUMBER* x, const L_NUMBER* p, const L_NUMBER* n, L_NUMBER* mu, L_NUMBER* res) {
	if (!mu) {
		m_pre_barret(2*x->len, n, mu);
	}
	u32 k = l_bit_len(p);
    L_NUMBER a, c;
    l_init(&c, n->len);
    l_init(&a, n->len);
    l_copy(&a, x);

    c.words[0] = 1;
    for (u32 i=0; i<k; i++) {
        if (p->words[i/ARCH] & (1L << (i % ARCH))) {
            m_mul(&c, &a, n, mu, &c);
        }
        m_sqr(&a, n, mu, &a);
    }

    l_copy(res, &c);
    l_free(&a);
    l_free(&c);
}
