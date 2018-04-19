#ifndef LONG_AR_H
#define LONG_AR_H

#ifdef __cplusplus
extern "C" {
#endif

#define AUTO_SIZE
#define LONG_AR_FUNC
#define COMMON_AR_FUNC
#define MODULAR_AR_FUNC
#define INTERNAL_AR_FUNC
#define Z2N1_AR_FUNCTION

#define ARCH 64

//#define DEBUG

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

typedef struct {
    WORD* words;
    u32 len;
} L_NUMBER;

typedef void (*_l_mul_func)(const L_NUMBER*,const L_NUMBER*, L_NUMBER*);

/* Section 0: Common functions */
COMMON_AR_FUNC int set_l_mul_func(_l_mul_func func);

COMMON_AR_FUNC int l_init(L_NUMBER* n, u32 word_len);

COMMON_AR_FUNC int l_init_by_len(L_NUMBER* n, u32 bit_len);

COMMON_AR_FUNC int l_init_by_str(L_NUMBER* n, const char* str);

COMMON_AR_FUNC void l_copy(L_NUMBER* dest, const L_NUMBER* source);

COMMON_AR_FUNC void l_free(L_NUMBER* n);

COMMON_AR_FUNC void l_dump(const L_NUMBER* n, char format);

COMMON_AR_FUNC u32 l_bit_len(const L_NUMBER* n);

COMMON_AR_FUNC u32 word_bit_len(WORD n);

COMMON_AR_FUNC void l_null(L_NUMBER* n);

/* Section 1: Standard arithmetic functions */
LONG_AR_FUNC int l_add(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res);

LONG_AR_FUNC int l_propagate_carry(L_NUMBER* n);

LONG_AR_FUNC int l_sub(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res);

LONG_AR_FUNC int l_sub_signed(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res);

LONG_AR_FUNC int l_cmp(const L_NUMBER* n1, const L_NUMBER* n2);

LONG_AR_FUNC void l_mul_one_digit(const L_NUMBER* n, WORD d, L_NUMBER* res);

LONG_AR_FUNC void l_shift_l(const L_NUMBER* n, u32 digits, L_NUMBER* res);

LONG_AR_FUNC void l_shift_r(const L_NUMBER* n, u32 digits, L_NUMBER* res);

LONG_AR_FUNC void l_mul(const L_NUMBER* n1, const L_NUMBER* n2, AUTO_SIZE L_NUMBER* res);

LONG_AR_FUNC void l_mul_karatsuba(const L_NUMBER* n1, const L_NUMBER* n2, AUTO_SIZE L_NUMBER* res);

LONG_AR_FUNC double l_mul_shonhage_strassen(const L_NUMBER* n1, const L_NUMBER* n2, AUTO_SIZE L_NUMBER* res);

LONG_AR_FUNC void l_div(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* q, AUTO_SIZE L_NUMBER* r);

LONG_AR_FUNC void l_sqr(const L_NUMBER* n, AUTO_SIZE L_NUMBER* res);

LONG_AR_FUNC void l_pow_slow(const L_NUMBER* n, WORD p, AUTO_SIZE L_NUMBER* res);

LONG_AR_FUNC void l_pow(const L_NUMBER* n, WORD p, AUTO_SIZE L_NUMBER* res);

LONG_AR_FUNC void l_pow_window(const L_NUMBER* n, WORD p, u32 win_size, L_NUMBER* d, AUTO_SIZE L_NUMBER* res);

/* Section 2: Modular arithmetic functions */
MODULAR_AR_FUNC void m_gcd(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res);

MODULAR_AR_FUNC void m_lcm(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res);

MODULAR_AR_FUNC void m_pre_barret(u32 l, const L_NUMBER* n, L_NUMBER* mu);

MODULAR_AR_FUNC void m_redc_barret(const L_NUMBER* a, const L_NUMBER* n, L_NUMBER* mu, L_NUMBER* res);

MODULAR_AR_FUNC void m_add(const L_NUMBER* n1, const L_NUMBER* n2, const L_NUMBER* n, L_NUMBER* res);

MODULAR_AR_FUNC void m_sub(const L_NUMBER* n1, const L_NUMBER* n2, const L_NUMBER* n, L_NUMBER* res);

MODULAR_AR_FUNC void m_mul(const L_NUMBER* n1, const L_NUMBER* n2, const L_NUMBER* n, L_NUMBER* mu, L_NUMBER* res);

MODULAR_AR_FUNC void m_pow(const L_NUMBER* a, const L_NUMBER* p, const L_NUMBER* n, L_NUMBER* mu, L_NUMBER* res);

MODULAR_AR_FUNC void m_sqr(const L_NUMBER* a, const L_NUMBER* n, L_NUMBER* mu, L_NUMBER* res);

MODULAR_AR_FUNC void m_mul_blakley(const L_NUMBER* n1, const L_NUMBER* n2, const L_NUMBER* n, L_NUMBER* res);

/* Section 3: Arithmetic in ring Z_{2^n + 1}*/
Z2N1_AR_FUNCTION void z2n1_init_base(WORD N);

Z2N1_AR_FUNCTION void z2n1_destroy_base();

Z2N1_AR_FUNCTION void z2n1_add(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res);

Z2N1_AR_FUNCTION void z2n1_sub(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res);

Z2N1_AR_FUNCTION void z2n1_normalize(const L_NUMBER* n, L_NUMBER* res);

Z2N1_AR_FUNCTION void z2n1_mul(const L_NUMBER* n1, const L_NUMBER* n2, L_NUMBER* res);

Z2N1_AR_FUNCTION void z2n1_mul_by_two_power(const L_NUMBER* n, WORD ex, L_NUMBER* res);

Z2N1_AR_FUNCTION void z2n1_div_by_two_power(const L_NUMBER* n, WORD ex, L_NUMBER* res);

Z2N1_AR_FUNCTION void z2n1_weighted_fft(L_NUMBER* buf, L_NUMBER* _mem, WORD K);

Z2N1_AR_FUNCTION void z2n1_weighted_fft_inv(L_NUMBER* buf, L_NUMBER* _mem, WORD K);

Z2N1_AR_FUNCTION void z2n1_fft(L_NUMBER* buf, L_NUMBER* _mem, WORD K);

Z2N1_AR_FUNCTION void z2n1_fft_inv(L_NUMBER* buf, L_NUMBER* _mem, WORD K);

Z2N1_AR_FUNCTION void z2n1_dft_ordinary(L_NUMBER* inp, WORD K, L_NUMBER* out);

Z2N1_AR_FUNCTION void z2n1_dft_inv_ordinary(L_NUMBER* inp, WORD K, L_NUMBER* out);

#ifdef __cplusplus
}
#endif

#endif
