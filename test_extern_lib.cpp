#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <openssl/bn.h>
#include <NTL/ZZ.h>

using namespace NTL;

int main(int argc, char* argv[])
{
    if(argc<5)
        {
            printf("* Test external long arithmetic libs *\n");
            // argv[2] = <number_lib> | arg[3] = <number_1> | arg[4] = <number_2> | arg[5] = base 
            printf("Usage: ./execute_program_name <number_lib> <number_1> <number_2> <base>\n");
            printf("where: <number_lib> = [GMP=1, OpenSSL=2, NTL=3, Other=4]\n");
            printf("for OpenSSL <base>  = [10, 16]\n");
            printf("for NTL <base>  = [10]\n");
            printf("Existing multiplication algorithm: \n");
            printf("for GMP(automatically choosing) : basecase, Karatsuba, Tom-Cook, FFT\n");
            printf("for Openssl : Karatsuba\n");
            printf("for NTL: Karatsuba\n");
            printf("Example: \n\n");
            printf("> ./test_ext_libs 2 A45E 34DE 16\n");
            printf("  -- multiply two hexadecimal numbers: A45E & 34DE with Openssl lib\n");
            printf("> ./test_ext_libs 1 10 10 2\n");
            printf("  -- multiply two binary numbers: 10 & 10 with GMP lib\n");
            return -1;
        }
    else if(atoi(argv[1])==1)
    {
        mpz_t a,b;
        mpz_init_set_str(a,argv[2],atoi(argv[4]));
        mpz_init_set_str(b,argv[3],atoi(argv[4]));
        mpz_mul(a,a,b);
        gmp_printf("result : %s\n", mpz_get_str(NULL,atoi(argv[4]),a));
        mpz_clear(a);
        mpz_clear(b);
        printf("\n");
        
        return 0;
    }
    else if(atoi(argv[1])==2)
    {
        BIGNUM *a   = BN_new();
        BIGNUM *b   = BN_new();
        BIGNUM *r   = BN_new();
        BN_CTX *ctx = BN_CTX_new();
        BN_init(a);
        BN_init(b);
        BN_init(r);
 
        if(atoi(argv[4])==10)
        {
            BN_dec2bn(&a, argv[2]);
            BN_dec2bn(&b, argv[3]);
            BN_mul(r,a,b,ctx);
            printf("result : %s\n", BN_bn2dec(r));
        }
        else if(atoi(argv[4])==16)
        {
            BN_hex2bn(&a, argv[2]);
            BN_hex2bn(&b, argv[3]);
            
            BN_mul(r,a,b,ctx);
            printf("result : %s\n", BN_bn2hex(r));
        }
    
        BN_free(b);
        BN_free(a);
        BN_free(r);
        BN_CTX_free(ctx);
        return 0;
    }
    else if(atoi(argv[1])==3)
    {
        ZZ a = conv<ZZ>(argv[2]);
        ZZ b = conv<ZZ>(argv[3]);
        ZZ r = a*b;
        std::cout << r << std::endl;
        return 0;
    }
    else if(atoi(argv[1])==4)
    {
        // other lib
        return 0;
    }
    else 
    {
        printf("try again....\n");
        return -1;
    }
}