#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static void read_string(int size, char string[size]);
static void divide_blocks(char plaintext[], int size_plaintext, unsigned short blocks[]);
static void unformat_block(unsigned short value, char block[]);
static unsigned short format_block(char block[]);

int main(int argc, char const *argv[]) {
    srand(time(NULL));
    
    char plaintext[100000];
    char ciphertext[100000];
    unsigned char C0 = rand() % 256;
    unsigned char C1 = 0;
    unsigned short plaintext_blocks[50000];

    printf(">> Escribe un texto: ");
    read_string(sizeof(plaintext), plaintext);

    printf("\nEl texto fue: %s", plaintext);
    printf("\nY la longitud del texto es: %d", strlen(plaintext));

    printf("\n\nBloques del plaintext:\n");
    divide_blocks(plaintext, strlen(plaintext), plaintext_blocks);

    /*
    for (int i = 0; i < strlen(plaintext) / 2; i++) {
        printf("Block[%d]: %04X\n", i, plaintext_blocks[i]);
    }
    */


    //printf("Valor de C0: %02X", C0);
    return 0;
}

/**
 * @brief Lee una cadena de caracteres desde el teclado.
 * 
 * @param size Tamaño del arreglo de la cadena.
 * @param string Cadena leida.
 */
static void read_string(int size, char string[size]) {
    if (fgets(string, size, stdin)) {
        string[strcspn(string, "\n")] = '\0';
    } else {
        string[0] = '\0';
    }
}

/**
 * @brief Convierte una cadena de 2 caracteres a un bloque de 2 bytes.
 * 
 * @param block Cadena.
 * 
 * @return Cadena como bloque de 2 bytes.
 */
static unsigned short format_block(char block[]) {
    return ((unsigned char)block[0] << 8) | (unsigned char)block[1];
}

/**
 * @brief Convierte un bloque de 2 bytes a una cadena de 2 caracteres.
 * 
 * @param value Bloque de dos 2 bytes.
 * @param block Arreglo donde se guardan los 2 caracteres.
 */
static void unformat_block(unsigned short value, char block[]) {
    block[0] = (unsigned char)(value >> 8) & 0xFF;
    block[1] = (unsigned char)value & 0xFF;
}

static void divide_blocks(char plaintext[], int size_plaintext, unsigned short blocks[]) {
    unsigned char block_temp[2];
    
    int i = 0, j = 0;
    while (i < size_plaintext) {
        block_temp[0] = plaintext[i];

        if (i + 1 < size_plaintext) {
            block_temp[1] = plaintext[i + 1];
        } else {
            block_temp[1] = 0x00; // padding
        }

        blocks[j] = format_block(block_temp);
        i+=2;
        j++;
    }
}