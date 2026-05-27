#include <stdio.h>
#include <stdint.h>

// Polinomio irreducible para GF(2^16): x^16 + x^5 + x^3 + x + 1
#define IRREDUCIBLE 0x002B // 0000 0000 0010 1011

uint16_t multiplicar_por_x(uint16_t f);
uint16_t multiplicar_gf(uint16_t a, uint16_t b);
void multX(uint16_t f);
void multGF(uint16_t f, uint16_t g);

int main()
{
    printf("=== Multiplicación por x*f(x) ===\n");

    multX(0x0001); // f(x) = 1
    multX(0x8000); // f(x) = x^15
    multX(0x4000); // f(x) = x^14
    multX(0x0020); // f(x) = x^5
    multX(0x8010); // f(x) = x^15 + x^4

    printf("\n=== Multiplicación f(x) * g(x) ===\n");

    multGF(0x0003, 0x0005); // (x+1) * (x^2+1)
    multGF(0x8000, 0x0002); // x^15 * x
    multGF(0x00FF, 0x00AA); // polinomios aleatorios
    multGF(0x1234, 0x5678); // valores más complejos
    multGF(0xFFFF, 0x0001); // elemento máximo * 1

    return 0;
}

// Función para multiplicar por x * f(x) mod m(x)
uint16_t multiplicar_por_x(uint16_t f)
{
    uint16_t carry = f & 0x8000; // Verificar bit 15 (x^15)
    uint16_t resultado = f << 1; // Multiplicar por x

    if (carry)
    {
        resultado ^= IRREDUCIBLE;
    }

    return resultado;
}

// Función para multiplicar dos elementos f(x) y g(x) mod m(x) en GF(2^16)
uint16_t multiplicar_gf(uint16_t a, uint16_t b)
{
    uint16_t r = 0;
    uint16_t carry;
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
void multX(uint16_t f)
{
    uint16_t resultado = multiplicar_por_x(f);
    printf("f(x) = %04X\n", f);
    printf("x * f(x) mod m(x) = %04X\n\n", resultado);
}

// Función para imprimir multiplicación (Ejercicio 2)
void multGF(uint16_t f, uint16_t g)
{
    uint16_t resultado = multiplicar_gf(f, g);
    printf("f(x) = %04X\n", f);
    printf("g(x) = %04X\n", g);
    printf("f(x) * g(x) mod m(x) = %04X\n\n", resultado);
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void multiplicar_por_x(unsigned short f);

int main()
{
    srand(time(NULL));
    unsigned short f;
    printf("Ingrese un número hexadecimal de 4 dígitos (16 bits): ");
    scanf("%hx", &f);
    multiplicar_por_x(f);

    return 0;
}
void multiplicar_por_x(unsigned short f)
{
    // f está en GF(2^16) con polinomio irreducible m(x) = x^16 + x^5 + x^3 + x + 1
    // m(x) en binario: 1 0000 0000 0010 1011 = 0x1002B, pero como solo
    // necesitamos 16 bits: 0000 0000 0010 1011 = 0x002B

    unsigned short resultado = f << 1; // Multiplicar por x

    // Si el bit 15 estaba encendido, necesitamos reducción modular
    if (f & 0x8000)
    {                        // 0x8000 = 1000 0000 0000 0000
        resultado ^= 0x002B; // XOR con m(x) sin el término x^16: 0000 0000 0010 1011
    }

    printf("f(x) = 0x%04X\n", f);
    printf("x * f(x) mod m(x) = 0x%04X\n", resultado);
}
    */
