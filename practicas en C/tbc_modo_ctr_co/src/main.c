#include "ctr_crypto.h"
#include "utils_crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char const *argv[]) {
    int option = 0;
    char plaintext_filename[60];
    char ciphertext_filename[60];
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
                show_possible_error(ctr_status);
                wait_key();
                break;
            case 2:
                ctr_status = secret_key_generator();
                if (CTR_OK == ctr_status) printf("\n>>> La llave secreta se guardo correctamente");
                show_possible_error(ctr_status);
                wait_key();
                break;
            case 3:
                ctr_status = pbox_generator();
                if (CTR_OK == ctr_status) printf("\n>>> La P-Box se guardo correctamente");
                show_possible_error(ctr_status);
                wait_key();
                break;
            case 4:
                printf("\n>> Escribe el nombre del archivo con el plaintext: ");
                read_string(sizeof(plaintext_filename), plaintext_filename);
                ctr_status = encrypt_ctr(plaintext_filename);
                show_possible_error(ctr_status);
                wait_key();
                break;
            case 5:
                printf("\n>> Escribe el nombre del archivo con el ciphertext: ");
                read_string(sizeof(ciphertext_filename), ciphertext_filename);
                ctr_status = decrypt_ctr(ciphertext_filename); // nombre del ciphertext
                show_possible_error(ctr_status);
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
