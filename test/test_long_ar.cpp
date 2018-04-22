#include <stdio.h>
#include <stdlib.h>
#include "../long_ar.h"

#include <cstring>
#include <fstream>
#include <vector>
#include <deque>
#include <algorithm>

#ifdef _WIN64
#include <intrin.h>
#pragma intrinsic(_umul128) 
#include <windows.h>
#else
#include <x86intrin.h>
#include <time.h>
double GetTickCount(void) 
{
  struct timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now))
    return 0;
  return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}
#endif // _WIN64


int main(int argc, char* argv[])
{
  if(argc<2 || !strcmp(argv[1],"-f") && argc<4 || !strcmp(argv[1],"-c") && argc<5)
  {
    printf("* Test long_ar lib *\n");
    // if argv[1] == -f ==> argv[1] = <number_from_file_or_console>, argv[2] = <algorithm>
    // argv[3] = <file_name>, argv[4] = <verbose_output> 
    // if argv[1] == -c ==> argv[1] = <number_from_file_or_console>, argv[2] = <algorithm>
    // argv[3] = <number_1>, argv[4] = <number_2>
    printf("Usage: ./execute_program_name ");
    printf("<numbers_from_file_or_console> [<number_1> <number_2>][if numbers from console ~ -c] ");
    printf("<file_name>[if numbers from file ~ -f] <verbose output ~ -verbose>[if numbers from file ~f]\n");
    printf("where: <numbers_from_file_or_console> = [-f, -c]\n");
    printf("       <algorithm> = [School=1, Karatsuba = 2, SS = 3]\n");
    printf("       <number_1,2> = input value with format prefix : \n");
    printf("             \"0x\"             : for numbers in hex - \n");
    printf("             \"0b\"             : for numbers in bin - \n");
    printf("             \"without_prefix\" : for numbers in dec - \n");
    printf("Example for console test: \n\n");
    printf("> ./test_long_ar -c 2 0xA45E 0x34DE\n");
    printf("  -- multiply two hexadecimal numbers: A45E & 34DE with karatsuba algorithm\n");
    printf("> ./test_long_ar -c 3 0b10 0b10\n");
    printf("  -- multiply two binary numbers: 10 & 10 with Schönhage-Strassen's algorithm\n\n");
    printf("Example for test multiplication numbers from file: \n\n");
    printf("> ./test_long_ar -f 1 file_name -verbose\n");
    printf("  -- multiply pairs of numbers from file with school algorithm and output results of multiplication\n");
    printf("> ./test_long_ar -f 1 file_name\n");
    printf("  -- multiply pairs of numbers from file with school algorithm and output time of tests\n");
    

    return -1;
  }
  else if(!strcmp(argv[1],"-c"))
  {
    if(atoi(argv[2])==1)
    {
      L_NUMBER a;
      L_NUMBER b;
      L_NUMBER res_sch = {0,0};
      l_init_by_str(&a,argv[3]);
      l_init_by_str(&b,argv[4]);
      
      printf("A: "); l_dump(&a, 'h');
      printf("B: "); l_dump(&b, 'h');
      double s1 = GetTickCount();
      l_mul(&a, &b, &res_sch);
      double s2 = GetTickCount();
     
      printf("C_school, time: %lf\n", s2-s1);
      l_dump(&res_sch, 'h');
      
      l_free(&res_sch);
      l_free(&b);
      l_free(&a);
      
      return 0;
    }
    else if(atoi(argv[2])==2)
    {
      L_NUMBER a;
      L_NUMBER b;
      L_NUMBER res_kar = {0,0};
      l_init_by_str(&a,argv[3]);
      l_init_by_str(&b,argv[4]);
     
      printf("A: "); l_dump(&a, 'h');
      printf("B: "); l_dump(&b, 'h');
      double s1 = GetTickCount();
      l_mul_karatsuba(&a, &b, &res_kar);
      double s2 = GetTickCount();

      printf("C_karatsuba, time: %lf\n", s2-s1); 
      l_dump(&res_kar, 'h');
    
      l_free(&res_kar);
      l_free(&b);
      l_free(&a);
      return 0;
   }
    else if(atoi(argv[2])==3)
    {
      L_NUMBER a;
      L_NUMBER b;
      L_NUMBER res_ss = {0,0};
      l_init_by_str(&a,argv[3]);
      l_init_by_str(&b,argv[4]);
      
      printf("A: "); l_dump(&a, 'h');
      printf("B: "); l_dump(&b, 'h');
      double s1 = GetTickCount();
      double eff = l_mul_shonhage_strassen(&a, &b, &res_ss);
      double s2 = GetTickCount();
      
      printf("C_ssa, time: %lf\n", s2-s1); 
      l_dump(&res_ss, 'd');
    
      l_free(&res_ss);
      l_free(&b);
      l_free(&a);
      return 0;
    }
    else 
    {
          printf("try again....\n");
          return -1;
    }
  }
  else if(!strcmp(argv[1],"-f"))
  {
    
    std::ifstream ifs(argv[3]);
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
    ifs.close();
    int testNumber = nums.size()/2;
    std::deque<double> testTimes;
    
    if(atoi(argv[2])==1)
    {
      printf("-----Test School algortihm with %d runs------\n",testNumber-1);
      
      for(int i = 0; i < testNumber; ++i)
      {  
        L_NUMBER a;
        L_NUMBER b;
        L_NUMBER res_sch = {0,0};
        l_init_by_str(&a,nums[i].c_str());
        l_init_by_str(&b,nums[i+1].c_str());

        if(argc > 4 && !strcmp(argv[4],"-verbose"))
        {
          printf("A: "); l_dump(&a, 'h');
          printf("B: "); l_dump(&b, 'h');
        }

        double s1 = GetTickCount();
        l_mul(&a, &b, &res_sch);
        double s2 = GetTickCount();

        testTimes.push_back(s2-s1);

        if(argc > 4 && !strcmp(argv[4],"-verbose"))
        {
          printf("C_school, time: %lf\n", s2-s1);
          l_dump(&res_sch, 'h');
        }

        l_free(&res_sch);
        l_free(&b);
        l_free(&a);
      }
      testTimes.pop_front();
    }
    else if(atoi(argv[2])==2)
    {
      printf("-----Test Karatsuba algorithm with %d runs------\n",testNumber-1);

      for(int i = 0; i < testNumber; ++i)
      {     
        L_NUMBER a;
        L_NUMBER b;
        L_NUMBER res_kar = {0,0};
        l_init_by_str(&a,nums[i].c_str());
        l_init_by_str(&b,nums[i].c_str());
        
        if(argc > 4 && !strcmp(argv[4],"-verbose"))
        {
          printf("A: "); l_dump(&a, 'h');
          printf("B: "); l_dump(&b, 'h');
        }
        double s1 = GetTickCount();
        l_mul_karatsuba(&a, &b, &res_kar);
        double s2 = GetTickCount();

        testTimes.push_back(s2-s1);

        if(argc > 4 && !strcmp(argv[4],"-verbose"))
        {
          printf("C_karatsuba, time: %lf\n", s2-s1); 
          l_dump(&res_kar, 'h');
        }

        l_free(&res_kar);
        l_free(&b);
        l_free(&a);
      }
      testTimes.pop_front();
    }
    else if(atoi(argv[2])==3)
    {
      printf("-----Test SS algorithm with %d runs------\n",testNumber-1);
            
      for(int i = 0; i < testNumber; ++i)
      {     
        L_NUMBER a;
        L_NUMBER b;
        L_NUMBER res_ss = {0,0};
        l_init_by_str(&a,nums[i].c_str());
        l_init_by_str(&b,nums[i].c_str());

        if(argc > 4 && !strcmp(argv[4],"-verbose"))
        {
          printf("A: "); l_dump(&a, 'h');
          printf("B: "); l_dump(&b, 'h');
        }
        double s1 = GetTickCount();
        double eff = l_mul_shonhage_strassen(&a, &b, &res_ss);
        double s2 = GetTickCount();
        
        testTimes.push_back(s2-s1);

        if(argc > 4 && !strcmp(argv[4],"-verbose"))
        {
          printf("C_ssa, time: %lf\n", s2-s1); 
          l_dump(&res_ss, 'd');
        }
        l_free(&res_ss);
        l_free(&b);
        l_free(&a);
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