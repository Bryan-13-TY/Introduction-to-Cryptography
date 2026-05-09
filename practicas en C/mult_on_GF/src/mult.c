#include "mult.h"
#include <stdio.h>
#include <stdlib.h>

#define REDUCTION 0x002B // x^5 + x^3 + x + 1
#define GF 16

unsigned short mult(
    unsigned short a,
    unsigned short b,
    unsigned short r,
    int n
) {
    unsigned short c = 0;

    for (int i = 0; i < n; i++) {
        if (b & 1) {
            c ^= a;
        } 

        unsigned short carry = a & (1 << (n - 1));
        a <<= 1;
        a &= ((1 << n) - 1);

        if (carry) {
            a ^= r;
        }

        b >>= 1;
    }

    return c;
}

unsigned short xtime(unsigned short f) {
    unsigned short carry = f & 0x8000;

    f <<= 1;

    if (carry) {
        f ^= REDUCTION;
    }

    return f;
}

unsigned short fg_mult(
    unsigned short f,
    unsigned short g
) {
    unsigned short result = 0;

    for (int i = 0; i < GF; i++) {
        if (g & 1) {
            result ^= f;
        } 

        f = xtime(f);

        g >>= 1;
    }

    return result;
}