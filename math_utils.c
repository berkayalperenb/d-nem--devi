#include "math_utils.h"

static int64_t ll_abs(int64_t x) {
    return (x < 0) ? -x : x;
}

int64_t gcd_ll(int64_t a, int64_t b) {
    a = ll_abs(a);
    b = ll_abs(b);
    while (b != 0) {
        int64_t t = a % b;
        a = b;
        b = t;
    }
    return a;
}

static int64_t ext_gcd(int64_t a, int64_t b, int64_t *x, int64_t *y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }

    int64_t x1 = 0, y1 = 0;
    int64_t g = ext_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

int mod_inverse(int64_t a, int64_t m, int64_t *inv) {
    if (m <= 1) {
        return 0;
    }
    int64_t x = 0, y = 0;
    int64_t g = ext_gcd(a, m, &x, &y);
    if (g != 1 && g != -1) {
        return 0;
    }
    int64_t r = x % m;
    if (r < 0) {
        r += m;
    }
    *inv = r;
    return 1;
}

int64_t mod_pow(int64_t a, int64_t b, int64_t m) {
    if (m <= 0 || b < 0) {
        return -1;
    }
    if (m == 1) {
        return 0;
    }
    int64_t base = a % m;
    if (base < 0) {
        base += m;
    }
    int64_t result = 1 % m;

    while (b > 0) {
        if (b & 1LL) {
            result = (result * base) % m;
        }
        base = (base * base) % m;
        b >>= 1;
    }
    return result;
}

int is_prime_ll(int64_t n) {
    if (n <= 1) {
        return 0;
    }
    if (n <= 3) {
        return 1;
    }
    if (n % 2 == 0 || n % 3 == 0) {
        return 0;
    }
    for (int64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return 0;
        }
    }
    return 1;
}

int64_t phi_ll(int64_t n) {
    if (n <= 0) {
        return -1;
    }
    int64_t result = n;
    for (int64_t p = 2; p * p <= n; ++p) {
        if (n % p == 0) {
            while (n % p == 0) {
                n /= p;
            }
            result -= result / p;
        }
    }
    if (n > 1) {
        result -= result / n;
    }
    return result;
}

int check_inverse(int64_t a, int64_t m) {
    int64_t inv = 0;
    if (!mod_inverse(a, m, &inv)) {
        return -1;
    }
    int64_t left = ((a % m) + m) % m;
    if (((left * inv) % m) == 1) {
        return 1;
    }
    return 0;
}
