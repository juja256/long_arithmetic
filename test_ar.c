#include <stdio.h>
#include <stdlib.h>
#include "long_ar.h"
#include <time.h>

void test_basic() {
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
#define K 1024
void test_fft()
{

    int N=100;
    z2n1_init_base(N);
    /*L_NUMBER a,b,c,d;
    l_init(&a, N+1);
    l_init(&b, N+1);
    l_init(&c, N+1);
    l_init(&d, 2*(N+1));

    a.words[0] = 0x00AABBFFFFAABBFF;
    a.words[1] = 0xFF00000000EEAABB;

    b.words[0] = 0x0000000000000000;
    b.words[1] = 1;

    z2n1_normalize(&a, N, &c);

    printf("ZN2n1 normalization a c:\n");
    l_dump(&a, 'h');
    l_dump(&b, 'h');


    printf("ZN2n1 add b c b:\n");
    l_dump(&b, 'h');
    l_dump(&c, 'h');
    z2n1_add(&b, &c, &b);
    l_dump(&b, 'h');

    a.words[0] = 0xF0AABBFFFFAABBFF;
    a.words[1] = 1;

    b.words[0] = 0xF000000000000000;
    b.words[1] = 1;

    printf("ZN2n1 add a b b:\n");
    l_dump(&a, 'h');
    l_dump(&b, 'h');
    z2n1_add(&a, &b, &b);
    l_dump(&b, 'h');

    printf("ZN2n1 mul by 2^n b 62 c:\n");
    b.words[1]=1;
    b.words[0]=0;
    l_dump(&b, 'h');
    z2n1_mul_by_two_power(&b, 64+64, &b);
    l_dump(&b, 'h');
    */



    L_NUMBER bb[K] =  {{0,0}};
    L_NUMBER bb2[K] = {{0,0}};
    L_NUMBER out[K] = {{0,0}};
    for (u32 i=0;i<K;i++){
      l_init(&bb[i], N+1);
      l_init(&bb2[i], N+1);
      l_init(&out[i], N+1);
      for (u32 j=0; j<N; j++)
        bb[i].words[j] = (u64)rand() << 32 | (u64)rand();
      //printf("bb[%d]:\n", i);
      //l_dump(&bb[i], 'h');
    }
    clock_t begin = clock();
    z2n1_dft_ordinary(bb, K, out);
    /*for (u32 i=0;i<K;i++) {
      printf("dft_ordinary[%d]:\n", i);
      l_dump(&out[i], 'h');
    }*/
    z2n1_dft_inv_ordinary(out, K, bb2);
    clock_t end = clock();
    printf("Ordinary dft time: %lf\n\n", (double)(end-begin)/CLOCKS_PER_SEC ) ;
    /*for (u32 i=0;i<K;i++) {
      printf("dft_inv_ordinary[%d]:\n", i);
      l_dump(&bb2[i], 'h');
    }*/

    for (u32 i=0;i<K;i++){
        //printf("bb[%d]:\n", i);
        l_copy(&bb2[i], &bb[i]);
        //l_dump(&bb[i], 'h');
    }
    printf("\n\n");

    begin = clock();
    z2n1_weighted_fft(bb, out, K);

    /*for (u32 i=0;i<K;i++){
      printf("fft[%d]:\n", i);
      l_dump(&bb[i], 'h');
    }

    printf("\n\n");*/

    z2n1_weighted_fft_inv(bb, out, K);
    end = clock();
    printf("Fft time: %lf\n\n", (double)(end-begin)/CLOCKS_PER_SEC ) ;

    for (u32 i=0;i<K;i++){
      //printf("fft_inv[%d]:\n", i);
      //l_dump(&bb[i], 'h');
      if (l_cmp(&bb[i], &bb2[i])) { printf("NOK!!!\n"); break; }
    }

    z2n1_destroy_base();
}

void test_karatsuba() {
  {
      srand(time(NULL));
      int n_len = 8;
      L_NUMBER a,b,c={0,0}, d={0,0};
      l_init(&a, n_len); // 2048
      for (unsigned i=0;i<a.len; i++)
          a.words[i] = (u64)rand() << 32 | (u64)rand();
      l_init(&b, n_len); // 2048
      for (unsigned i=0;i<a.len; i++)
          b.words[i] = (u64)rand() << 32 | (u64)rand();

      l_mul_karatsuba(&a, &b, &c);
      l_mul(&a, &b, &d);

      l_dump(&a, 'h');
      l_dump(&b, 'h');
      int cmp = l_cmp(&c, &d);
      if (cmp == 0)
          printf("Mul test passed!\n");
      else {
          printf("Ordinary mul:\n");
          l_dump(&d, 'h');
          printf("Karatsuba mul:\n");
          l_dump(&c, 'h');
          printf("%d\n", cmp);
      }


      /*clock_t begin = clock();
      for (u32 i=0; i<0xFF; i++) {
          l_mul_karatsuba(&a, &b, &c);
      }
      clock_t end = clock();
      printf("Karatsuba(%d) mul time: %lf\n",n_len*ARCH, (double)(end-begin)/CLOCKS_PER_SEC ) ;

      begin = clock();
      for (u32 i=0; i<0xFF; i++) {
          l_mul(&a, &b, &d);
      }
      end = clock();
      printf("Ordinary(%d) mul time: %lf\n\n",n_len*ARCH, (double)(end-begin)/CLOCKS_PER_SEC ) ;*/
  }
}

void test_ssa() {
  int N = 1<<10;
  //int K=1024;
  L_NUMBER a,b,c={0,0}, d={0,0};
  l_init(&a, N);
  for (unsigned i=0;i<a.len; i++)
      a.words[i] = (u64)rand() << 32 | (u64)rand();
  l_init(&b, N);
  for (unsigned i=0;i<a.len; i++)
      b.words[i] = (u64)rand() << 32 | (u64)rand();

  double eff = l_mul_shonhage_strassen(&a, &b, &c);
  printf("Efficiency: %lf\n", eff);

}

int main() {

  test_ssa();
  return 0;
}
