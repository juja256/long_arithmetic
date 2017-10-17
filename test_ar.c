#include <stdio.h>
#include <stdlib.h>
#include "long_ar.h"
#include <time.h>

int main() {
    {
    L_NUMBER n1;
    L_NUMBER n2;
    L_NUMBER n3;
    L_NUMBER r;
    L_NUMBER q;
    L_NUMBER d;
    WORD p = 0x0A;
    l_init_by_len(&n3, 1024);
    l_init_by_len(&r, 128);
    l_init_by_len(&q, 128);
    if ( (l_init_by_str(&d,"999999999999999999102435354648748786586796964976587")) ||
         (l_init_by_str(&n1,"0xFFFFFF00000000000000FFFF") != 0) || 
         (l_init_by_str(&n2,"0xFF000000000000000AAE") != 0)) {
        fprintf(stderr, "Error\n");
        exit(-2);
    } 
    printf("Dump of 999999999999999999102435354648748786586796964976587:\n");
    l_dump(&d, 'd');
    l_dump(&d, 'h');
    l_dump(&d, 'b');
    printf("Add n1 n2: \n");
    l_dump(&n1, 'h');
    l_dump(&n2, 'h');
    l_add(&n1, &n2, &n1);
    printf("Result: \n");
    l_dump(&n1, 'h');

    int cmp = l_cmp(&n1, &n2);
    if (cmp == 1)
        printf("n1 > n2\n");
    else if (cmp == -1) 
        printf("n1 < n2\n");
    else 
        printf("n1 == n2 %d\n", cmp);

    printf("Sub n1 n2: \n");
    l_dump(&n1, 'h');
    l_dump(&n2, 'h');
    l_sub(&n1, &n2, &n1);
    printf("Result: \n");
    l_dump(&n1, 'h');
    printf("Bl n1: %d\n", l_bit_len(&n1));

    printf("Shl n1 64:\n");
    l_shift_l(&n1, ARCH, &n1);
    printf("Result: \n");
    l_dump(&n1, 'h');

    printf("Shr n1 1:\n");
    l_shift_r(&n1, 1, &n1);
    printf("Result: \n");
    l_dump(&n1, 'h');

    printf("Mul n1 n2: \n");
    l_dump(&n1, 'h');
    l_dump(&n2, 'h');
    l_mul(&n1, &n2, &n1);
    printf("Result: \n");
    l_dump(&n1, 'h');

    printf("Div n1 n2 r q: \n");
    l_dump(&n1, 'h');
    l_dump(&n2, 'h');
    l_div(&n1, &n2, &r, &q);
    printf("Result: \n");
    l_dump(&r, 'h');
    l_dump(&q, 'h');

    printf("Pow n1 p n3: \n");
    l_dump(&n1, 'h');
    printf(HEX_FORMAT"\n", p);
    l_pow(&n1, p, &n3);
    printf("Result: \n");
    l_dump(&n3, 'h');

    l_null(&n3);

    printf("PowSl n1 p n3: \n");
    l_dump(&n1, 'h');
    printf(HEX_FORMAT"\n", p);
    l_pow_slow(&n1, p, &n3);
    printf("Result: \n");
    l_dump(&n3, 'h');

    l_null(&n3);

    printf("PowW n1 p n3 4: \n");
    l_dump(&n1, 'h');
    printf(HEX_FORMAT"\n", p);
    l_pow_window(&n1, p, 4, NULL, &n3);
    printf("Result: \n");
    l_dump(&n3, 'h');

    l_free(&n1);
    l_free(&n2);
    l_free(&n3);
    l_free(&r);
    l_free(&q);



    /* Modular test */
    L_NUMBER a, a2, b, red, gcd, lcm, n, mu;
    l_init_by_len(&gcd, 128);
    l_init_by_len(&lcm, 128);
    l_init_by_len(&mu, 256);
    l_init_by_len(&a2, 128);
    l_init_by_str(&n,  "0x70000000000000000000000FF0993949"); // Modullo

    l_init_by_str(&a,  "0xE000000000000000000000FF800000AB");
    l_init_by_str(&b,  "0xE00000000000000000000FF0000000CA");
    l_init_by_str(&red,"0x70000000000000FFFF232323230001AA");

    printf("Gcd a b gcd:\n");
    l_dump(&a, 'h');
    l_dump(&b, 'h');
    m_gcd(&a, &b, &gcd);
    printf("Result:\n");
    l_dump(&gcd, 'h');

    printf("Lcm a b lcm:\n");
    l_dump(&a, 'h');
    l_dump(&b, 'h');
    m_lcm(&a, &b, &lcm);
    printf("Result:\n");
    l_dump(&lcm, 'h');

    printf("Addm a b n a:\n");
    l_dump(&a, 'h');
    l_dump(&b, 'h');
    l_dump(&n, 'h');
    m_add(&a, &b, &n, &a2);
    printf("Result:\n");
    l_dump(&a2, 'h');    

    printf("Preredc red.len n mu:\n");
    m_pre_barret(2*red.len, &n, &mu);
    l_dump(&mu, 'h');

    printf("Redc red n mu a2:\n");
    l_dump(&red, 'h');
    m_redc_barret(&red, &n, &mu, &a2);
    printf("Result:\n");
    l_dump(&a2, 'h');
    l_null(&a2);

    printf("Mul a b n mu a2\n");
    l_dump(&a, 'h');
    l_dump(&b, 'h');
    l_dump(&n, 'h');

    m_mul(&a, &b, &n, &mu, &a2);
    l_dump(&a2, 'h');

    l_null(&a2);
    printf("MulB a b n a2\n");
    l_dump(&a, 'h');
    l_dump(&b, 'h');
    l_dump(&n, 'h');

    m_mul_blakley(&a, &b, &n, &a2);
    l_dump(&a2, 'h');

    l_null(&a2);
    printf("Powm a, b, n, a2\n");
    l_dump(&a, 'h');
    l_dump(&b, 'h');
    l_dump(&n, 'h');

    m_pow(&a, &b, &n, &mu, &a2);
    l_dump(&a2, 'h');
    }
    {
        srand(0);
        WORD n_len = 256;
        L_NUMBER a,b,c={0,0}, d={0,0};
        l_init(&a, n_len); // 2048
        for (unsigned i=0;i<a.len; i++)
            a.words[i] = (u64)rand() << 32 | (u64)rand();
        l_init(&b, n_len); // 2048
        for (unsigned i=0;i<a.len; i++)
            b.words[i] = (u64)rand() << 32 | (u64)rand();

        clock_t begin = clock();
        for (u32 i=0; i<0xFFF; i++) {
            l_mul_karatsuba(&a, &b, &c);
        }
        clock_t end = clock();
        printf("Karatsuba(%d) mul time: %lf\n",n_len*ARCH, (double)(end-begin)/CLOCKS_PER_SEC ) ;

        begin = clock();
        for (u32 i=0; i<0xFFF; i++) {
            l_mul(&a, &b, &d);
        }
        end = clock();
        printf("Ordinary(%d) mul time: %lf\n\n",n_len*ARCH, (double)(end-begin)/CLOCKS_PER_SEC ) ;

        l_dump(&a, 'h');
        l_dump(&b, 'h');
        printf("Karatsuba mul res:\n");
        l_dump(&c, 'h');
        printf("Ord mul res:\n");
        l_dump(&d, 'h');

    }
    return 0;
}
