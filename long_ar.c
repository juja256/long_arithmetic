#include "long_ar.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* Static Begin */

static void l_mul_half_digit(const L_NUMBER* n, HALF d, L_NUMBER* res) {
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

static u32 word_bit_len(WORD n) {
    u32 c = ARCH;
    while (c) {
        if ((((WORD)1 << (ARCH-1)) & n) >> (ARCH-1))
            return c;
        n <<= 1;
        --c;
    }
    return 0;
}

/* Static End */


LONG_AR_FUNC int l_init(L_NUMBER* n, u32 len) {
    n->words = (WORD*)malloc((len)*sizeof(WORD));
    memset(n->words, 0, (len)*sizeof(WORD));
    n->len = len;
    n->carry = 0;
    return 0;
}

LONG_AR_FUNC int l_init_by_str(L_NUMBER* n, const char* str) {
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
    n->carry = 0;
    return 0;
}

LONG_AR_FUNC void l_copy(L_NUMBER* dest, const L_NUMBER* source) {
    if (!dest->len) {
        dest->len = source->len;
        dest->carry = source->carry;
        dest->words = (WORD*)malloc((dest->len)*sizeof(WORD));
    }
    for (u32 i=0; i<source->len; i++) {
        dest->words[i] = source->words[i];
        dest->carry = source->carry;
    }
}

LONG_AR_FUNC void l_free(L_NUMBER* n) {
    free(n->words);
    n->len = 0;
}

LONG_AR_FUNC void l_null(L_NUMBER* n) {
    for (u32 i=0; i<n->len; i++)
        n->words[i] = 0;
}

LONG_AR_FUNC void l_add(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
    u8 carry = 0;
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
    res->carry = carry;
    res->len = n1->len; 
}

LONG_AR_FUNC int l_sub(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
    u8 borrow = 0;

    for (u32 i=0; i<n1->len; i++) {
        if (n1->words[i] >= n2->words[i] + borrow) {
            res->words[i] = n1->words[i] - n2->words[i] - borrow;
            borrow = 0;
        }
        else {
            res->words[i] = (MAX_WORD + (n1->words[i] - n2->words[i] - borrow)) + 1;
            borrow = 1;   
        }
    }
    res->carry = 0;
    return borrow;
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
}

LONG_AR_FUNC void l_mul_one_digit(const L_NUMBER* n, WORD d, L_NUMBER* res) {
    HALF d1 = d & LSB_H;
    HALF d2 = (d & MSB_H) >> ARCH_2;
    L_NUMBER res2;
    l_init(&res2, res->len);

    l_mul_half_digit(n, d1, res);
    l_mul_half_digit(n, d2, &res2);

    l_shift_l(&res2, ARCH_2, &res2);
    l_add(res, &res2, res);

    l_free(&res2);
}

LONG_AR_FUNC void l_shift_l(const L_NUMBER* n, u32 p, L_NUMBER* res) {
    int digits = p / ARCH;
    for (int i=n->len-1; i>=digits; i--) {
        res->words[i] = n->words[i-digits];
    }
    for (u32 i=0; i<digits; i++) {
        res->words[i] = 0;
    } 
    WORD buf = 0;
    p = p % ARCH;
    if (p)
        for (u32 i = digits; i < n->len; i++) {
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
            WORD cur = n->words[i];
            res->words[i] >>= p;
            res->words[i] += buf;
            buf = (cur & (MAX_WORD >> (ARCH - p))) << (ARCH - p);
        }
}

LONG_AR_FUNC void l_mul(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
    L_NUMBER tmp, res_tmp;
    l_init(&tmp, 2 * n1->len);
    l_init(&res_tmp, res->len);

    for (u32 i=0; i<n2->len; i++) {
        l_mul_one_digit(n1, n2->words[i], &tmp);
        l_shift_l(&tmp, i * ARCH, &tmp);
        l_add(&res_tmp, &tmp, &res_tmp);
    }

    l_copy(res, &res_tmp);
    l_free(&tmp);
    l_free(&res_tmp);
}

LONG_AR_FUNC void l_dump(const L_NUMBER* n, char format) {
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
                //printf("%d\n", i);
                //l_dump(&a, 'h');
                //l_dump(&c, 'h');
                l_div(&c, &ten, &c, &a);
                //printf("\n");
                //l_dump(&a, 'h');
                //l_dump(&c, 'h');
                //printf("\n");
                digits[i] = a.words[0];
                //l_null(&c);
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
            if (n->carry)
                printf(", Carry: 1");
            printf("\n");
        break;
        case 'h':
            printf("0x");
            for (int i=n->len-1; i>=0; i--) {
                printf(HEX_FORMAT, n->words[i]);
            }
            if (n->carry)
                printf(", Carry: 1");
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

LONG_AR_FUNC void l_div(const L_NUMBER* a, const L_NUMBER* b, L_NUMBER* q, L_NUMBER* r) {
    u32 k = l_bit_len(b), t;
    L_NUMBER c;
    l_copy(r, a);
    l_null(q);
    if (!q->len)
        l_init(q, a->len);
    l_init(&c, a->len);
    while (l_cmp(r, b) != -1) {
        t = l_bit_len(r);
        l_shift_l(b, t-k, &c);
        
        
        if (l_cmp(&c, r) == 1) {
            l_shift_l(b, (--t)-k, &c);
            
        }
        l_sub(r, &c, r);
        q->words[(t-k)/ARCH] ^= ((WORD)1 << ((t-k)%ARCH));
        
    }
    l_free(&c);
}

LONG_AR_FUNC void l_sqr(const L_NUMBER* n, L_NUMBER* res) { //Скр Скр Скр
    L_NUMBER n2 = { 0, 0, 0 };
    l_copy(&n2, n);
    l_mul(n, &n2, res);
    l_free(&n2);
}

LONG_AR_FUNC void l_pow_slow(const L_NUMBER* n, WORD p, L_NUMBER* res) {
    L_NUMBER a;
    l_init(&a, res->len);
    a.words[0]=1;
    for (WORD i=0; i<p; i++) {
        l_mul(&a, n, &a);
    }
    l_copy(res, &a);
    l_free(&a);
}

LONG_AR_FUNC void l_pow(const L_NUMBER* n, WORD p, L_NUMBER* res) {
    u32 k = word_bit_len(p);
    L_NUMBER a, c;
    l_init(&c, res->len);
    l_init(&a, res->len);
    l_copy(&a, n);

    c.words[0] = 1;

    for (u32 i=0; i<k; i++) {
        if (p & (1 << i)) {
            l_mul(&c, &a, &c);
        }
        l_sqr(&a, &a);
    }

    l_copy(res, &c);
    l_free(&a);
    l_free(&c);
}

LONG_AR_FUNC void l_pow_window(const L_NUMBER* n, WORD p, L_NUMBER* res, u32 win_size) {
    u32 k = word_bit_len(p);
    u32 l = (1<<win_size);
    if (k%l == 0) k = k/l;
    else k = k/l + 1;
    WORD m = MAX_WORD >> (ARCH - win_size);

    L_NUMBER c;
    l_init(&c, res->len);
    c.words[0] = 1;

    L_NUMBER* d = (L_NUMBER*)malloc(sizeof(L_NUMBER)*l);
    l_init(&d[0], res->len); d[0].words[0] = 1;
    l_init(&d[1], res->len); l_copy(&d[1], n);
    for (u32 i=2; i<l; i++) {
        l_init(&d[i], res->len); l_mul(&d[i-1], n, &d[i]);
    }

    for (int i=k-1; i>=0; i--) {
        l_mul(&c, &d[ (p & (m << (i * win_size))) >> i ], &c);
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

