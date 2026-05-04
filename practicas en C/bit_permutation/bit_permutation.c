#include <stdio.h>
#include <stdlib.h>

static void print_binary(unsigned char c) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (c >> i) & 1);
    }
}

void exercise_1(unsigned char value) {
    printf("El caracter %c como caracter es: %c\n", value, value);
    printf("El caracter %c como hexadecimal es: %02X\n", value, value);
    printf("El caracter %c como entero es: %d", value, value);
}

void exercise_2(unsigned char value) {
    for (int i = 0; i < 8; i++) {
        unsigned char shifted = value << i;
        printf("Corrimiento %d de %c a la izquierda en caracter: %c, hexadecimal: %02x y entero: %d\n", i + 1, value, shifted, shifted, shifted);
    }
}

void exercise_3(unsigned char value) {
    // A
    for (int i = 0; i < 8; i++) {
        unsigned char shifted = value >> i;
        printf("Corrimiento %d de %c a la derecha en caracter: %c, hexadecimal: %02x y entero: %d\n", i + 1, value, shifted, shifted, shifted);
    }
}

void exercise_4(unsigned char v1, unsigned char v2) {
    printf("%c & %c en char: %c, hexadecimal: %02X, int: %d\n", v1, v2, v1 & v2, v1 & v2, v1 & v2);
    printf("%c & %c en char: %c, hexadecimal: %02X, int: %d\n", v2, v1, v2 & v1, v2 & v1, v2 & v1);
    printf("%c | %c en char: %c, hexadecimal: %02X, int: %d\n", v1, v2, v1 | v2, v1 | v2, v1 | v2);
    printf("%c | %c en char: %c, hexadecimal: %02X, int: %d\n", v2, v1, v2 | v1, v2 | v1, v2 | v1);
    printf("%c ^ %c en char: %c, hexadecimal: %02X, int: %d\n", v1, v2, v1 ^ v2, v1 ^ v2, v1 ^ v2);
    printf("%c ^ %c en char: %c, hexadecimal: %02X, int: %d", v2, v1, v2 ^ v1, v2 ^ v1, v2 ^ v1);
}

void exercise_5(unsigned char data2, unsigned char mask) {
    printf("El bit mas significativo de %c es: %u", data2, (data2 & mask) >> 7);    
}

void exercise_6(unsigned char data2, unsigned char mask) {
    unsigned char result = data2 & mask;

    printf("Los cuatro bits menos significativos de %c son: ", data2);

    for (int i = 3; i >= 0; i--) {
        printf("%d", (result >> i) & 1);
    }
}

int main(int argc, char const *argv[]) {
    //exercise_6('^', 0x0F);
    print_binary('X');
    return 0;
}
