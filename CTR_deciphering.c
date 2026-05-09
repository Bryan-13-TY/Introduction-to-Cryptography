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
BlockCStatus sbox_generator();
BlockCStatus permutation_generator();
BlockCStatus load_permutation(char filename[], unsigned char P[8]);
// BlockCStatus load_inverse_permutation(char filename[], unsigned char P_inv[8]);
static unsigned char apply_permutation(unsigned char P[8], unsigned char s);
BlockCStatus secret_key_generator();
static void read_string(int size, char string[size]);
static BlockCStatus load_sbox(char sbox_filename[], unsigned char sbox[]);
static BlockCStatus load_key(char key_filename[], unsigned short *K);
static unsigned short format_block(char block[]);
static unsigned char get_8MSB(unsigned short K);
static unsigned char get_8LSB(unsigned short K);
static unsigned short shuffle_bytes(unsigned char high_byte, unsigned char low_byte);
static unsigned char get_4MSB(unsigned char c);
static unsigned char get_4LSB(unsigned char c);
static unsigned char swap_nibbles(unsigned char byte);
static void key_expansion(unsigned short K, unsigned char sbox[], unsigned short sub_keys[]);
static BlockCStatus load_inverse_sbox(char sbox_filename[], unsigned char inverse_sbox[]);
static void unformat_block(unsigned short value, char block[]);
// BlockCStatus decrypt(char sbox_filename[], char key_filename[], unsigned short block, unsigned short sub_keys[]);
BlockCStatus ctr_encrypt_file(char sbox_filename[], char key_filename[], char input_filename[], char output_filename[]);
BlockCStatus ctr_decrypt_file(char sbox_filename[], char key_filename[], char input_filename[], char output_filename[]);
static unsigned short tbc_encrypt(unsigned short block, unsigned short key, unsigned char sbox[], unsigned char P[]);

int main(int argc, char const *argv[])
{
    int option = 0;
    char key_filename[100], sbox_filename[100], input_filename[100], output_filename[100];
    unsigned short sub_keys[3], C;
    BlockCStatus blockc_status;

    srand(time(NULL));

    do
    {
        printf("\033[2J\033[H");
        printf("/*------------------.\n");
        printf("| CTR Mode |\n");
        printf("`------------------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Generar una S-Box de 8 bits\n");
        printf("2.- Generar una clave secreta\n");
        printf("3.- Generar una permutacion de 8 bits\n");
        printf("4.- Cifrar un plaintext\n");
        printf("5.- Descifrar un ciphertext\n");
        printf("6.- Salir del programa\n\n");
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
            printf("\n>> Escribe el nombre del archivo con la llave: ");
            read_string(sizeof(key_filename), key_filename);
            printf(">> Escribe el nombre del archivo con la S-Box: ");
            read_string(sizeof(sbox_filename), sbox_filename);
            printf(">> Escribe el nombre del archivo de entrada: ");
            read_string(sizeof(input_filename), input_filename);
            printf(">> Escribe el nombre del archivo de salida: ");
            read_string(sizeof(output_filename), output_filename);

            blockc_status = ctr_encrypt_file(sbox_filename, key_filename,
                                             input_filename, output_filename);
            if (BLOCKC_OK != blockc_status)
                printf("\n>>> Error durante el cifrado CTR");
            wait_key();
            break;
        case 5:
            printf("\n>> Escribe el nombre del archivo con la llave: ");
            read_string(sizeof(key_filename), key_filename);
            printf(">> Escribe el nombre del archivo con la S-Box: ");
            read_string(sizeof(sbox_filename), sbox_filename);
            printf(">> Escribe el nombre del archivo cifrado: ");
            read_string(sizeof(input_filename), input_filename);
            printf(">> Escribe el nombre del archivo de salida: ");
            read_string(sizeof(output_filename), output_filename);

            blockc_status = ctr_decrypt_file(sbox_filename, key_filename,
                                             input_filename, output_filename);
            if (BLOCKC_OK != blockc_status)
                printf("\n>>> Error durante el descifrado CTR");
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
    } while (option != 6);
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
    snprintf(name, max_size, "sbox_%dbits.txt", sbox_size);
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

    FILE *fp = fopen("key.txt", "w");
    if (!fp)
        return BLOCKC_KEY_OPEN_FILE_ERROR;

    fprintf(fp, "%04X\n", K);
    fclose(fp);
    return BLOCKC_OK;
}

/**
 * @brief Lee una cadena de caracteres desde el teclado.
 *
 * @param size Tamaño del arreglo de la cadena.
 * @param string Cadena leida.
 */
static void read_string(int size, char string[size])
{
    if (fgets(string, size, stdin))
    {
        string[strcspn(string, "\n")] = '\0';
    }
    else
    {
        string[0] = '\0';
    }
}

/**
 * @brief Carga la S-Box desde un archivo de texto.
 *
 * @param sbox_filename Nombre del archivo.
 * @param sbox Arreglo en donde se guarda la S-Box.
 *
 * @return
 * - BLOCKC_OK si se cargo correctamente.
 * - BLOCKC_SBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo.
 */
static BlockCStatus load_sbox(char sbox_filename[], unsigned char sbox[])
{
    FILE *fp = fopen(sbox_filename, "r");
    if (!fp)
        return BLOCKC_SBOX_OPEN_FILE_ERROR;

    // Inicializa la S-Box
    int size = 1 << 8;
    for (int i = 0; i < size; i++)
    {
        sbox[i] = 0;
    }

    int input, output;
    while (fscanf(fp, " %X -> %X", &input, &output) == 2)
    {
        if (input >= 0 && input < 256 && output >= 0 && output < 256)
        {
            sbox[input] = (unsigned char)output;
        }
    }

    fclose(fp);
    return BLOCKC_OK;
}

/**
 * @brief Carga la llave desde un archivo de texto.
 *
 * @param key_filename Nombre del archivo.
 * @param K Puntero donde se guarda la llave.
 *
 * @return
 * - BLOCKC_OK si se cargo correctamente.
 * - BLOCKC_KEY_OPEN_FILE_ERROR si hubo un error al abrir al archivo.
 * - BLOCKC_KEY_READ_ERROR si hubo un error al leer la llave.
 */
static BlockCStatus load_key(char key_filename[], unsigned short *K)
{
    FILE *fp = fopen(key_filename, "r");
    if (!fp)
        return BLOCKC_KEY_OPEN_FILE_ERROR;

    if (fscanf(fp, "%X", K) != 1)
    {
        fclose(fp);
        return BLOCKC_KEY_READ_ERROR;
    }

    fclose(fp);

    return BLOCKC_OK;
}

/**
 * @brief Convierte una cadena de 2 caracteres a un bloque de 2 bytes.
 *
 * @param block Cadena.
 *
 * @return Cadena como bloque de 2 bytes.
 */
static unsigned short format_block(char block[])
{
    return ((unsigned char)block[0] << 8) | (unsigned char)block[1];
}

/**
 * @brief Convierte un bloque de 2 bytes a una cadena de 2 caracteres.
 *
 * @param value Bloque de dos 2 bytes.
 * @param block Arreglo donde se guardan los 2 caracteres.
 */
static void unformat_block(unsigned short value, char block[])
{
    block[0] = (unsigned char)((value >> 8) & 0xFF);
    block[1] = (unsigned char)(value & 0xFF);
    block[2] = '\0';
}

/**
 * @brief Obtiene el byte alto (8 bits más significativos) de dos bytes.
 *
 * @param K Un número válido de 16 bits.
 *
 * @return Byte alto de dos bytes.
 */
static unsigned char get_8MSB(unsigned short K)
{
    return (K >> 8) & 0xFF;
}

/**
 * @brief Obtiene el byte bajo (8 bits menos significativos) de dos bytes.
 *
 * @param K Un número válido de 16 bits.
 *
 * @return Byte bajo de dos bytes.
 */
static unsigned char get_8LSB(unsigned short K)
{
    return K & 0XFF;
}

/**
 * @brief Combina el byte alto y bajo en dos bytes.
 *
 * @param high_byte Byte alto de dos bytes.
 * @param low_byte Byte bajo de dos bytes.
 *
 * @return Dos bytes.
 */
static unsigned short shuffle_bytes(unsigned char high_byte, unsigned char low_byte)
{
    return ((unsigned short)high_byte << 8) | low_byte;
}

/**
 * @brief Obtiene el nibble alto (4 bits más significativos) de un byte.
 *
 * @param byte Un byte.
 *
 * @return Nibble alto del byte.
 */
static unsigned char get_4MSB(unsigned char byte)
{
    return (byte >> 4) & 0x0F;
}

/**
 * @brief Obtiene el nibble bajo (4 bits menos significativos) de un byte.
 *
 * @param byte Un byte.
 *
 * @return Nibble bajo del byte.
 */
static unsigned char get_4LSB(unsigned char byte)
{
    return byte & 0x0F;
}

/**
 * @brief Intercambia el nibble alto y el bajo de un byte.
 *
 * @param byte Un byte.
 *
 * @return Byte con los nibble intercambiados.
 */
static unsigned char swap_nibbles(unsigned char byte)
{
    return (get_4LSB(byte) << 4) | get_4MSB(byte);
}

/**
 * @brief Expande la llave secreta en sub-llaves.
 *
 * @param K LLave original.
 * @param sbox S-Box de 8 bits.
 * @param sub_keys Arreglo donde se guardan las sub-llaves.
 */
static void key_expansion(unsigned short K, unsigned char sbox[], unsigned short sub_keys[])
{
    unsigned char w0 = get_8MSB(K);
    unsigned char w1 = get_8LSB(K);
    unsigned char w2 = w0 ^ 0x80 ^ sbox[swap_nibbles(w1)];
    unsigned char w3 = w2 ^ w1;
    unsigned char w4 = w2 ^ 0x30 ^ sbox[swap_nibbles(w3)];
    unsigned char w5 = w4 ^ w3;

    // Guardamos las sub-llaves
    sub_keys[0] = shuffle_bytes(w0, w1);
    sub_keys[1] = shuffle_bytes(w2, w3);
    sub_keys[2] = shuffle_bytes(w4, w5);
}

/**
 * @brief
 *
 * @param sbox_filename Archivo con la S-Box.
 * @param key_filename Archivo con la llave secreta.
 * @param block Bloque a cifrar.
 * @param sub_keys Arreglo con las sub-llaves.
 *
 * @return
 * - BLOCKC_OK si el cifrado se hizo correctamente.
 * - BLOCKC_SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 * - BLOCKC_KEY_OPEN_FILE_ERROR si hubo un error al cargar la llave.
 * - BLOCKC_KEY_READ_ERROR si hubo un error al leer la llave.
 */
static unsigned short tbc_encrypt(unsigned short block, unsigned short key, unsigned char sbox[], unsigned char P[])
{
    unsigned short sub_keys[3];
    unsigned char L, R;

    key_expansion(key, sbox, sub_keys);

    unsigned short M = block;

    for (int i = 0; i < 3; i++)
    {
        M = M ^ sub_keys[i];
        L = (M >> 8) & 0xFF;
        R = M & 0xFF;
        L = sbox[L];
        R = sbox[R];
        L = apply_permutation(P, L);
        R = apply_permutation(P, R);
        M = (L << 8) | R;
    }

    return M;
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

BlockCStatus load_permutation(char filename[], unsigned char P[8])
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
        return BLOCKC_SBOX_OPEN_FILE_ERROR;

    int input, output = 0;
    while (output < 8 && fscanf(fp, "%d", &input) == 1)
    {
        P[output++] = (unsigned char)(input - 1);
    }

    fclose(fp);
    return BLOCKC_OK;
}

static unsigned char apply_permutation(unsigned char P[8], unsigned char s)
{
    unsigned char result = 0;
    for (int i = 0; i < 8; i++)
    {
        unsigned char bit = (s >> (7 - P[i])) & 1;
        result |= (bit << (7 - i));
    }
    return result;
}

BlockCStatus ctr_encrypt_file(char sbox_filename[], char key_filename[], char input_filename[], char output_filename[])
{
    unsigned short K;
    unsigned char sbox[256], P[8];
    BlockCStatus blockc_status;

    blockc_status = load_key(key_filename, &K);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    blockc_status = load_sbox(sbox_filename, sbox);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    blockc_status = load_permutation("permutation.txt", P);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    FILE *input_fp = fopen(input_filename, "rb");
    if (!input_fp)
        return BLOCKC_FILE_ERROR;

    // Obtenemos el tamaño del archivo
    fseek(input_fp, 0, SEEK_END);
    long file_size = ftell(input_fp);
    fseek(input_fp, 0, SEEK_SET);

    unsigned char *plaintext = malloc(file_size);
    if (!plaintext)
    {
        fclose(input_fp);
        return BLOCKC_MEMORY_ERROR;
    }

    size_t bytes_read = fread(plaintext, 1, file_size, input_fp);
    fclose(input_fp);

    FILE *output_fp = fopen(output_filename, "wb");
    if (!output_fp)
    {
        free(plaintext);
        return BLOCKC_FILE_ERROR;
    }

    unsigned char C0 = rand() % 256;
    unsigned char C1 = 0;

    fprintf(output_fp, "%02X", C0);

    unsigned char *ciphertext = malloc(bytes_read + 1);
    if (!ciphertext)
    {
        free(plaintext);
        fclose(output_fp);
        return BLOCKC_MEMORY_ERROR;
    }

    int ciphertext_len = 0;
    printf("\n>> C0 usado: %02X", C0);

    for (long i = 0; i < bytes_read; i += 2)
    {
        unsigned short cont = (C0 << 8) | C1;

        unsigned short X = tbc_encrypt(cont, K, sbox, P);

        unsigned short M_block = 0;
        M_block = plaintext[i] << 8;
        if (i + 1 < bytes_read)
            M_block |= plaintext[i + 1];
        // Si el archivo tiene un número impar de bytes, el último bloque se completa con un byte de valor 0

        unsigned short Y = M_block ^ X;

        fprintf(output_fp, "%04X", Y);
        ciphertext_len += 2;

        C1 = (C1 + 1) % 256;
        if (C1 == 0)
            C0 = (C0 + 1) % 256;

        printf("\n>> Bloque %d: Counter = %04X, TBC = %04X, Cipher Block = %04X", (i / 2) + 1, cont, X, Y);
    }

    fclose(output_fp);
    free(plaintext);
    free(ciphertext);

    printf("\n>> Archivo cifrado guardado en: %s", output_filename);

    return BLOCKC_OK;
}

BlockCStatus ctr_decrypt_file(char sbox_filename[], char key_filename[], char input_filename[], char output_filename[])
{
    unsigned short K;
    unsigned char sbox[256], P[8];
    BlockCStatus blockc_status;

    blockc_status = load_key(key_filename, &K);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    blockc_status = load_sbox(sbox_filename, sbox);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    blockc_status = load_permutation("permutation.txt", P);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    FILE *input_fp = fopen(input_filename, "r");
    if (!input_fp)
        return BLOCKC_FILE_ERROR;

    unsigned int temp;
    if (fscanf(input_fp, "%02X", &temp) != 1)
    {
        fclose(input_fp);
        return BLOCKC_FILE_ERROR;
    }
    unsigned char C0 = (unsigned char)temp;
    unsigned char C1 = 0;
    unsigned char *mensaje_completo = malloc(65536);
    int mensaje_len = 0;
    unsigned char *ciphertext = malloc(65536);
    if (!ciphertext)
    {
        fclose(input_fp);
        return BLOCKC_MEMORY_ERROR;
    }

    int ciphertext_len = 0;
    unsigned short block;
    while (fscanf(input_fp, "%04hX", &block) == 1)
    {
        ciphertext[ciphertext_len++] = (block >> 8) & 0xFF;
        ciphertext[ciphertext_len++] = block & 0xFF;
    }
    fclose(input_fp);

    FILE *output_fp = fopen(output_filename, "wb");
    if (!output_fp)
    {
        free(ciphertext);
        return BLOCKC_FILE_ERROR;
    }

    printf("\n>> C0 usado: %02X", C0);
    for (int i = 0; i < ciphertext_len; i += 2)
    {
        unsigned short cont = (C0 << 8) | C1;

        unsigned short X = tbc_encrypt(cont, K, sbox, P);

        unsigned short C_block = (ciphertext[i] << 8) | ciphertext[i + 1];

        unsigned short M = C_block ^ X;

        fputc((M >> 8) & 0xFF, output_fp);
        fputc(M & 0xFF, output_fp);

        C1 = (C1 + 1) % 256;
        if (C1 == 0)
            C0 = (C0 + 1) % 256;

        char caracteres[3];
        unformat_block(M, caracteres);
        mensaje_completo[mensaje_len++] = caracteres[0];
        mensaje_completo[mensaje_len++] = caracteres[1];

        printf("\n>> Bloque %d: Counter = %04X, TBC = %04X, Text block = %04X", (i / 2) + 1, cont, X, M);
    }
    mensaje_completo[mensaje_len] = '\0';
    fclose(output_fp);
    free(ciphertext);

    printf("\n");
    printf("\n>> Texto descifrado: %s\n", mensaje_completo);
    printf("\n>> Archivo descifrado guardado en: %s", output_filename);

    return BLOCKC_OK;
}