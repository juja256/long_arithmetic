#include "long_ar.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* Static Begin */
#define MIN(a,b) ((a)>=(b)) ? (b) : (a);

#if defined(__GNUC__) && (ARCH == 64)
static inline u64 _add_raw(unsigned long long a, unsigned long long b, unsigned long long* c) {
  __int128 r = (__int128)a + (__int128)b;
  *c = (u64)r;
  return r >> 64;
}
static inline void _mul_raw(unsigned long long a, unsigned long long b, unsigned long long* low, unsigned long long* high) {
  __int128 r = (__int128)a * (__int128)b;
  *low = (unsigned long long)r;
  *high = r >> 64;
}
#endif

static _l_mul_func l_mul_func = l_mul;

INTERNAL_AR_FUNC static void l_mul_half_digit(const L_NUMBER* n, HALF d, L_NUMBER* res) {
    WORD carry = 0, tmp;
    for (u32 i=0; i < n->len; i+=1) {
        /* First half */
        tmp = (n->words[i] & LSB_H) * d + carry;
        res->words[i] = tmp & LSB_H;
        carry = tmp >> ARCH_2;

        /* Second half */
        tmp = ((n->words[i] & MSB_H) >> ARCH_2) * d + carry;
        res->words[i] += tmp << ARCH_2;
        carry = tmp >> ARCH_2;
    }
    if (res->len > n->len)
        res->words[n->len] = carry;
    else {
        //...
    }
}

/* Static End */
COMMON_AR_FUNC int set_l_mul_func(_l_mul_func func) {
    if (func){
        l_mul_func = func;
        return 0;
    }
    return -1;
}

COMMON_AR_FUNC u32 word_bit_len(WORD n) {
    u32 c = ARCH;
    while (c) {
        if ((((WORD)1 << (ARCH-1)) & n) >> (ARCH-1))
            return c;
        n <<= 1;
        --c;
    }
    return 0;
}

COMMON_AR_FUNC int l_init(L_NUMBER* n, u32 len) {
    n->words = (WORD*)malloc((len)*sizeof(WORD));
    memset(n->words, 0, (len)*sizeof(WORD));
    n->len = len;
    return 0;
}

COMMON_AR_FUNC int l_init_by_len(L_NUMBER* n, u32 len) {
    len = (len%ARCH==0) ? len/ARCH : len/ARCH + 1;
    n->words = (WORD*)malloc((len)*sizeof(WORD));
    memset(n->words, 0, (len)*sizeof(WORD));
    n->len = len;
    return n->len;
}

COMMON_AR_FUNC int l_init_by_str(L_NUMBER* n, const char* str) {
    unsigned s_len = strlen(str);
    u64 tmp;

    if (str[0] == '0') {
        unsigned n_len = s_len-2;
        if (str[1] == 'x') { // hexadecimal
            n->len = (n_len % ARCH_4 == 0) ? n_len / ARCH_4 : n_len / ARCH_4 + 1;
            n->words = (WORD*)malloc((n->len)*sizeof(WORD));
            memset(n->words, 0, (n->len)*sizeof(WORD));
            for (int i = s_len-1; i >= 2; i--) {
                if ((str[i] >= '0') && (str[i] <= '9')) { // 0,1,2,...
                    tmp = str[i] - 48;
                }
                else if ((str[i] >= 'A') && (str[i] <= 'F')) { // A,B,...
                    tmp = str[i] - 55;
                }
                else {
                    free(n->words);
                    return -2;
                }

                n->words[(s_len - 1 - i) / ARCH_4] ^= ( tmp << (((s_len - 1 - i) % ARCH_4)*4) );
            }
        }
        else if (str[1] == 'b') { // binary
            n->len = (n_len % ARCH == 0) ? n_len / ARCH : n_len / ARCH + 1;
            n->words = (WORD*)malloc((n->len)*sizeof(WORD));
            memset(n->words, 0, (n->len)*sizeof(WORD));
            for (u32 i = s_len-1; i >= 2; i--) {
                if (str[i] == '0') {
                    // ...
                }
                else if (str[i] == '1') {
                    n->words[(s_len - 1 - i) / ARCH] ^= (1 << ((s_len - 1 - i) % ARCH) );
                }
                else {
                    free(n->words);
                    return -2;
                }
            }
        }
        else {
            return -1;
        }
    }
    else {
        WORD bit_len = (WORD)((s_len)/0.301) + 1;
        n->len = (bit_len % ARCH == 0) ? bit_len / ARCH : bit_len / ARCH + 1; // lg(2)
        n->words = (WORD*)malloc((n->len)*sizeof(WORD));
        memset(n->words, 0, (n->len)*sizeof(WORD));
        L_NUMBER a, d, t;
        l_init(&a, n->len);
        l_init(&d, n->len);
        l_init(&t, 1); t.words[0] = 10;
        d.words[0] = 1;
        for (int i = s_len-1; i >= 0; i--) {
            if ((str[i] >= '0') && (str[i] <= '9')) {
                tmp = str[i] - 48;
                a.words[0] = tmp;
                l_mul(&a, &d, &a);
                l_add(n, &a, n);
                l_null(&a);
                l_mul(&d, &t, &d);

            }
        }
        l_free(&a);
        l_free(&d);
    }
    return 0;
}

COMMON_AR_FUNC void l_copy(L_NUMBER* dest, const L_NUMBER* source) {
    u32 len = MIN(dest->len, source->len);
    if (!dest->len) {
        dest->len = source->len;
        dest->words = (WORD*)malloc((dest->len)*sizeof(WORD));
        len = dest->len;
    }
    for (u32 i=0; i<len; i++) {
        dest->words[i] = source->words[i];
    }
}

COMMON_AR_FUNC void l_free(L_NUMBER* n) {
    free(n->words);
    n->len = 0;
}

COMMON_AR_FUNC void l_null(L_NUMBER* n) {
    for (u32 i=0; i<n->len; i++)
        n->words[i] = 0;
}

LONG_AR_FUNC int l_add(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
    WORD carry = 0;
    WORD msb_a;
    WORD msb_b;
    for (u32 i=0; i<n1->len; i++) {

        msb_a = n1->words[i] & MSB_M;
        msb_b = n2->words[i] & MSB_M;
        res->words[i] = n1->words[i] + n2->words[i] + carry;

        if ( (msb_a && msb_b) || ((msb_a ^ msb_b) && !(MSB_M & res->words[i])) )
            carry = 1;
        else
            carry = 0;
    }
    if (carry && (res->len > n1->len))
      res->words[n1->len] += carry;

    return carry;
}

LONG_AR_FUNC int l_propagate_carry(L_NUMBER* n) {
    for (u32 i=0;i<n->len;i++) {
        if (n->words[i] == MAX_WORD) {
            n->words[i]++;
            continue;
        }
        else {
            n->words[i]++;
            return 0;
        } 
    }
    return 1;
}

LONG_AR_FUNC int l_sub(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
    u8 borrow = 0;

    for (u32 i=0; i<n1->len; i++) {
        if ((n1->words[i] >= (n2->words[i] + borrow)) && !(n2->words[i] == MAX_WORD)) {
            res->words[i] = n1->words[i] - n2->words[i] - borrow;
            borrow = 0;
        }
        else {
            res->words[i] = (MAX_WORD + (n1->words[i] - n2->words[i] - borrow)) + 1;
            borrow = 1;
        }
    }
    return borrow;
}

LONG_AR_FUNC int l_sub_signed(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
  int c = l_cmp(n1, n2);
  if (c == 1) {
    l_sub(n1, n2, res);
    return 1;
  }
  else if (c == -1) {
    l_sub(n2, n1, res);
    return -1;
  }
  else {
    l_null(res);
    return 0;
  }
}

LONG_AR_FUNC int l_cmp(const L_NUMBER* n1, const L_NUMBER* n2) {
    int i = n1->len - 1;
    do {
        if (i==-1) return 0;
        else {
            if (n1->words[i] > n2->words[i]) {
                return 1;
            }
            else if (n1->words[i] < n2->words[i]) {
                return -1;
            }
        }
        i--;
    } while(n1->words[i+1] == n2->words[i+1]);
    return 0;
}


LONG_AR_FUNC void l_mul_one_digit(const L_NUMBER* n, WORD d, L_NUMBER* res) {
    #if defined(__GNUC__) && (ARCH == 64)
    u64 carry = 0, carry_tmp;
    for (WORD i=0; i < n->len; i++) {
      carry_tmp = carry;
      _mul_raw(d, n->words[i], &(res->words[i]), &carry);
      carry += _add_raw(res->words[i], carry_tmp, &(res->words[i]));

    }
    res->words[n->len] = carry;
    #else
    HALF d1 = d & LSB_H;
    HALF d2 = (d & MSB_H) >> ARCH_2;
    L_NUMBER res2;
    l_init(&res2, res->len);

    l_mul_half_digit(n, d1, res);
    l_mul_half_digit(n, d2, &res2);

    l_shift_l(&res2, ARCH_2, &res2);

    l_add(res, &res2, res);
    l_free(&res2);
    #endif

}

LONG_AR_FUNC void l_shift_l(const L_NUMBER* n, u32 p, L_NUMBER* res) {
    int digits = p / ARCH;
    int bnd = n->len-1;
    if ( res->len - n->len > digits ) bnd += digits;
    for (int i=bnd; i>=digits; i--) {
        res->words[i] = n->words[i-digits];
    }
    for (u32 i=0; i<digits; i++) {
        res->words[i] = 0;
    }
    WORD buf = 0;
    p = p % ARCH;

    if (p)
        for (u32 i = digits; i <= bnd; i++) {
            WORD cur = res->words[i];
            res->words[i] <<= p;
            res->words[i] += buf;
            buf = (cur & (MAX_WORD << (ARCH - p))) >> (ARCH - p);
        }
}

LONG_AR_FUNC void l_shift_r(const L_NUMBER* n, u32 p, L_NUMBER* res) {
    int digits = p / ARCH;
    for (int i=0; i < n->len - digits; i++) {
        res->words[i] = n->words[i+digits];
    }
    for (u32 i = n->len - digits; i < n->len; i++) {
        res->words[i] = 0;
    }
    WORD buf = 0;
    p = p % ARCH;
    if (p)
        for (int i = n->len - digits -1; i >=0; i--) {
            WORD cur = res->words[i];
            res->words[i] >>= p;
            res->words[i] += buf;
            buf = (cur & (MAX_WORD >> (ARCH - p))) << (ARCH - p);
        }
}

LONG_AR_FUNC void l_mul(const L_NUMBER* n1, const L_NUMBER* n2, AUTO_SIZE L_NUMBER* res) {
    L_NUMBER tmp, res_tmp;
    l_init(&tmp, n2->len + n1->len);
    l_init(&res_tmp, n2->len + n1->len);

    for (u32 i=0; i<n2->len; i++) {
        l_mul_one_digit(n1, n2->words[i], &tmp);
        l_shift_l(&tmp, i * ARCH, &tmp);
        l_add(&res_tmp, &tmp, &res_tmp);
        l_null(&tmp);
    }

    l_copy(res, &res_tmp);
    l_free(&tmp);
    l_free(&res_tmp);
}



LONG_AR_FUNC void l_mul_karatsuba(const L_NUMBER* a, const L_NUMBER* b, L_NUMBER* res) {

    if ( (a->len <= 4) || (a->len % 2 == 1) ) {
      l_mul(a, b, res);
      return;
    }
    L_NUMBER z0={0,0}, z1={0,0}, r1 = {0,0}, r2 = {0,0}, r3 = {0,0}, r4 = {0,0}, r5 = {0,0};
    l_init(&r1, a->len/2);
    l_init(&r2, a->len/2);
    l_init(&r3, 2*a->len);
    l_init(&r4, 2*a->len);
    l_init(&r5, 2*a->len);
    L_NUMBER A1={a->words, a->len/2}, A2={a->words + a->len/2, a->len/2},
             B1={b->words, b->len/2}, B2={b->words + b->len/2, b->len/2};
    L_NUMBER hi_r5 = {r5.words + r5.len/2, r5.len/2}, lo_r4 = {r4.words, r4.len/2};
    l_mul_karatsuba(&A1, &B1, &z0); // z0 = A1*B1
    l_mul_karatsuba(&A2, &B2, &z1); // z1 = A2*B2
    int d0 = l_sub_signed(&A2, &A1, &r1); // r1 = |A2-A1|
    int d1 = l_sub_signed(&B2, &B1, &r2); // r2 = |B2-B1|

    l_mul_karatsuba(&r1, &r2, &r3); // r3 = r1*r2
    l_shift_l(&r3, a->len * ARCH / 2, &r3); // r3 = (2^b) * r3

    l_shift_l(&z0, a->len * ARCH / 2, &r4); // r4 = (2^b) * z0
    int c = l_add(&lo_r4, &z0, &r4); // r4 = r4 + z0
    l_shift_l(&z1, a->len * ARCH / 2, &r5); // r5 = (2^b) * z1

    l_add(&hi_r5, &z1, &hi_r5); // r5 = r5 + (2^2b) * z1

    l_add(&r5, &r4, &r5); // r5 = r5 + r4

    if (d0 * d1 > 0) {
        l_sub(&r5, &r3, &r5); // r5 = r5 - r3
    }
    else {
        l_add(&r5, &r3, &r5); // r5 = r5 + r3
    }
    l_copy(res, &r5);
    l_free(&r1); l_free(&r2); l_free(&r3); l_free(&r4); l_free(&r5); l_free(&z0); l_free(&z1);
}

COMMON_AR_FUNC void l_dump(const L_NUMBER* n, char format) {
    WORD b;
    switch (format) {
        case 'd':
            b = l_bit_len(n);

            b = (WORD)(b/3.322) + 1;
            u8* digits = (u8*)malloc(sizeof(u8)*b);
            L_NUMBER ten, d, a, c;
            l_init(&ten, n->len); ten.words[0] = 10;
            l_init(&d, n->len); d.words[0] = 1;
            l_init(&c, n->len);
            l_init(&a, n->len);
            for (u32 i = 0; i<b; i++) {
                l_div(n, &d, &c, &a);
                l_div(&c, &ten, &c, &a);
                digits[i] = a.words[0];
                l_mul(&d, &ten, &d);
            }

            for (int j = b-1; j>=0; j--) {
                printf("%d", digits[j]);
            }
            printf("\n");
            l_free(&ten);
            l_free(&d);
            l_free(&a);
            l_free(&c);
            free(digits);
        break;
        case 'b':
            printf("0b");
            for (int i = ARCH * n->len - 1; i>=0; i--) {
                printf("%d", (n->words[i/ARCH] & ( (WORD)1 << (i%ARCH) )) >> (i%ARCH) );
            }

            printf("\n");
        break;
        case 'h':
            printf("0x");
            for (int i=n->len-1; i>=0; i--) {
                printf(HEX_FORMAT, n->words[i]);
            }

            printf("\n");
        break;
        default:

        break;
    }
}

LONG_AR_FUNC u32 l_bit_len(const L_NUMBER* n) {
    for (int i=n->len - 1; i >= 0; i--) {
        if (n->words[i] == 0)
            continue;
        else {
            WORD cur = n->words[i];
            u32 c = ARCH;
            while (c) {
                if ((((WORD)1 << (ARCH-1)) & cur) >> (ARCH-1))
                    return c + i * ARCH;
                cur <<= 1;
                --c;
            }
        }
    }
    return 0;
}

LONG_AR_FUNC void l_div(const L_NUMBER* a, const L_NUMBER* b, L_NUMBER* q, AUTO_SIZE L_NUMBER* r) {
    u32 k = l_bit_len(b), t;
    L_NUMBER c;
    l_copy(r, a);
    if (q)
        l_null(q);

    l_init(&c, a->len);
    while (l_cmp(r, b) != -1) {
        t = l_bit_len(r);
        l_shift_l(b, t-k, &c);
        if (l_cmp(&c, r) == 1) {
            t--;
            l_shift_l(b, t-k, &c);
        }

        l_sub(r, &c, r);
        if (q)
            q->words[(t-k)/ARCH] ^= ((WORD)1L << ((t-k)%ARCH));

    }
    l_free(&c);
}

LONG_AR_FUNC void l_sqr(const L_NUMBER* n, AUTO_SIZE L_NUMBER* res) { //Скр Скр Скр
    l_mul_func(n, n, res);
}

LONG_AR_FUNC void l_pow_slow(const L_NUMBER* n, WORD p, AUTO_SIZE L_NUMBER* res) {
    L_NUMBER a;
    l_init(&a, n->len * p);
    a.words[0]=1;
    for (WORD i=0; i<p; i++) {
        l_mul_func(&a, n, &a);
    }
    l_copy(res, &a);
    l_free(&a);
}

LONG_AR_FUNC void l_pow(const L_NUMBER* n, WORD p, AUTO_SIZE L_NUMBER* res) {
    u32 k = word_bit_len(p);
    L_NUMBER a, c;
    l_init(&c, n->len * p);
    l_init(&a, c.len);
    l_copy(&a, n);

    c.words[0] = 1;
    for (u32 i=0; i<k; i++) {
        if (p & ((WORD)1L << i)) {
            l_mul_func(&c, &a, &c);
        }
        l_sqr(&a, &a);
    }
    l_copy(res, &c);
    l_free(&a);
    l_free(&c);
}

LONG_AR_FUNC void l_pow_window(const L_NUMBER* n, WORD p, u32 win_size, L_NUMBER* d, AUTO_SIZE L_NUMBER* res) {
    u32 k = word_bit_len(p);
    u32 l = (1<<win_size);
    if (k%l == 0) k = k/l;
    else k = k/l + 1;
    WORD m = MAX_WORD >> (ARCH - win_size);

    L_NUMBER c;
    l_init(&c, n->len * p);
    c.words[0] = 1;

    if (!d) {
        d = (L_NUMBER*)malloc(sizeof(L_NUMBER)*l);
        l_init(&d[0], c.len); d[0].words[0] = 1;
        l_init(&d[1], c.len); l_copy(&d[1], n);
        for (u32 i=2; i<l; i++) {
            l_init(&d[i], c.len); l_mul_func(&d[i-1], n, &d[i]);
        }
    }

    for (int i=k-1; i>=0; i--) {
        l_mul_func(&c, &d[ (p & (m << (i * win_size))) >> i ], &c);
        if (i) {
            for (u32 j=0; j<win_size; j++) {
                l_sqr(&c, &c);
            }
        }
    }

    l_copy(res, &c);
    l_free(&c);
    for (u32 i=0; i<l; i++) {
        l_free(&d[i]);
    }
    free(d);
}
