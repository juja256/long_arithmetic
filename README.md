# long_arithmetic
Classical long arithmetic and Galois field realisation in C.
  - Base for all numbers is machine word(64 or 32 bits).
  - Several methods for modular exponentation are supported. Mod operation is implemented using fast Barret reduction.
  - Library works with any-length numbers. Number length is bounded only with amount of memory, so one could allocate 128, 256, 512, 1024, 2048, 4096 bit-numbers without recompilation of library.
  - Library provides module for operating in finite Galois fields of characteristic 2. Polynomial and Normal basises are supported, however normal basis is bit slowly than polynomial.


To run test_extern_lib.c:
1.   install gmp,openssl libs:
   - gmp:     sudo apt-get install libgmp3-dev;
   - openssl: sudo apt-get install libssl-dev;
   - ntl:     http://www.shoup.net/ntl/doc/tour-unix.html
2.   compile: g++ -g -O2 test_extern_lib.c -o <exe_name> -lgmp -lssl -lcrypto -lntl -lm;
3.   help:    run ./<exe_name>

  
