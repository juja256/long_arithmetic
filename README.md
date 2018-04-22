# long_arithmetic
Classical long arithmetic and Galois fields realisation in C.
  - Base for all numbers is machine word(64 or 32 bits).
  - Several methods for modular exponentation are supported. Mod operation is implemented using fast Barret reduction.
  - Library works with any-length numbers. Number length is bounded only with amount of memory, so one could allocate 128, 256, 512, 1024, 2048, 4096 bit-numbers without recompilation of library.
  - Library provides module for operating in finite Galois fields of characteristic 2. Polynomial and Normal basises are supported, however normal basis is bit slowly than polynomial.
  - Library provides three algorithms for multiplication: School algorithm that runs at O(n^2),Karatsuba's recursive algorithm - O(n^log3) and Schönhage-Strassen's algorithm - O(n*log(n)*lo(log(n))). By default inner functions such as powering use common school multiplication, inorder to change this behavior call `set_l_mul_func` with appropriate backend multiplicationfunction. Schönhage-Strassen's algorithm implementation is based onhttps://hal.inria.fr/inria-00126462v1. It accepts only numbers of equal length in format N = M *2^k, where K = 2^k is number of blocks of Mbit words input numbers are split in the decomposestage of algorithm. In current implementation additional condition must hold for M: it must bedivisible by ARCH/2, where ARCH is the size of machine word.


Generate numbers for test:
0. help: run python genNumber.py

To run test/test_extern_lib.cpp:
1.   install gmp,openssl,ntl libs:
   - gmp:     sudo apt-get install libgmp3-dev;
   - openssl: sudo apt-get install libssl-dev;
   - ntl:     http://www.shoup.net/ntl/doc/tour-unix.html
2.   compile: g++ -g -O2 test_extern_lib.cpp -o <exe_name> -lgmp -lssl -lcrypto -lntl -lm;
3.   help:    run ./<exe_name>

To run test/test_long_ar.cpp:
1. compile : g++ --std=c++11 test_long_ar.cpp ../long_ar.c ../ssa.c -o <exe_name>
2. help:     run ./<exe_name>

