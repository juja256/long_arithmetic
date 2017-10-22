
GXX:= gcc
CFLAGS:= -O

all: test_ar test_gf

test_ar: l.a test_ar.o
	$(GXX) $(CFLAGS) test_ar.o l.a -o test_ar

test_gf: l.a test_gf.o
	$(GXX) $(CFLAGS) test_gf.o l.a -o test_gf

test_ar.o: test_ar.c
	$(GXX) -c test_ar.c

l.a: long_ar.o mod_ar.o gf.o z2n1_ring.o
	ar rcs l.a long_ar.o mod_ar.o gf.o z2n1_ring.o

long_ar.o: long_ar.c
	$(GXX) $(CFLAGS) -c long_ar.c

mod_ar.o: mod_ar.c
	$(GXX) $(CFLAGS) -c mod_ar.c

gf.o: gf.c
	$(GXX) $(CFLAGS) -c gf.c

z2n1_ring.o: z2n1_ring.c
	$(GXX) $(CFLAGS) -c z2n1_ring.c

clear:
	rm -f *.o *.a test_ar test_gf
