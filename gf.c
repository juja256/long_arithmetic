#include "long_ar.h"
#include "gf.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

INTERNAL_AR_FUNC extern u32 word_bit_len(WORD n);
/* BEGIN STATIC */
static void p_shift_l(const GF_ELEMENT* n, u32 p, GF_ELEMENT* res) {
	l_shift_l((const L_NUMBER*)n, p, (L_NUMBER*)res);
}

static int f_is_null(GF_ELEMENT* a) {
    for (int i=0; i<a->len; i++) {
        if (a->words[i]) return 0;
    }
    return 1;
}

static void f_null(GF_ELEMENT* a) {
    for (int i=0; i<a->len; i++) {
        a->words[i] = 0;
    }
}

static void f_copy_low(GF_ELEMENT* dest, const GF_ELEMENT* source) {
    if (!dest->len) {
        dest->len = source->len;
        dest->bit_len = source->bit_len;
        dest->words = (WORD*)malloc((dest->len)*sizeof(WORD));
    }
    for (u32 i=0; i<source->len; i++) {
        dest->words[i] = source->words[i];
    }
}

static void p_div(const GF_ELEMENT* a, const GF_ELEMENT* p, GF_ELEMENT* res) {
    f_null(res);
    u32 k = p_bit_len(a);
    u32 n = p_bit_len(p);
    GF_ELEMENT t={0,0,0};
    f_copy(&t, a);
    GF_ELEMENT c;
    f_init(&c, a->bit_len);
    GF_ELEMENT P;
    f_init(&P, a->bit_len);
    f_copy_low(&P, p);
    
    while (k >= n) {
        p_shift_l(&P, k-n, &c);
        p_add(&t, &c, &t);
        res->words[(k-n)/ARCH] ^= (1L << ((k-n)%ARCH));
        k = p_bit_len(&t);
    }
    f_free(&t);
    f_free(&c);
    f_free(&P);
}
/* END STATIC */

COMMON_GF void f_init(GF_ELEMENT* a, u32 bit_len) {
    L_NUMBER x;
    l_init_by_len(&x, bit_len);
    a->words = x.words;
    a->len = x.len;
    a->bit_len = bit_len;  
}

COMMON_GF void f_init_by_str(GF_ELEMENT* a, const char* str, u32 bit_len) {
    L_NUMBER x;
    l_init_by_str(&x, str);
    a->words = x.words;
    a->len = x.len;
    a->bit_len = bit_len;  
}

COMMON_GF void f_free(GF_ELEMENT* a) {
    free(a->words);
    a->len = 0;
    a->bit_len = 0;
}

COMMON_GF void f_copy(GF_ELEMENT* dest, const GF_ELEMENT* source) {
    if (!dest->len) {
        dest->words = (WORD*)malloc((source->len)*sizeof(WORD));
    }
    for (u32 i=0; i<source->len; i++) {
        dest->words[i] = source->words[i];
    }
    dest->len = source->len;
    dest->bit_len = source->bit_len;
}

COMMON_GF void f_dump(const GF_ELEMENT* a, char format) {
    l_dump((const L_NUMBER*)a, format);
}

POLYNOMIAL_BASIS u32 p_bit_len(const GF_ELEMENT* n) {
	return l_bit_len((const L_NUMBER*)n);
}

POLYNOMIAL_BASIS void p_zero(u32 len, GF_ELEMENT* zero) {
	if (!zero->len) f_init(zero, len);
}

POLYNOMIAL_BASIS void p_unity(u32 len, GF_ELEMENT* unity) {
	if (!unity->len) f_init(unity, len);
	unity->words[0] = 1;
}

POLYNOMIAL_BASIS int p_add(const GF_ELEMENT* a, const GF_ELEMENT* b, GF_ELEMENT* res) {
	if (a->bit_len != b->bit_len) return GF_ERROR;
	if (!res->bit_len) f_init(res, a->bit_len);
	for (int i=0; i<res->len; i++) {
		res->words[i] = a->words[i] ^ b->words[i];
	}
	return GF_SUCCESS;
}

POLYNOMIAL_BASIS void p_reduct(const GF_ELEMENT* a, const GF_ELEMENT* p, GF_ELEMENT* res) { /* slow */
    if (!res->len) f_init(res, p->bit_len);
	u32 k = p_bit_len(a);
	u32 n = p_bit_len(p);

    GF_ELEMENT t = {0,0,0};
	f_copy(&t, a);
	GF_ELEMENT c;
    GF_ELEMENT P;
    f_init(&P, a->bit_len);
    f_copy_low(&P, p);
	f_init(&c, a->bit_len);
	while (k >= n) {
		p_shift_l(&P, k-n, &c);
		p_add(&t, &c, &t);
        k = p_bit_len(&t);
	}
    
    for (u32 i=0; i<p->len; i++) {
        res->words[i] = t.words[i];
    }
    res->bit_len = p->bit_len-1;
    res->len = p->len;
    f_free(&t);
    f_free(&c);
    f_free(&P);
}

POLYNOMIAL_BASIS int p_mul(const GF_ELEMENT* a, const GF_ELEMENT* b, const GF_ELEMENT* poly, GF_ELEMENT* res) {
    if (a->bit_len != b->bit_len) return GF_ERROR;
    if (!res->bit_len) f_init(res, a->bit_len);
    GF_ELEMENT d[16];
    GF_ELEMENT tmp, c;
    f_init(&c, 2*a->bit_len-1);
    f_init(&tmp, 2*a->bit_len-1);
            //case 0:
    f_init(&d[0], a->bit_len*2-1); 
            //case 1: 
    f_init(&d[1], a->bit_len*2-1); 
    f_copy_low(&d[1], a);
            //case 2:
    f_init(&d[2], a->bit_len*2-1); 
    f_copy_low(&d[2], a);
    p_shift_l(&d[2], 1, &d[2]);
            //case 3:
    f_init(&d[3], a->bit_len*2-1); 
    f_copy_low(&d[3], a);
    p_add(&d[3], &d[2], &d[3]);
            //case 4:
    f_init(&d[4], a->bit_len*2-1); 
    f_copy_low(&d[4], a);
    p_shift_l(&d[4], 2, &d[4]);
            //case 5:
    f_init(&d[5], a->bit_len*2-1); 
    f_copy_low(&d[5], a);
    p_add(&d[5], &d[4], &d[5]);
            //case 6:
    f_init(&d[6], a->bit_len*2-1); 
    p_add(&d[6], &d[2], &d[6]);
    p_add(&d[6], &d[4], &d[6]);
            //case 7:
    f_init(&d[7], a->bit_len*2-1); 
    p_add(&d[7], &d[3], &d[7]);
    p_add(&d[7], &d[4], &d[7]);
            //case 8:
    f_init(&d[8], a->bit_len*2-1); 
    f_copy_low(&d[8], a);
    p_shift_l(&d[8], 3, &d[8]);
            //case 9:
    f_init(&d[9], a->bit_len*2-1); 
    f_copy_low(&d[9], a);
    p_add(&d[9], &d[8], &d[9]);
            //case 10:
    f_init(&d[10], a->bit_len*2-1); 
    p_add(&d[10], &d[2], &d[10]);
    p_add(&d[10], &d[8], &d[10]);
            //case 11:
    f_init(&d[11], a->bit_len*2-1); 
    p_add(&d[11], &d[3], &d[11]);
    p_add(&d[11], &d[8], &d[11]);
            //case 12:
    f_init(&d[12], a->bit_len*2-1); 
    p_add(&d[12], &d[4], &d[12]);
    p_add(&d[12], &d[8], &d[12]);
            //case 13:
    f_init(&d[13], a->bit_len*2-1); 
    p_add(&d[13], &d[5], &d[13]);
    p_add(&d[13], &d[8], &d[13]);
            //case 14:
    f_init(&d[14], 2*a->bit_len-1); 
    p_add(&d[14], &d[6], &d[14]);
    p_add(&d[14], &d[8], &d[14]);
            //case 15:
    f_init(&d[15], a->bit_len*2-1); 
    p_add(&d[15], &d[7], &d[15]);
    p_add(&d[15], &d[8], &d[15]);

    u8 t;
    WORD u = 0x0F;
    u32 bl = p_bit_len(b);
    for (u32 i=0; i<bl; i+=4) {
        t = (b->words[i/ARCH] & (u << (i%ARCH))) >> (i%ARCH);
        p_shift_l(&d[t], i, &c);
        p_add(&tmp, &c, &tmp);
    }
    if (poly)
        p_reduct(&tmp, poly, res);
    else {
        for (u32 i=0; i<res->len; i++) 
            res->words[i] = tmp.words[i];
    }

    f_free(&tmp);
    for (u32 i=0; i<16; i++)
        f_free(&d[i]);
}

POLYNOMIAL_BASIS void p_sqr(const GF_ELEMENT* a, const GF_ELEMENT* p, GF_ELEMENT* res) {
    if (!res->len) f_init(res, a->bit_len);
    GF_ELEMENT tmp;
    f_init(&tmp, 2*a->bit_len);
    for (u32 i=0; i<a->bit_len; i++) {
        if (a->words[i/ARCH] & (1L << (i%ARCH)))
            tmp.words[(2*i)/ARCH] ^= (1L << ((2*i)%ARCH));
    }
    p_reduct(&tmp, p, res);
    f_free(&tmp);
}

POLYNOMIAL_BASIS void p_pow_slow(const GF_ELEMENT* n, WORD p, const GF_ELEMENT* poly, GF_ELEMENT* res) {
    GF_ELEMENT a = {0,0,0};
    p_unity(n->bit_len, &a);
    for (u32 i=0; i<p; i++) {
        p_mul(&a, n, poly, &a);
    }
    f_copy(res, &a);
    f_free(&a);
}

POLYNOMIAL_BASIS void p_pow(const GF_ELEMENT* n, WORD p, const GF_ELEMENT* poly, GF_ELEMENT* res) {
    u32 k = word_bit_len(p);
    GF_ELEMENT a={0,0,0}, c={0,0,0};
    p_unity(n->bit_len, &c);
    f_copy(&a, n);
    for (u32 i=0; i<k; i++) {
        if (p & (1L << i)) {
            p_mul(&c, &a, poly, &c);
        }
        p_sqr(&a, poly, &a);
    }
    f_copy(res, &c);
    f_free(&c);
    f_free(&a);
}

POLYNOMIAL_BASIS void p_inv(const GF_ELEMENT* a, const GF_ELEMENT* p, GF_ELEMENT* res) {
    if (!res->len) f_init(res, a->bit_len);
    GF_ELEMENT t={0,0,0}, newt={0,0,0}, r={0,0,0}, newr={0,0,0}, q, tmp, tmp_swap;
    p_zero(a->bit_len, &t);
    p_unity(a->bit_len, &newt);
    f_init(&r, a->bit_len);
    f_copy_low(&r, p);
    f_copy(&newr, a);
    f_init(&q, a->bit_len);
    f_init(&tmp, a->bit_len);
    f_init(&tmp_swap, a->bit_len);
    while(!f_is_null(&newr)) {
        p_div(&r, &newr, &q);
        // (r, newr) = (newr, r ^ (q*newr))
        p_mul(&q, &newr, NULL, &tmp);
        p_add(&tmp, &r, &tmp);
        f_copy(&tmp_swap, &newr);
        f_copy(&newr, &tmp);
        f_copy(&r, &tmp_swap);

        // (t, newt) = (newt, r ^ (q*newt))
        p_mul(&q, &newt, NULL, &tmp);
        p_add(&tmp, &t, &tmp);
        f_copy(&tmp_swap, &newt);
        f_copy(&newt, &tmp);
        f_copy(&t, &tmp_swap);
    }
    f_copy(res, &t);
    f_free(&t); f_free(&newt); f_free(&r); f_free(&newr); f_free(&q); f_free(&tmp); f_free(&tmp_swap);
}

POLYNOMIAL_BASIS int p_trace(const GF_ELEMENT* a, const GF_ELEMENT* p) {
    
}