#ifndef LONG_AR_H
#define LONG_AR_H

#ifdef __cplusplus
extern "C" {
#endif

#define LONG_AR_FUNC

#define X_64 64

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned long long u64;
typedef unsigned char u8;

#ifdef X_86 
    #define ARCH 32
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
#else
    #define ARCH 64
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
#endif

typedef struct {
    WORD* words;
    u32 len;
    u8 carry;
} L_NUMBER;

LONG_AR_FUNC int l_init(L_NUMBER* n, u32 len); 

LONG_AR_FUNC int l_init_by_str(L_NUMBER* n, const char* str); 

LONG_AR_FUNC void l_copy(L_NUMBER* dest, const L_NUMBER* source);

LONG_AR_FUNC void l_free(L_NUMBER* n); 

LONG_AR_FUNC void l_dump(const L_NUMBER* n, char format);

LONG_AR_FUNC void l_add(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res); 

LONG_AR_FUNC int l_sub(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res); 

LONG_AR_FUNC int l_cmp(const L_NUMBER* n1, const L_NUMBER* n2); 

LONG_AR_FUNC void l_mul_one_digit(const L_NUMBER* n, WORD d, L_NUMBER* res); 

LONG_AR_FUNC void l_shift_l(const L_NUMBER* n, u32 digits, L_NUMBER* res); 

LONG_AR_FUNC void l_shift_r(const L_NUMBER* n, u32 digits, L_NUMBER* res); 

LONG_AR_FUNC void l_mul(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res); 

LONG_AR_FUNC void l_div(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* div, L_NUMBER* mod);

LONG_AR_FUNC void l_sqr(const L_NUMBER* n, L_NUMBER* res);

LONG_AR_FUNC void l_pow_slow(const L_NUMBER* n, WORD p, L_NUMBER* res);

LONG_AR_FUNC void l_pow(const L_NUMBER* n, WORD p, L_NUMBER* res);

LONG_AR_FUNC u32 l_bit_len(const L_NUMBER* n);

LONG_AR_FUNC void l_null(L_NUMBER* n);

#ifdef __cplusplus
}
#endif

#endif