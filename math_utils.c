/*
 * Ad Soyad   : Berkay Alperen Bayram
 * Ogrenci No : 439541
 */
#include "math_utils.h"

// Sayının mutlak değerini döndüren yardımcı fonksiyon
static int64_t ll_abs(int64_t x) {
    return (x < 0) ? -x : x;
}

// Öklid Algoritması ile En Büyük Ortak Bölen (EBOB - GCD) hesabı
// Zaman Karmaşıklığı: O(log(min(a, b)))
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

// Genişletilmiş Öklid Algoritması
// a*x + b*y = GCD(a, b) denklemini sağlayan x ve y katsayılarını hesaplar
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

// Modüler Ters Hesaplama (INV)
// (a * inv) % m = 1 eşitliğindeki inv değerini bulur. 
// Zaman Karmaşıklığı: O(log(min(a, m)))
int mod_inverse(int64_t a, int64_t m, int64_t *inv) {
    if (m <= 1) {
        return 0; // m <= 1 için ters yoktur
    }
    int64_t x = 0, y = 0;
    int64_t g = ext_gcd(a, m, &x, &y);
    // a ve m aralarında asal değilse (GCD != 1) modüler ters yoktur
    if (g != 1 && g != -1) {
        return 0;
    }
    // Negatif katsayıları pozitif formata çevirme: x = (x % M + M) % M
    int64_t r = x % m;
    if (r < 0) {
        r += m;
    }
    *inv = r;
    return 1;
}

// İkili Üs Alma (Binary Exponentiation - POW)
// (a^b) % m işlemini hızlı hesaplar. Zaman Karmaşıklığı: O(log b)
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
        // Üs tek sayı ise sonucu tabanla çarpıp modunu al
        if (b & 1LL) {
            result = (result * base) % m;
        }
        // Tabanın karesini alıp modunu alarak ilerle (overflow koruması)
        base = (base * base) % m;
        b >>= 1; // Üssü ikiye böl
    }
    return result;
}

// Asallık Testi (PRIME)
// O(sqrt(N)) zaman karmaşıklığında çalışır
int is_prime_ll(int64_t n) {
    if (n <= 1) {
        return 0; // N <= 1 reddedilir
    }
    if (n <= 3) {
        return 1; // 2 ve 3 asaldır
    }
    // Çift sayıları ve 3'ün katlarını baştan eliyoruz
    if (n % 2 == 0 || n % 3 == 0) {
        return 0;
    }
    // 6k - 1 ve 6k + 1 formundaki sayılara bölerek kontrol
    for (int64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return 0;
        }
    }
    return 1;
}

// Euler Totient Fonksiyonu (PHI)
// O(sqrt(N)) zaman karmaşıklığında çalışır
int64_t phi_ll(int64_t n) {
    if (n <= 0) {
        return -1;
    }
    int64_t result = n;
    // Asal çarpanlara ayırma yöntemi
    for (int64_t p = 2; p * p <= n; ++p) {
        if (n % p == 0) {
            while (n % p == 0) {
                n /= p;
            }
            // Hassasiyet kaybını (precision loss) önlemek için
            // sonuc = sonuc - (sonuc / p) formülü kullanılıyor
            result -= result / p;
        }
    }
    if (n > 1) {
        result -= result / n;
    }
    return result;
}

// CHECK Komutu: A'nın M modundaki tersinin doğruluğunu test eder
// Tersi alınır, sonuç A ile çarpılıp M'ye göre modu alınır
int check_inverse(int64_t a, int64_t m) {
    int64_t inv = 0;
    if (!mod_inverse(a, m, &inv)) {
        return -1; // Ters hesaplanamadı
    }
    // left * inv % m == 1 doğruluğunu test et
    int64_t left = ((a % m) + m) % m;
    if (((left * inv) % m) == 1) {
        return 1; // Hesaplama doğru
    }
    return 0; // Hesaplama yanlış
}
