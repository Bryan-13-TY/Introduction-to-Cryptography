/**
 * Permutaciones de bits.
 * 
 * @author García Escamilla Bryan Alexis.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BIT_SIZE 32

typedef enum {
    BIT_OK = 0,
    BIT_POS_OUT_OF_RANGE = -1
} BitStatus;

static void wait_key();
static void print_binary(uint32_t num);
BitStatus get_nth_bit_of_number(uint32_t num, int position, int *nht_bit);
BitStatus set_nth_bit_of_number(uint32_t num, int position, uint32_t *new_num);
void leading_zeros(uint32_t num, int *zeros);

int main(int argc, char const *argv[]) {
    int nth_bit = 0, position = 0, option = 0, zeros = 0;
    uint32_t num = 0, new_num = 0;
    BitStatus bit_status;

    do {
        printf("\033[2J\033[H");
        printf("/*----------------.\n");
        printf("| BIT PERMUTATION |\n");
        printf("`----------------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Obtener el nth bit de un numero (32 bits)\n");
        printf("2.- Encender el nth bit de un numero (32 bits)\n");
        printf("3.- Contar los ceros a la izquierda de un numero (32 bits)\n");
        printf("4.- Salir del programa\n\n");
        printf("Opcion: ");
        scanf("%d", &option);

        switch (option) {
        case 1:
            printf("\n>> Escribe un numero (32 bits): ");
            scanf("%u", &num);
            printf(">>> %u -> ", num);
            print_binary(num);
            printf("\n\n>> Escribe la posicion del bit que desesar conocer: ");
            scanf("%d", &position);

            bit_status = get_nth_bit_of_number(num, position, &nth_bit);
            if (BIT_POS_OUT_OF_RANGE == bit_status) printf("\n>>> Posicion fuera de rango");
            if (BIT_OK == bit_status) printf(">>> El bit en la posicion %d es: %d", position, nth_bit);
            wait_key();
            break;
        case 2:
            printf("\n>> Escribe un numero (32 bits): ");
            scanf("%u", &num);
            printf(">>> %u -> ", num);
            print_binary(num);
            printf("\n\n>> Escribe la posicion del bit que deseas encender: ");
            scanf("%d", &position);

            bit_status = set_nth_bit_of_number(num, position, &new_num);
            if (BIT_POS_OUT_OF_RANGE == bit_status) printf("\n>>> Posicion fuera de rango");
            if (BIT_OK == bit_status) {
                printf("\n>>> El nuevo numero es: %u", new_num);
                printf("\n>>> %u -> ", new_num);
                print_binary(new_num);
            }
            wait_key();
            break;
        case 3:
            printf("\n>> Escribe un numero (32 bits): ");
            scanf("%u", &num);
            printf(">>> %u -> ", num);
            print_binary(num);

            leading_zeros(num, &zeros);
            printf("\n\n>>> Hay %d ceros a la izquierda del numero %u", zeros, num);
            wait_key();
            break;
        case 4:
            printf("\n>> Gracias por probar el programa");
            break;
        default:
            printf("\n>> Opcion no valida");
            wait_key();
            break;
        }
    } while (option != 4);
    return 0;
}

static void wait_key() {
    printf("\n\nPresiona Enter para continuar...");
    getchar();
    getchar();
}

static void print_binary(uint32_t num) {
    for (int i = 31; i >= 0; i--) {
        printf("%u", (num >> i) & 1);
    }
}

/**
 * @brief Obtiene el enésimo bit de un número de 32 bits.
 * 
 * @param num Número de 32 bits.
 * @param position Posición del bit a obtener.
 * @param nth_bit Puntero donde se almacena el bit.
 * 
 * @return
 * - BIT_OK si se obtuvo correctamente.
 * - BIT_POS_OUT_OF_RANGE si hubo un error en la posición del bit.
 */
BitStatus get_nth_bit_of_number(uint32_t num, int position, int *nth_bit) {
    if (position < 0 || position >= BIT_SIZE) return BIT_POS_OUT_OF_RANGE;

    *nth_bit = (num >> position) & 1;
    return BIT_OK;
}

/**
 * @brief Enciende el enésimo bit de un número de 32 bits.
 * 
 * @param num Número de 32 bits.
 * @param position Posición del bit a encender.
 * @param new_num Puntero donde se almacena el número resultante.
 * 
 * @return
 * - BIT_OK si se encendio correctamente.
 * - BIT_POS_OUT_OF_RANGE si hubo un error en la posición del bit.
 */
BitStatus set_nth_bit_of_number(uint32_t num, int position, uint32_t *new_num) {
    if (position < 0 || position >= BIT_SIZE) return BIT_POS_OUT_OF_RANGE;

    uint32_t mask = 1U << position;
    *new_num = num | mask;
    return BIT_OK;
}

/**
 * @brief Cuenta los números a la izquierda de un número de 32 bits.
 * 
 * @param num Número de 32 bits.
 * @param zeros Puntero donde se almacena el número de ceros.
 */
void leading_zeros(uint32_t num, int *zeros) {
    int count = 0;

    for (int i = 31; i >= 0; i--) {
        if ((num >> i) & 1) {
            *zeros = count;
            return;
        }
        count++;
    }

    *zeros = BIT_SIZE;
}
