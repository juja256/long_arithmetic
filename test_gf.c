#include "gf.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
	GF_ELEMENT poly, a, b, c, d, e, f, zero, un={0,0,0}, t={0,0,0}, p={0,0,0};
	n_unity(233, &un);
	f_init(&f, 233);
	n_unity(233, &p);
	p.words[0] &= ~1L;
	f_init_by_str(&poly, "0x20000000000000000000000000000000000000000000000000000000213", 234);
	f_init_by_str(&a,    "0x1000000000110000000000000000AA0FF00000000000EE0000000000713", 233);
	f_init_by_str(&b,    "0x0F0A0000001100000000000000004A0FF000000000009E0000000000013", 233);
	f_init_by_str(&c,    "0x0F012320022100000000000230002A0F5001100000009E000EFEEE0044E", 233);
	f_init_by_str(&d,    "0x0F012320022100012343200230002A0F500F42434342424AABB34440441", 233);
	f_init_by_str(&e,    "0x093028584FEEEE0123432002302342333345F24343424A4AABB9387593A", 233);
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

	printf("ppow a p\n");
	f_dump(&a, 'h');
	f_dump(&p, 'h');
	p_pow(&a, &p, &poly, &a);
	f_dump(&a, 'h');

	printf("pinv a f\n");
	f_dump(&a, 'h');
	p_inv(&a, &poly, &f);
	f_dump(&f, 'h');

	printf("pmul a f\n");
	f_dump(&a, 'h');
	f_dump(&f, 'h');
	p_mul(&a, &f, &poly, &a);
	f_dump(&a, 'h');

	printf("ptr c\n");
	f_dump(&c, 'h');
	printf("%d\n", p_trace(&c, &poly));

	printf("GF(2^233), Gaussian basis of 2nd type p=467\n");
	printf("Multiplication table:\n");
	GF_ELEMENT* table_ptr3=NULL;
	GF_ELEMENT* table_ptr=NULL;
	n_init_mul_table(6, 2, &table_ptr3);
	for(u32 i=0; i<6; i++) {
		f_dump(&table_ptr3[i], 'b');
	}
	GF_ELEMENT test1, test2, test3={0,0,0};
	f_init(&test1, 6); f_init(&test2, 6);
	test1.words[0] = 7;
	test2.words[0] = 7;
	printf("nmul test1 test2\n");
	f_dump(&test1, 'b');
	f_dump(&test2, 'b');
	n_mul(&test1, &test2, table_ptr3, &test3);
	f_dump(&test3, 'b');

	n_init_mul_table(233, 2, &table_ptr);
	for(u32 i=0; i<233; i++) {
		f_dump(&table_ptr[i], 'b');
	}
	printf("ntr c\n");
	f_dump(&c, 'h');
	printf("%d\n", n_trace(&c));

	printf("nmul c unity\n");
	f_dump(&c, 'h');
	f_dump(&un, 'h');
	n_mul(&c, &un, table_ptr, &c);
	f_dump(&c, 'h');

	printf("nsqr c t\n");
	f_dump(&a, 'h');
	n_sqr(&a, &t);
	f_dump(&t, 'h');

	printf("nmul c c\n");
	f_dump(&c, 'h');
	f_dump(&c, 'h');
	n_mul(&c, &c, table_ptr, &c);
	f_dump(&c, 'h');

	printf("nmul a c c\n");
	f_dump(&a, 'h');
	f_dump(&c, 'h');
	n_mul(&a, &c, table_ptr, &c);
	f_dump(&c, 'h');

	printf("npow a p c\n");
	f_dump(&a, 'h');
	f_dump(&p, 'h');
	n_pow(&a, &p, table_ptr, &c);
	f_dump(&c, 'h');

	printf("npow2 a 2 c\n");
	f_dump(&a, 'h');
	n_pow_power_of_two(&a, 2, &c);
	f_dump(&c, 'h');
	
	printf("ninv a c\n");
	f_dump(&a, 'h');
	n_inv(&a, table_ptr, &c);
	f_dump(&c, 'h');

	printf("nmul a c\n");
	f_dump(&a, 'h');
	f_dump(&c, 'h');
	n_mul(&a, &c, table_ptr, &a);
	f_dump(&a, 'h');
	return 0;
}