#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef enum
{
    BLOCKC_OK = 0,
    BLOCKC_SBOX_OPEN_FILE_ERROR = -1,
    BLOCKC_SBOX_MEMORY_ERROR = -2,
    BLOCKC_KEY_OPEN_FILE_ERROR = -3,
    BLOCKC_KEY_READ_ERROR = -4,
    BLOCKC_FILE_ERROR = -5,
    BLOCKC_MEMORY_ERROR = -6,
} BlockCStatus;

static void wait_key();
static void clean_buffer();
static void fill_array(int size, unsigned char array[size]);
static void shuffle_array(int size, unsigned char array[size]);
static void generate_textfile_name(char name[], int max_size, int sbox_size);
static BlockCStatus store_permutation(int size, unsigned char array[size], char textfile_name[]);
static void bytes_to_base64(unsigned char byte1, unsigned char byte2, char *output);
BlockCStatus sbox_generator();
BlockCStatus permutation_generator();
BlockCStatus secret_key_generator();

int main(int argc, char const *argv[])
{
    int option = 0;
    BlockCStatus blockc_status;

    srand(time(NULL));

    do
    {
        printf("\033[2J\033[H");
        printf("/*------------------.\n");
        printf("| File Generation |\n");
        printf("`------------------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Generar una S-Box de 8 bits\n");
        printf("2.- Generar una clave secreta\n");
        printf("3.- Generar una permutacion de 8 bits\n");
        printf("4.- Salir del programa\n\n");
        printf("Opcion: ");
        scanf("%d", &option);
        clean_buffer();

        switch (option)
        {
        case 1:
            blockc_status = sbox_generator();
            if (BLOCKC_OK == blockc_status)
                printf("\n>>> La S-Box se guardo correctamente");
            if (BLOCKC_SBOX_MEMORY_ERROR == blockc_status)
                printf("\n>>> Hubo un error al reservar memoria para la S-Box");
            if (BLOCKC_SBOX_OPEN_FILE_ERROR == blockc_status)
                printf("\n>>> Hubo un error al abrir el archivo de la S-Box");
            wait_key();
            break;
        case 2:
            blockc_status = secret_key_generator();
            if (BLOCKC_OK == blockc_status)
                printf("\n>>> La llave secreta se guardo correctamente");
            if (BLOCKC_KEY_OPEN_FILE_ERROR == blockc_status)
                printf("\n>>> Hubo un error al guardar la llave");
            wait_key();
            break;
        case 3:
            blockc_status = permutation_generator();
            if (BLOCKC_OK == blockc_status)
                printf("\n>>> La permutación se guardo correctamente");
            if (BLOCKC_SBOX_OPEN_FILE_ERROR == blockc_status)
                printf("\n>>> Hubo un error al abrir el archivo de la permutación");
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

static void wait_key()
{
    printf("\n\nPresiona Enter para continuar...");
    getchar();
    getchar();
}

static void clean_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/**
 * @brief Rellena una la S-Box con los números desde 0 a l-1.
 *
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo para la S-Box.
 */
static void fill_array(int size, unsigned char array[size])
{
    for (int i = 0; i < size; i++)
    {
        array[i] = i;
    }
}

/**
 * @brief Permuta los valores de la S-Box.
 *
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo para la S-Box.
 */
static void shuffle_array(int size, unsigned char array[size])
{
    for (int i = size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        unsigned char temp = array[i];
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
static void generate_textfile_name(char name[], int max_size, int sbox_size)
{
    snprintf(name, max_size, "EDU_sbox.txt", sbox_size);
}

/**
 * @brief Almacena la S-Box en un archivo de texto.
 *
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo con la S-Box.
 * @param textfile_name Nombre del archivo.
 *
 * @return
 * - BLOCKC_SBOX_OK si se guardo correctamente.
 * - BLOCKC_SBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo.
 */
static BlockCStatus store_permutation(int size, unsigned char array[size], char textfile_name[])
{
    FILE *fp = fopen(textfile_name, "w");
    if (!fp)
        return BLOCKC_SBOX_OPEN_FILE_ERROR;

    for (int i = 0; i < size; i++)
    {
        fprintf(fp, "%02X -> %02X\n", i, array[i]);
    }

    fclose(fp);
    return BLOCKC_OK;
}

/**
 * @brief Genera una S-Box de 4 u 8 bits.
 *
 * @param n Tal que el tamaño de la S-Box es `l` = 2^`n` bits.
 *
 * @return
 * - BLOCKC_SBOX_OK si la S-Box se genero correctamente.
 * - BLOCKC_SBOX_OPEN_FILE_ERROR si hubo un error al guardar la S-Box.
 * - BLOCKC_SBOX_MEMORY_ERROR si hubo un error al reservar memoria para la S-Box.
 */
BlockCStatus sbox_generator()
{
    int l = 1 << 3;
    int size_permutation = 1 << l;

    unsigned char *array = malloc(size_permutation * sizeof(*array));
    if (!array)
        return BLOCKC_SBOX_MEMORY_ERROR;

    char textfile_name[50];

    fill_array(size_permutation, array);
    shuffle_array(size_permutation, array);
    generate_textfile_name(textfile_name, sizeof(textfile_name), l);

    BlockCStatus blockc_status = store_permutation(size_permutation, array, textfile_name);

    free(array);
    return blockc_status;
}

/**
 * @brief Genera una llave secreta `K` de 16 bits.
 *
 * @return
 * - BLOCKC_OK si la llave se genero correctamente.
 * - BLOCKC_KEY_OPEN_FILE_ERROR si hubo un error al guardar la llave.
 */
BlockCStatus secret_key_generator()
{
    unsigned short K = rand() % 65536;

    char b64_key[5];

    unsigned char byte_alto = (K >> 8) & 0xFF;
    unsigned char byte_bajo = K & 0xFF;

    // Convertimos los bytes a Base64
    bytes_to_base64(byte_alto, byte_bajo, b64_key);

    FILE *fp = fopen("key.txt", "w");
    if (!fp)
        return BLOCKC_KEY_OPEN_FILE_ERROR;

    fprintf(fp, "%s\n", b64_key);
    fclose(fp);
    return BLOCKC_OK;
}

BlockCStatus permutation_generator()
{
    unsigned char P[8];

    for (int i = 0; i < 8; i++)
        P[i] = (unsigned char)(i + 1);

    for (int i = 7; i > 0; i--)
    {
        int j = rand() % (i + 1);
        unsigned char temp = P[i];
        P[i] = P[j];
        P[j] = temp;
    }

    FILE *fp = fopen("permutation.txt", "w");
    if (!fp)
        return BLOCKC_SBOX_OPEN_FILE_ERROR;

    for (int i = 0; i < 8; i++)
    {
        fprintf(fp, "%d\n", P[i]);
    }

    fclose(fp);
    return BLOCKC_OK;
}

static void bytes_to_base64(unsigned char byte1, unsigned char byte2, char *output)
{
    const char b64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    // Dividimos los 16 bits de los 2 bytes en grupos de 6 bits para Base64
    // Un unsigned short completo genera 3 grupos de 6 bits (el último se rellena con ceros)
    unsigned int value = (byte1 << 16) | (byte2 << 8);

    output[0] = b64_chars[(value >> 18) & 0x3F];
    output[1] = b64_chars[(value >> 12) & 0x3F];
    output[2] = b64_chars[(value >> 6) & 0x3F];
    output[3] = '='; // Acolchado (Padding) porque solo procesamos 2 bytes de los 3 que espera Base64 originalmente
    output[4] = '\0';
}
