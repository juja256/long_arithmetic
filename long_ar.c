#include "long_ar.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
        res->words[i] = n1->words[i] - n2->words[i] - borrow;
        if ((S_WORD)res->words[i] >= 0) {
            borrow = 0;
        }
        else {
            res->words[i] = MAX_WORD + res->words[i] + 1;
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
    
    int buf = 0;
    p = p % ARCH;
    if (p)
        for (u32 i = digits; i < n->len; i++) {
            WORD cur = n->words[i];
            res->words[i] <<= p;
            res->words[i] += buf;
            buf = (cur & (MAX_WORD << (ARCH - p))) >> (ARCH - p);
        }
}

LONG_AR_FUNC void l_mul(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res) {
    L_NUMBER tmp;
    L_NUMBER res_tmp;
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
    switch (format) {
        case 'd':
            //
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

LONG_AR_FUNC void l_sqr(const L_NUMBER* n, L_NUMBER* res) {
    L_NUMBER n2 = { 0, 0, 0 };
    l_copy(&n2, n);
    l_mul(n, &n2, res);
    l_free(&n2);
}

LONG_AR_FUNC void l_pow_slow(const L_NUMBER* n, WORD p, L_NUMBER* res) {
    res->words[0]=1;
    for (WORD i=0; i<p; i++) {
        l_mul(res, n, res);
    }
}

LONG_AR_FUNC void l_pow(const L_NUMBER* n, WORD p, L_NUMBER* res) {
    u32 k = word_bit_len(p);
    L_NUMBER a;
    L_NUMBER c;
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