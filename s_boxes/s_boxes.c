#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef enum {
    SBOX_OK = 0,
    SBOX_OUT_OF_RANGE = -1,
    SBOX_OPEN_FILE_ERROR = -2,
    SBOX_MEMORY_ERROR = -3,
} SBoxStatus;

static void wait_key();
static void clean_buffer();
static void fill_array(int size, int array[size]);
static void shuffle_array(int size, int array[size]);
static void generate_textfile_name(char name[], int max_size, int num_bits);
static SBoxStatus store_permutation(int size, int array[size], char textfile_name[]);
SBoxStatus sbox_generator(int n);

int main(int argc, char const *argv[]) {
    int option = 0, n = 0;
    SBoxStatus sbox_status;
    srand(time(NULL));

    do {
        printf("\033[2J\033[H");
        printf("/*------.\n");
        printf("| S-Box |\n");
        printf("`------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Generar una S-Box\n");
        printf("2.- Opcion 2\n");
        printf("3.- Opcion 3\n");
        printf("4.- Opcion 4\n");
        printf("5.- Salir de programa\n\n");
        printf("Opcion: ");
        scanf("%d", &option);
        clean_buffer();

        switch (option) {
            case 1:
                printf("\n>> Escribe el tamano de la S-Box: ");
                scanf("%d", &n);
                
                sbox_status = sbox_generator(n);
                if (SBOX_OK == sbox_status) printf("\n>>> La S-Box se guardo correctamente");
                if (SBOX_OUT_OF_RANGE == sbox_status) printf("\n>>> Deber estar entre 2 y 4");
                if (SBOX_MEMORY_ERROR == sbox_status) printf("\n>> Hubo un error al reservar memoria para la S-Box");
                if (SBOX_OPEN_FILE_ERROR == sbox_status) printf("\n>> Hubo un error al abrir el archivo de la S-Box");
                wait_key();
                break;
            case 2:
                wait_key();
                break;
            case 3:
                wait_key();
                break;
            case 4:
                wait_key();
                break;
            case 5:
                printf("\n>> Gracias por probar el programa");
                break;
            default:
                printf("\n>> Opcion no valida");
                wait_key();
                break;
        }
    } while (option != 5);
    return 0;
}

static void wait_key() {
    printf("\n\nPresiona Enter para continuar...");
    getchar();
    getchar();
}

static void clean_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Rellena una la S-Box con los números desde 0 a l-1.
 * 
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo para la S-Box.
 */
static void fill_array(int size, int array[size]) {
    for (int i = 0; i < size; i++) {
        array[i] = i;
    }
}

/**
 * @brief Permuta los valores de la S-Box.
 * 
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo para la S-Box.
 */
static void shuffle_array(int size, int array[size]) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    } 
}

/**
 * @brief Genera el nombre del archivo para almacenas la S-Box.
 * 
 * @param name Arreglo para el nombre del archivo.
 * @param max_size Tamaño del areglo `name`.
 * @param num_bits Número de bits a sustituir.
 */
static void generate_textfile_name(char name[], int max_size, int num_bits) {
    snprintf(name, max_size, "sbox_%dbits.txt", num_bits);
}

/**
 * @brief Almacena la S-Box en un archivo de texto.
 * 
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo con la S-Box.
 * @param textfile_name Nombre del archivo.
 * 
 * @return
 * - SBOX_OK si se guardo correctamente.
 * - SBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo.
 */
static SBoxStatus store_permutation(int size, int array[size], char textfile_name[]) {
    FILE *fp = fopen(textfile_name, "w");
    if (!fp) return SBOX_OPEN_FILE_ERROR;

    for (int i = 0; i < size; i++) {
        fprintf(fp, "%X -> %X\n", i, array[i]);
    }

    fclose(fp);
    return SBOX_OK;
}

SBoxStatus sbox_generator(int n) {
    if (!(n >= 2 && n <= 4)) return SBOX_OUT_OF_RANGE;

    int l = 1 << n;
    int size_permutation = 1 << l;

    int *array = malloc(size_permutation * sizeof(int));
    if (!array) return SBOX_MEMORY_ERROR;

    char textfile_name[50];

    fill_array(size_permutation, array);
    shuffle_array(size_permutation, array);
    generate_textfile_name(textfile_name, sizeof(textfile_name), l);
    
    SBoxStatus sbox_status = store_permutation(size_permutation, array, textfile_name);
    
    free(array);
    return sbox_status;
}