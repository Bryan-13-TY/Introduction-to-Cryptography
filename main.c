#include <stdio.h>

unsigned char xtime(unsigned char x) {
    if (x & 0x80)
        return ((x << 1) ^ 0x1B) & 0xFF;

    return (x << 1) & 0xFF;
}

unsigned char mult(unsigned char a, unsigned char b) {

    switch(a) {

        case 0x01:
            return b;

        case 0x02:
            return xtime(b);

        case 0x03:
            return xtime(b) ^ b;
    }

    return 0x00;
}

int main() {

    // Estado después de ShiftRows
    unsigned char state[4][4] = {
        {0x56, 0x5A, 0x00, 0xA8},
        {0x1B, 0xCB, 0x72, 0x70},
        {0x53, 0x7B, 0x65, 0x04},
        {0x77, 0x08, 0x2C, 0x20}
    };

    unsigned char result[4][4];

    // MixColumns
    for (int c = 0; c < 4; c++) {

        result[0][c] =
            mult(0x02, state[0][c]) ^
            mult(0x03, state[1][c]) ^
            mult(0x01, state[2][c]) ^
            mult(0x01, state[3][c]);

        result[1][c] =
            mult(0x01, state[0][c]) ^
            mult(0x02, state[1][c]) ^
            mult(0x03, state[2][c]) ^
            mult(0x01, state[3][c]);

        result[2][c] =
            mult(0x01, state[0][c]) ^
            mult(0x01, state[1][c]) ^
            mult(0x02, state[2][c]) ^
            mult(0x03, state[3][c]);

        result[3][c] =
            mult(0x03, state[0][c]) ^
            mult(0x01, state[1][c]) ^
            mult(0x01, state[2][c]) ^
            mult(0x02, state[3][c]);
    }

    // Imprimir resultado
    printf("MixColumns:\n\n");

    for (int i = 0; i < 4; i++) {

        for (int j = 0; j < 4; j++) {
            printf("%02X ", result[i][j]);
        }

        printf("\n");
    }

    return 0;
}