#ifndef GF_H
#define GF_H

#ifdef __cplusplus
extern "C" {
#endif

#define COMMON_GF
#define POLYNOMIAL_BASIS
#define NORMAL_BASIS

#ifndef ARCH
#define ARCH 64
#endif

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned long long u64;
typedef unsigned char u8;

#if ARCH == 32
    #define ARCH_2 16
    #define ARCH_4 8
    #define WORD u32
    #define S_WORD int
    #define HALF u16
    #define MSB_M 0x80000000
    #define LSB_H 0x0000ffff
    #define MSB_H 0xffff0000
    #define MAX_WORD ((u32)(-1))
    #define HEX_FORMAT "%.8X"
#elif ARCH == 64
    #define ARCH_2 32
    #define ARCH_4 16
    #define WORD u64
    #define HALF u32
    #define S_WORD long long int
    #define MSB_M 0x8000000000000000
    #define LSB_H 0x00000000ffffffff
    #define MSB_H 0xffffffff00000000
    #define MAX_WORD ((u64)(-1))
    #define HEX_FORMAT "%.16llX"
#else
    #error "Unknown word length!"
#endif

#define GF_SUCCESS 0
#define GF_ERROR -1

typedef struct {
    WORD* words;
    u32 len;
    u32 bit_len;
} GF_ELEMENT;

COMMON_GF void f_init(GF_ELEMENT* a, u32 bit_len);

COMMON_GF void f_init_by_str(GF_ELEMENT* a, const char* str, u32 bit_len);

COMMON_GF void f_copy(GF_ELEMENT* dest, const GF_ELEMENT* source);

COMMON_GF void f_dump(const GF_ELEMENT* a, char format);

COMMON_GF void f_free(GF_ELEMENT* a);

/* Polynomial basis */
POLYNOMIAL_BASIS u32 p_bit_len(const GF_ELEMENT* n);

POLYNOMIAL_BASIS void p_zero(u32 bit_len, GF_ELEMENT* zero);

POLYNOMIAL_BASIS void p_unity(u32 bit_len, GF_ELEMENT* unity);

POLYNOMIAL_BASIS int p_add(const GF_ELEMENT* a, const GF_ELEMENT* b, GF_ELEMENT* res);

POLYNOMIAL_BASIS int p_mul(const GF_ELEMENT* a, const GF_ELEMENT* b, const GF_ELEMENT* p, GF_ELEMENT* res);

POLYNOMIAL_BASIS void p_inv(const GF_ELEMENT* a, const GF_ELEMENT* p, GF_ELEMENT* res);

POLYNOMIAL_BASIS void p_sqr(const GF_ELEMENT* a, const GF_ELEMENT* p, GF_ELEMENT* res);

POLYNOMIAL_BASIS void p_pow(const GF_ELEMENT* a, const GF_ELEMENT* n, const GF_ELEMENT* p, GF_ELEMENT* res);

POLYNOMIAL_BASIS void p_pow_slow(const GF_ELEMENT* n, WORD p, const GF_ELEMENT* poly, GF_ELEMENT* res);

POLYNOMIAL_BASIS void p_reduct(const GF_ELEMENT* a, const GF_ELEMENT* p, GF_ELEMENT* res);

POLYNOMIAL_BASIS int p_trace(const GF_ELEMENT* a, const GF_ELEMENT* p);

/* Normal basis */
NORMAL_BASIS void n_zero(u32 bit_len, GF_ELEMENT* zero);

NORMAL_BASIS void n_unity(u32 bit_len, GF_ELEMENT* unity);

NORMAL_BASIS int n_add(const GF_ELEMENT* a, const GF_ELEMENT* b, GF_ELEMENT* res);

NORMAL_BASIS void n_init_mul_table(u32 bit_len, u32 type, GF_ELEMENT** table);

NORMAL_BASIS int n_mul(const GF_ELEMENT* a, const GF_ELEMENT* b, GF_ELEMENT* table, GF_ELEMENT* res);

NORMAL_BASIS void n_inv(const GF_ELEMENT* a, GF_ELEMENT* table, GF_ELEMENT* res);

NORMAL_BASIS void n_sqr(const GF_ELEMENT* a, GF_ELEMENT* res);

NORMAL_BASIS void n_pow_power_of_two(const GF_ELEMENT* a, u32 k, GF_ELEMENT* res);

NORMAL_BASIS void n_pow(const GF_ELEMENT* a, const GF_ELEMENT* n, GF_ELEMENT* table, GF_ELEMENT* res);

NORMAL_BASIS int n_trace(const GF_ELEMENT* a);

#ifdef __cplusplus
}
#endif

#endif