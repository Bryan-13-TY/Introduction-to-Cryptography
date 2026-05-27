#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

// Polinomio irreducible para GF(2^16): x^16 + x^5 + x^3 + x + 1
#define IRREDUCIBLE 0x002B // 0000 0000 0010 1011

unsigned short multiplicar_por_x(unsigned short f);
unsigned short multiplicar_gf(unsigned short a, unsigned short b);
void multX(unsigned short f);
void multGF(unsigned short f, unsigned short g);

int main(int argc, char const *argv[])
{
    printf("=== Multiplicación por x*f(x) ===\n");
    multX(0x0001); // f(x) = 1
    multX(0x8000); // f(x) = x^15
    multX(0x4000); // f(x) = x^14
    multX(0x0020); // f(x) = x^5
    multX(0x8010); // f(x) = x^15 + x^4

    printf("\n=== Multiplicación f(x) * g(x) ===\n"); // 0111

    multGF(0x0003, 0x0005); // (x+1) * (x^2+1)
    multGF(0x8000, 0x0002); // x^15 * x
    multGF(0x00FF, 0x00AA); // (x^7 + x^6 + x^5 + x^4 + x^3 + x^2 + x + 1) * (x^7 + x^5 + x^3 + x)
    multGF(0x1234, 0x5678); // (x^12 + x^9 + x^5 + x^4 + x^2) * (x^14 + x^12 + x^10 + x^9 + x^6 + x^5 + x^4 + x^3)
    multGF(0xFFFF, 0x0001); // (x^15 + x^14 + x^13 + x^12 + x^11 + x^10 + x^9 + x^8 + x^7 + x^6 + x^5 + x^4 + x^3 + x^2 + x + 1) * 1

    return 0;
}

// Función para multiplicar por x * f(x) mod m(x)
unsigned short multiplicar_por_x(unsigned short f)
{
    unsigned short carry = f & 0x8000; // Verificar bit 15 (x^15)
    unsigned short resultado = f << 1; // Multiplicar por x

    if (carry)
    {
        resultado ^= IRREDUCIBLE;
    }

    return resultado;
}

// Función para multiplicar dos elementos f(x) y g(x) mod m(x) en GF(2^16)
unsigned short multiplicar_gf(unsigned short a, unsigned short b)
{
    unsigned short r = 0;
    unsigned short carry;
    int i;

    for (i = 0; i < 16; i++)
    {
        if (b & 1)
        {
            r ^= a;
        }
        carry = a & 0x8000; // 1 << 15 para GF(2^16)
        a <<= 1;
        if (carry)
        {
            a ^= IRREDUCIBLE;
        }
        b >>= 1;
    }

    return r;
}

// Función para imprimir en hexadecimal (Ejercicio 1)
void multX(unsigned short f)
{
    unsigned short resultado = multiplicar_por_x(f);
    printf("f(x) = %04X\n", f);
    printf("x * f(x) mod m(x) = %04X\n\n", resultado);
}

// Función para imprimir multiplicación (Ejercicio 2)
void multGF(unsigned short f, unsigned short g)
{
    unsigned short resultado = multiplicar_gf(f, g);
    printf("f(x) = %04X\n", f);
    printf("g(x) = %04X\n", g);
    printf("f(x) * g(x) mod m(x) = %04X\n\n", resultado);
}
