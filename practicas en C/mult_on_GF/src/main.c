#include "mult.h"
#include <stdio.h>
#include <stdlib.h>

#define GF 16

int main(int argc, char const *argv[]) {
    // Ejercicio 1 de la sección 1
    unsigned short r = 0b011; // x^3 + x + 1
    int n = 3;
    unsigned short tests[][2] = {
        {0b001, 0b001},
        {0b010, 0b010},
        {0b011, 0b011},
        {0b110, 0b101},
        {0b111, 0b111},
        {0b100, 0b100},
        {0b101, 0b010},
        {0b110, 0b110},
        {0b001, 0b111},
        {0b000, 0b101}
    };

    printf("\nEjercicio 1 de la seccion 1\n\n");

    for (int i = 0; i < 10; i++) {
        unsigned short a = tests[i][0];
        unsigned short b = tests[i][1];

        unsigned short c = mult(a, b, r, n);

        printf("a = ");
        for (int i = n - 1; i >= 0; i--) {
            printf("%u", (a >> i) & 1);
        }

        printf("   b = ");
        for (int i = n - 1; i >= 0; i--) {
            printf("%u", (b >> i) & 1);
        }

        printf("   c = ");
        for (int i = n - 1; i >= 0; i--) {
            printf("%u", (c >> i) & 1);
        }

        printf("\n");
    }

    // Ejercicio 1 de la sección 2
    unsigned short values[] = {
        0x1234,
        0xABCD,
        0xFFFF,
        0x8001,
        0x00F0
    };

    printf("\nEjercicio 1 de la seccion 2\n\n");

    for (int i = 0; i < 5; i++) {
        unsigned short f = values[i];
        unsigned short result = xtime(f);

        printf("f(x) = %04X -> x*f(x) = %04X\n", f, result);
    }

    // Ejercicio 2 de la sección 2
    unsigned short tests1[][2] = {
        {0x1234, 0x00F1},
        {0xABCD, 0x0002},
        {0xFFFF, 0x00FF},
        {0x8001, 0x0003},
        {0x00F0, 0x000A}
    };

    printf("\nEjercicio 2 de la seccion 2\n\n");

    for (int i = 0; i < 5; i++) {
        unsigned short f = tests1[i][0];
        unsigned short g = tests1[i][1];

        unsigned short result = fg_mult(f, g);

       printf("f(x) = %04X g(x) = %04X -> f(x)*g(x) = %04X\n", f, g, result);
    }

    return 0;
}
