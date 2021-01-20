#include <complex>
#include <cmath>
#include <iterator>
#include <iostream>
//From O'Reilly
//Copied implementation to compare to
using namespace std;
unsigned int bitReverse(unsigned int x, int log2n) {
    int n = 0;
//    int mask = 0x1;
    for (int i = 0; i < log2n; i++) {
        n <<= 1;
        n |= (x & 1);
        x >>= 1;
    }
    return n;
}

/**
 * @param a input vector
 * @param b output vector, Requires b.size() == a.size()
 * @param log2n the log base 2 of the size of a
 */
template<class Iter_T>
void fft(Iter_T a, Iter_T b, int log2n)
{
    typedef typename iterator_traits<Iter_T>::value_type complex;
    const complex J(0, 1);
    int n = 1 << log2n;
    const double PI = acos(-1);
    for (unsigned int i = 0; i < static_cast<unsigned int>(n); ++i) {
        b[bitReverse(i, log2n)] = a[i];
    }
    for (int s = 1; s <= log2n; ++s) {
        int m = 1 << s;
        int m2 = m >> 1;
        complex w(1, 0);
        complex wm = exp(-J * (PI / m2));
        for (int j = 0; j < m2; ++j) {
            for (int k = j; k < n; k += m) {
                complex t = w * b[k + m2];
                complex u = b[k];
                b[k] = u + t;
                b[k + m2] = u - t;
            }
            w *= wm;
        }
    }
}