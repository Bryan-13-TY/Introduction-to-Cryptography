#include "ctr_crypto.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char const *argv[]) {
    int option = 0;
    char key_filename[100];
    char sbox_filename[100];
    char pbox_filename[100];
    char plaintext[100000];
    char ciphertext_filename[100];
    CTRStatus ctr_status;

    srand(time(NULL));

    do {
        printf("\033[2J\033[H");
        printf("/*-----------------.\n");
        printf("| Cifrado modo CTR |\n");
        printf("`-----------------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Generar una S-Box de 8 bits\n");
        printf("2.- Generar una clave secreta\n");
        printf("3.- Generar una permutacion\n");
        printf("4.- Cifrar un plaintext\n");
        printf("5.- Descifrar un ciphertext\n");
        printf("6.- Salir del programa\n\n");
        printf("Opcion: ");
        scanf("%d", &option);
        clean_buffer();

        switch (option) {
            case 1:
                ctr_status = sbox_generator();
                if (CTR_OK == ctr_status) printf("\n>>> La S-Box se guardo correctamente");
                if (CTR_SBOX_MEMORY_ERROR == ctr_status) printf("\n>>> Hubo un error al reservar memoria para la S-Box");
                if (CTR_SBOX_GENERATION_ERROR == ctr_status) printf("\n>>> Hubo un error al generar y guardar la S-Box");
                wait_key();
                break;
            case 2:
                ctr_status = secret_key_generator();
                if (CTR_OK == ctr_status) printf("\n>>> La llave secreta se guardo correctamente");
                if (CTR_KEY_GENERATION_ERROR == ctr_status) printf("\n>>> Hubo un error al generar y guardar la llave");
                wait_key();
                break;
            case 3:
                ctr_status = pbox_generator();
                if (CTR_OK == ctr_status) printf("\n>>> La P-Box se guardo correctamente");
                if (CTR_PBOX_GENERATION_ERROR == ctr_status) printf("\n>>> Hubo un error al generar y guardar la P-Box");
                wait_key();
                break;
            case 4:
                printf("\n>> Escribe el nombre del archivo con la llave: ");
                read_string(sizeof(key_filename), key_filename);
                printf(">> Escribe el nombre del archivo con la S-Box: ");
                read_string(sizeof(sbox_filename), sbox_filename);
                printf(">> Escribe el nombre del archivo con la P-Box: ");
                read_string(sizeof(pbox_filename), pbox_filename);
                printf(">> Escribe el texto a cifrar: ");
                read_string(sizeof(plaintext), plaintext);
                ctr_status = encrypt_ctr(sbox_filename, key_filename, pbox_filename, plaintext, strlen(plaintext));
                if (CTR_KEY_OPEN_FILE_ERROR == ctr_status) printf("\n>>> Hubo un error al cargar la llave");
                if (CTR_KEY_READ_ERROR == ctr_status) printf("\n>>> Hubo un erro al leer la llave");
                if (CTR_SBOX_OPEN_FILE_ERROR == ctr_status) printf("\n>>> Hubo un error al cargar la S-Box");
                if (CTR_PBOX_OPEN_FILE_ERROR == ctr_status) printf("\n>>> Hubo un error al cargar la P-Box");
                if (CTR_PBOX_OUT_OF_THE_RANGE_ERROR == ctr_status) printf("\n>>> El tamano de la P-Box es incorrecto");
                if (CTR_PBOX_REPEATED_VALUES_ERROR == ctr_status) printf("\n>>> Hay valores repetidos en la P-Box");
                if (CTR_CIPHER_GENERATION_ERROR == ctr_status) printf("\n>>> Hubo un error al generar y guardar el texto cifrado");
                wait_key();
                break;
            case 5:
                printf("\n>> Escribe el nombre del archivo con la llave: ");
                read_string(sizeof(key_filename), key_filename);
                printf(">> Escribe el nombre del archivo con la S-Box: ");
                read_string(sizeof(sbox_filename), sbox_filename);
                printf(">> Escribe el nombre del archivo con la P-Box: ");
                read_string(sizeof(pbox_filename), pbox_filename);
                printf(">> Escribe el nombre del archivo con el ciphertext: ");
                read_string(sizeof(ciphertext_filename), ciphertext_filename);
                ctr_status = decrypt_ctr(sbox_filename, key_filename, pbox_filename, ciphertext_filename);
                if (CTR_KEY_OPEN_FILE_ERROR == ctr_status) printf("\n>>> Hubo un error al cargar la llave");
                if (CTR_KEY_READ_ERROR == ctr_status) printf("\n>>> Hubo un erro al leer la llave");
                if (CTR_SBOX_OPEN_FILE_ERROR == ctr_status) printf("\n>>> Hubo un error al cargar la S-Box");
                if (CTR_PBOX_OPEN_FILE_ERROR == ctr_status) printf("\n>>> Hubo un error al cargar la P-Box");
                if (CTR_PBOX_OUT_OF_THE_RANGE_ERROR == ctr_status) printf("\n>>> El tamano de la P-Box es incorrecto");
                if (CTR_PBOX_REPEATED_VALUES_ERROR == ctr_status) printf("\n>>> Hay valores repetidos en la P-Box");
                if (CTR_CIPHER_OPEN_FILE_ERROR == ctr_status) printf("\n>>> Hubo un error al cargar el texto cifrado");
                if (CTR_COUNTER_READ_ERROR == ctr_status) printf("\n>>> Hubo un error al leer el byte alto del contador del modo CTR");
                if (CTR_NUM_BLOCKS_READ_ERROR == ctr_status) printf("\n>>> Hubo un error al leer el numero de bloques del ciphertext");
                if (CTR_CIPHERTEXT_READ_ERROR == ctr_status) printf("\n>>> Hubo un error el leer los bloques del ciphertext");
                wait_key();
                break;
            case 6:
                printf("\n>> Gracias por probar el programa");
                break;
            default:
                printf("\n>> Opcion no valida");
                wait_key();
                break;
        }
    } while(option != 6);
    return 0;
}
