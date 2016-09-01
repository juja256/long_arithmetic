#include "gf.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
	GF_ELEMENT poly, a, b, c, d, e, f, zero;
	f_init(&f, 233);
	f_init_by_str(&poly, "0x20000000000000000000000000000000000000000000000000000000213", 234);
	f_init_by_str(&a,    "0x1000000000110000000000000000AA0FF00000000000EE0000000000713", 233);
	f_init_by_str(&b,    "0x0F0A0000001100000000000000004A0FF000000000009E0000000000013", 233);
	printf("GF(2^233), polynomial basis, p(x) = x^233 + x^9 + x^4 + x + 1\n");
	printf("poly:\n");
	f_dump(&poly, 'h');

	printf("padd a b\n");
	f_dump(&a, 'h');
	f_dump(&b, 'h');
	p_add(&a, &b, &a);
	f_dump(&a, 'h');

	printf("psqr a\n");
	f_dump(&a, 'h');
	p_sqr(&a, &poly, &a);
	f_dump(&a, 'h');

	printf("pmul a b\n");
	f_dump(&a, 'h');
	f_dump(&b, 'h');
	p_mul(&a, &b, &poly, &a);
	f_dump(&a, 'h');

	printf("ppow a 1024\n");
	f_dump(&a, 'h');
	p_pow(&a, 1024, &poly, &a);
	f_dump(&a, 'h');

	printf("pinv a c\n");
	f_dump(&a, 'h');
	p_inv(&a, &poly, &c);
	f_dump(&c, 'h');

	printf("pmul a c\n");
	f_dump(&a, 'h');
	f_dump(&c, 'h');
	p_mul(&a, &c, &poly, &a);
	f_dump(&a, 'h');
    /* code */
    return 0;
}