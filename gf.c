#include "long_ar.h"
#include "gf.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

INTERNAL_AR_FUNC extern u32 word_bit_len(WORD n);
/* BEGIN STATIC */
#if ARCH == 32
WORD HW(WORD i) {
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
#else
static const WORD m1  = 0x5555555555555555; //binary: 0101...
static const WORD m2  = 0x3333333333333333; //binary: 00110011..
static const WORD m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
static const WORD m8  = 0x00ff00ff00ff00ff; //binary:  8 zeros,  8 ones ...
static const WORD m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...
static const WORD m32 = 0x00000000ffffffff; //binary: 32 zeros, 32 ones
static const WORD hff = 0xffffffffffffffff; //binary: all ones
static const WORD h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...
static WORD HW(WORD x) {
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits 
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits 
    return (x * h01)>>56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ... 
}
#endif

static int true_mod(int a, int b) {
    int ret = a % b;
    if (ret < 0) ret += b;
    return ret;
}

static void p_shift_l(const GF_ELEMENT* n, u32 p, GF_ELEMENT* res) {
	l_shift_l((const L_NUMBER*)n, p, (L_NUMBER*)res);
}

static void n_rot_l_1(const GF_ELEMENT* n, GF_ELEMENT* res) {
    WORD k = n->words[n->len-1] & ((WORD)1 << ((n->bit_len-1) % ARCH));
    l_shift_l((const L_NUMBER*)n, 1, (L_NUMBER*)res);
    res->words[n->len-1] &= ~((WORD)1 << (n->bit_len % ARCH));
    if (k)
        res->words[0] ^= 1;
}

/*static void n_rot_r(const GF_ELEMENT* n, u32 p, GF_ELEMENT* res) {
    WORD k = n->words[n->len-1] & (1L << ((n->bit_len-1) % ARCH));
    l_shift_l((const L_NUMBER*)n, 1, (L_NUMBER*)res);
    res->words[n->len-1] &= ~(1L << (n->bit_len % ARCH));
    if (k)
        res->words[0] ^= 1;
}*/

static void n_and(const GF_ELEMENT* a, const GF_ELEMENT* b, GF_ELEMENT* res) {
    for (u32 i=0; i<a->len; i++)
        res->words[i] = a->words[i] & b->words[i];
}

static int n_hw(const GF_ELEMENT* a) {
    int c=0;
    for (u32 i=0; i<a->len; i++) {
        c+=HW(a->words[i]);
    }
    return c;
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
        res->words[(k-n)/ARCH] ^= ((WORD)1 << ((k-n)%ARCH));
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
    return GF_SUCCESS;
}

POLYNOMIAL_BASIS void p_sqr(const GF_ELEMENT* a, const GF_ELEMENT* p, GF_ELEMENT* res) {
    if (!res->len) f_init(res, a->bit_len);
    GF_ELEMENT tmp;
    f_init(&tmp, 2*a->bit_len);
    for (u32 i=0; i<a->bit_len; i++) {
        if (a->words[i/ARCH] & ((WORD)1 << (i%ARCH)))
            tmp.words[(2*i)/ARCH] ^= ((WORD)1 << ((2*i)%ARCH));
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

POLYNOMIAL_BASIS void p_pow(const GF_ELEMENT* n, const GF_ELEMENT* p, const GF_ELEMENT* poly, GF_ELEMENT* res) {
    u32 k = p_bit_len(p);
    GF_ELEMENT a={0,0,0}, c={0,0,0};
    p_unity(n->bit_len, &c);
    f_copy(&a, n);
    for (u32 i=0; i<k; i++) {
        if (p->words[i/ARCH] & ((WORD)1 << (i%ARCH))) {
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

POLYNOMIAL_BASIS int p_trace(const GF_ELEMENT* a, const GF_ELEMENT* poly) {
    GF_ELEMENT tmp={0,0,0}, c={0,0,0};
    f_copy(&tmp, a);
    f_init(&c, a->bit_len);
    for (u32 i=0; i<a->bit_len; i++) {
        p_add(&c, &tmp, &c);
        p_sqr(&tmp, poly, &tmp);
    }
    int ret = c.words[0];
    f_free(&tmp);
    f_free(&c);
    return ret;
}

NORMAL_BASIS void n_zero(u32 bit_len, GF_ELEMENT* zero) {
    p_zero(bit_len, zero);
}

NORMAL_BASIS void n_unity(u32 bit_len, GF_ELEMENT* unity) {
    if (!unity->bit_len) f_init(unity, bit_len);
    u32 t = bit_len % ARCH;
    u32 k = (t == 0) ? unity->len : unity->len - 1;
    for (u32 i=0; i<k; i++) {
        unity->words[i] = (WORD)(-1);
    }
    if (t) {
        unity->words[unity->len-1] = ((WORD)1 << t) - 1;
    }
}

NORMAL_BASIS int n_add(const GF_ELEMENT* a, const GF_ELEMENT* b, GF_ELEMENT* res) {
    p_add(a, b, res);
}

NORMAL_BASIS int n_trace(const GF_ELEMENT* a) {
    int k=0;
    for (u32 i=0; i<a->len; i++) {
        k += HW(a->words[i]);
    }
    return k % 2;
}

NORMAL_BASIS void n_sqr(const GF_ELEMENT* a, GF_ELEMENT* res) {
    if (!res->bit_len) f_init(res, a->bit_len);
    GF_ELEMENT tmp;
    f_init(&tmp, a->bit_len);
    WORD carry = a->words[0] & 1;
    l_shift_r((const L_NUMBER*)a, 1, (L_NUMBER*)&tmp);
    if (carry)
        tmp.words[tmp.len-1] ^= ((WORD)1 << ((tmp.bit_len-1) % ARCH));
    f_copy(res, &tmp);
    f_free(&tmp);
}

NORMAL_BASIS void n_pow_power_of_two(const GF_ELEMENT* a, u32 k, GF_ELEMENT* res) {
    GF_ELEMENT t = {0,0,0};
    f_copy(&t, a);
    for (u32 i=0; i<k; i++) {
        n_sqr(&t, &t);
    }
    f_copy(res, &t);
    f_free(&t);
}

NORMAL_BASIS void n_init_mul_table(u32 bit_len, u32 type, GF_ELEMENT** table_ptr) {
    GF_ELEMENT* table = (GF_ELEMENT*)malloc(sizeof(GF_ELEMENT)*bit_len);
    for (u32 i=0; i<bit_len; i++) {
        f_init(&table[i], bit_len);
    }
    
    int p = bit_len*type + 1;
    L_NUMBER P;
    L_NUMBER tmp, tmp2, tmp_p;
    L_NUMBER un, un_inv;
    l_init_by_len(&tmp, bit_len+1);
    l_init_by_len(&tmp2, bit_len+1);
    l_init_by_len(&P, bit_len);
    l_init_by_len(&un, bit_len); un.words[0] = 1;
    l_init_by_len(&un_inv, bit_len); un_inv.words[0] = p-1;
    l_init_by_len(&tmp_p, bit_len);
    P.words[0] = p;
    int* two_powers = (int*)malloc(sizeof(int)*bit_len);
    two_powers[0] = 1;
    for (u32 i=1; i<bit_len; i++) {
        l_shift_l(&un, i, &tmp_p);
        l_div(&tmp_p, &P, NULL, &tmp_p);
        two_powers[i] = tmp_p.words[0];
    }
    for (u32 i=0; i<bit_len; i++) {
        for (u32 j=0; j<bit_len; j++) {
            if (( true_mod((two_powers[i] + two_powers[j]), p) == 1) || ( true_mod((two_powers[i] + two_powers[j]), p) == (p-1))
            || ( true_mod((two_powers[i] - two_powers[j]), p) == 1) || ( true_mod((two_powers[i] - two_powers[j]), p) == (p-1)))
                table[i].words[(bit_len-1-j)/ARCH] ^= ((WORD)1 << ((bit_len-1-j)%ARCH));
        }
    }
    *table_ptr = table;
    l_free(&P); l_free(&tmp); l_free(&tmp2); l_free(&un); l_free(&un_inv); l_free(&tmp_p);
    free(two_powers);
}

NORMAL_BASIS int n_mul(const GF_ELEMENT* a, const GF_ELEMENT* b, GF_ELEMENT* table, GF_ELEMENT* res) {
    if (a->bit_len != b->bit_len) return GF_ERROR;
    if (!res->bit_len) f_init(res, a->bit_len);
    GF_ELEMENT tmp_a={0,0,0}, tmp_b={0,0,0}, tmp, tmp_res, t;
    f_copy(&tmp_a, a);
    f_copy(&tmp_b, b);
    f_init(&tmp, a->bit_len);
    f_init(&tmp_res, a->bit_len);
    f_init(&t, a->bit_len);
    for (u32 i=0; i<a->bit_len; i++) {
        for (u32 j=0; j<a->bit_len; j++) {
            n_and(&table[j], &tmp_b, &t);
            if (n_trace(&t)) {
                tmp.words[(a->bit_len -1 - j)/ARCH] ^= ((WORD)1 << ((a->bit_len -1 - j)%ARCH));
            }
        }
        n_and(&tmp_a, &tmp, &tmp);
        if (n_trace(&tmp)) {
            tmp_res.words[(a->bit_len -1-i)/ARCH] ^= ((WORD)1 << ((a->bit_len -1-i)%ARCH));
        }
        n_rot_l_1(&tmp_a, &tmp_a);
        n_rot_l_1(&tmp_b, &tmp_b);
        f_null(&tmp);
    }
    f_copy(res, &tmp_res);
    f_free(&tmp_a); f_free(&tmp_b); f_free(&tmp); f_free(&tmp_res); f_free(&t);
    return GF_SUCCESS;
}

NORMAL_BASIS void n_pow(const GF_ELEMENT* n, const GF_ELEMENT* p, GF_ELEMENT* table, GF_ELEMENT* res) {
    u32 k = p_bit_len(p);
    GF_ELEMENT a={0,0,0}, c={0,0,0};
    n_unity(n->bit_len, &c);
    f_copy(&a, n);
    for (u32 i=0; i<k; i++) {
        if (p->words[i/ARCH] & ((WORD)1 << (i%ARCH))) {
            n_mul(&c, &a, table, &c);
        }
        n_sqr(&a, &a);
    }
    f_copy(res, &c);
    f_free(&c);
    f_free(&a);
}

NORMAL_BASIS void n_inv(const GF_ELEMENT* a, GF_ELEMENT* table, GF_ELEMENT* res) { // Itoh-Tsujii
    GF_ELEMENT b = {0,0,0}, tmp={0,0,0};
    WORD k=1;
    WORD m = a->bit_len - 1;
    u32 t = word_bit_len(m);
    f_init(&tmp, a->bit_len);
    f_copy(&b, a);
    for (int i=t-2; i>=0; i--) {
        n_pow_power_of_two(&b, k, &tmp);
        n_mul(&b, &tmp, table, &b);
        k <<= 1;
        if (m & ((WORD)1 << i)) {
            n_sqr(&b, &b);
            n_mul(&b, a, table, &b);
            k++;
        }
    }
    n_sqr(&b, res);
    f_free(&b); f_free(&tmp);
}
