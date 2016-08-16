#include <stdio.h>
#include <stdlib.h>
#include "long_ar.h"

int main() {
    L_NUMBER n1;
    L_NUMBER n2;
    L_NUMBER n3;
    L_NUMBER r;
    L_NUMBER q;
    WORD p = 0x0A;
    l_init(&n3, 32);
    l_init(&r, 2);
    l_init(&q, 2);
    if ( 
         (l_init_by_str(&n1,"0xFFFFFF00000000000000FFFF") != 0) || 
         (l_init_by_str(&n2,"0xFF000000000000000AAE") != 0)) {
        fprintf(stderr, "Error\n");
        exit(-2);
    } 
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

    printf("Mul n1 n2: \n");
    l_dump(&n1, 'h');
    l_dump(&n2, 'h');
    l_mul(&n1, &n2, &n1);
    printf("Result: \n");
    l_dump(&n1, 'h');

    printf("Div n1 n2 r q: \n");
    l_div(&n1, &n2, &r, &q);
    l_dump(&n1, 'h');
    l_dump(&n2, 'h');
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

    l_free(&n1);
    l_free(&n2);
    l_free(&n3);
    l_free(&r);
    l_free(&q);
    return 0;
}