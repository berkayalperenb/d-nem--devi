#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdint.h>

int64_t gcd_ll(int64_t a, int64_t b);
int mod_inverse(int64_t a, int64_t m, int64_t *inv);
int64_t mod_pow(int64_t a, int64_t b, int64_t m);
int is_prime_ll(int64_t n);
int64_t phi_ll(int64_t n);
int check_inverse(int64_t a, int64_t m);

#endif
