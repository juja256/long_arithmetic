#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <openssl/bn.h>
#include <NTL/ZZ.h>
#include <fstream>
#include <vector>
#include <deque>
#include <algorithm>

double GetTickCount(void)
{
    struct timespec now;
    if(clock_gettime(CLOCK_MONOTONIC, &now))
        return 0;
    return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}

using namespace NTL;

int main(int argc, char* argv[])
{
    if(argc<3 || !strcmp(argv[2],"-f") && argc<5 || !strcmp(argv[2],"-c") && argc<6)
    {
        printf("* Test external long arithmetic libs *\n");
        // if argv[2] == -f ==> argv[1] = <number_lib>, argv[2] = <number_from_file_or_console>,
        // argv[3] = <base>, argv[4] = <file_name> 
        // if argv[2] == -c ==> argv[1] = <number_lib>, argv[2] = <number_from_file_or_console>, 
        // argv[3] = <number_1>, argv[4] = <number_2>, argv[5] = <base>
        printf("Usage: ./execute_program_name ");
        printf("<number_lib> <numbers_from_file_or_console> [<number_1> <number_2>][if numbers from console ~ -c] ");
        printf("<base> <file_name\n");
        printf("where: <number_lib> = [GMP=1, OpenSSL=2, NTL=3]\n");
        printf("<numbers_from_file_or_console> = [-f, -c]\n");
        printf("<base> for: GMP      = [2, 8, 10, 16]\n");
        printf("            OpenSSL  = [10, 16]\n");
        printf("            NTL      = [10]\n");
        printf("Existing multiplication algorithm: \n");
        printf("for GMP(automatically choosing) : basecase, Karatsuba, Tom-Cook, FFT\n");
        printf("for Openssl : Karatsuba\n");
        printf("for NTL:      Karatsuba\n");
        printf("Example for console test: \n\n");
        printf("> ./test_ext_libs 2 -c A45E 34DE 16\n");
        printf("  -- multiply two hexadecimal numbers: A45E & 34DE with Openssl lib\n");
        printf("> ./test_ext_libs 1 -c 10 10 2\n");
        printf("  -- multiply two binary numbers: 10 & 10 with GMP lib\n\n");
        printf("Example for test from file(numbers choosing from file): \n\n");
        printf("> ./test_ext_libs 2 -f 16 file_name\n");
        printf("  -- multiply pairs of numbers from file with Openssl lib\n");
       
        return -1;
    }
    else if(!strcmp(argv[2],"-c"))
    {
        if(atoi(argv[1])==1)
        {
            mpz_t a,b;
            mpz_init_set_str(a,argv[3],atoi(argv[5]));
            mpz_init_set_str(b,argv[4],atoi(argv[5]));
            double s1 = GetTickCount();
            mpz_mul(a,a,b);
            double s2 = GetTickCount();
            printf("GMP, time: %1f\n", s2-s1 );
            gmp_printf("result : %s\n", mpz_get_str(NULL,atoi(argv[5]),a));
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
    
            if(atoi(argv[5])==10)
            {
                BN_dec2bn(&a, argv[3]);
                BN_dec2bn(&b, argv[4]);
                double s1 = GetTickCount();
                BN_mul(r,a,b,ctx);
                double s2 = GetTickCount();
                printf("OpenSSL(Karatsuba), time: %1f\n", s2-s1 );
                printf("result : %s\n", BN_bn2dec(r));
            }
            else if(atoi(argv[5])==16)
            {
                BN_hex2bn(&a, argv[3]);
                BN_hex2bn(&b, argv[4]);
                double s1 = GetTickCount();
                BN_mul(r,a,b,ctx);
                double s2 = GetTickCount();
                printf("OpenSSL(Karatsuba), time: %1f\n", s2-s1 );
               
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
            ZZ a = conv<ZZ>(argv[3]);
            ZZ b = conv<ZZ>(argv[4]);
            
            double s1 = GetTickCount();
            ZZ r = a*b;
            double s2 = GetTickCount();
            printf("NTL(Karatsuba), time: %1f\n", s2-s1 );
            
            std::cout << "result : " << r << std::endl;
            return 0;
        }
        else 
        {
            printf("try again....\n");
            return -1;
        }
    }
    else if(!strcmp(argv[2],"-f"))
    {
        std::ifstream ifs(argv[4]);
        if(!ifs)
        {
            printf("Can't open file\n");
            return -1;
        }

        std::vector<std::string> nums;
        std::string number;
        for(ifs >> number; !ifs.eof(); ifs >> number)
        {
            nums.push_back(number);
        }
        int testNumber = nums.size()/2;

        std::deque<double> testTimes;

        if(atoi(argv[1])==1)
        {
            printf("-----Test GMP with %d runs------\n",testNumber-1);
            
            for(int i = 0; i < testNumber; ++i)
            {
                mpz_t a,b;
    
                mpz_init_set_str(a,nums[i].c_str(),atoi(argv[3]));
                mpz_init_set_str(b,nums[i+1].c_str(),atoi(argv[3]));
                
                double s1 = GetTickCount();
                mpz_mul(a,a,b);
                double s2 = GetTickCount();
                //printf("GMP, time: %1f\n", s2-s1 );
                
                testTimes.push_back(s2-s1);
                
                //gmp_printf("result : %s\n", mpz_get_str(NULL,atoi(argv[5]),a));

                mpz_clear(a);
                mpz_clear(b);
            }
            testTimes.pop_front();
        }
        else if(atoi(argv[1])==2)
        {
            printf("-----Test OpenSSL with %d runs------\n",testNumber-1);
            
            BIGNUM *a   = BN_new();
            BIGNUM *b   = BN_new();
            BIGNUM *r   = BN_new();
            BN_CTX *ctx = BN_CTX_new();
            
            if(atoi(argv[3])==10)
            {
                for(int i = 0; i < testNumber; ++i)
                {   
                    BN_init(a);
                    BN_init(b);
                    BN_init(r);
    
                    BN_dec2bn(&a, nums[i].c_str());
                    BN_dec2bn(&b, nums[i+1].c_str());
                    
                    double s1 = GetTickCount();
                    BN_mul(r,a,b,ctx);
                    double s2 = GetTickCount();
                   //printf("OpenSSL(Karatsuba), time: %1f\n", s2-s1 );

                    testTimes.push_back(s2-s1);
                
                   //printf("result : %s\n", BN_bn2dec(r));
    
                    BN_free(b);
                    BN_free(a);
                    BN_free(r);
                }

                testTimes.pop_front();
            }
            else if(atoi(argv[3])==16)
            {
                for(int i = 0; i < testNumber; ++i)
                {   
                    BN_init(a);
                    BN_init(b);
                    BN_init(r);
    
                    BN_hex2bn(&a, nums[i].c_str());
                    BN_hex2bn(&b, nums[i+1].c_str());
                    
                    double s1 = GetTickCount();
                    BN_mul(r,a,b,ctx);
                    double s2 = GetTickCount();
                    //printf("OpenSSL(Karatsuba), time: %1f\n", s2-s1 );
                    
                    testTimes.push_back(s2-s1);    
                    
                    //printf("result : %s\n", BN_bn2hex(r));
                    
                    BN_free(b);
                    BN_free(a);
                    BN_free(r);
                }
            }
        
            BN_CTX_free(ctx);
        }
        else if(atoi(argv[1])==3)
        {
            printf("-----Test NTL with %d runs------\n",testNumber-1);
            
            for(int i=0; i<testNumber; i++)
            {
                ZZ a = conv<ZZ>(nums[i].c_str());
                ZZ b = conv<ZZ>(nums[i+1].c_str());
                double s1 = GetTickCount(); 
                ZZ r = a*b;
                double s2 = GetTickCount();
                //printf("NTL(Karatsuba), time: %1f\n", s2-s1 );

                testTimes.push_back(s2-s1);  

                //std::cout << "result : " << r << std::endl;      
            }
            testTimes.pop_front();
        }
        else 
        {
            printf("try again....\n");
            return -1;
        }

        auto iter_min_time = std::min_element(std::begin(testTimes),std::end(testTimes));
        double min_time = *iter_min_time;
        printf("Min time: %1f\n", min_time);
        
        double avg_time = 0.0;
        for(auto time : testTimes)
        {
            avg_time += time;
        }
        avg_time = avg_time / (testNumber-1);
        
        printf("Avg time: %1f\n", avg_time);
    
        auto iter_max_time = std::max_element(std::begin(testTimes),std::end(testTimes));
        double max_time = *iter_max_time;
        printf("Max time: %1f\n", max_time);
        return 0;
    }
}
