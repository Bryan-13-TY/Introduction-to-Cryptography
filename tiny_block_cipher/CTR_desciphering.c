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
static BlockCStatus store_permutation(int size, unsigned char array[size], char textfile_name[]);
BlockCStatus load_permutation(char filename[], unsigned char P[8]);
static unsigned char apply_permutation(unsigned char P[8], unsigned char s);
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
static void unformat_block(unsigned short value, char block[]);
BlockCStatus ctr_decrypt_file(char output_filename[]);
static unsigned short tbc_encrypt(unsigned short block, unsigned short key);

int main(int argc, char const *argv[])
{
    int option = 0;
    char output_filename[100];
    BlockCStatus blockc_status;

    srand(time(NULL));

    do
    {
        printf("\033[2J\033[H");
        printf("/*------------------.\n");
        printf("| CTR_Decipher|\n");
        printf("`------------------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Descifrar un ciphertext\n");
        printf("2.- Salir del programa\n\n");
        printf("Opcion: ");
        scanf("%d", &option);
        clean_buffer();

        switch (option)
        {
        case 1:
            printf(">> Escribe el nombre del archivo de salida: ");
            read_string(sizeof(output_filename), output_filename);

            blockc_status = ctr_decrypt_file(output_filename);
            if (BLOCKC_OK != blockc_status)
                printf("\n>>> Error durante el descifrado CTR");
            wait_key();
            break;
        case 2:
            printf("\n>> Gracias por probar el programa");
            break;
        default:
            printf("\n>> Opcion no valida");
            wait_key();
            break;
        }
    } while (option != 2);
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
static unsigned short tbc_encrypt(unsigned short block, unsigned short key)
{
    unsigned short sub_keys[3];
    unsigned char L, R;
    unsigned char sbox[256], P[8];
    BlockCStatus blockc_status;

    blockc_status = load_sbox("EDU_sbox.txt", sbox);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    blockc_status = load_permutation("permutation.txt", P);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

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

static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static unsigned char base64_value(char c)
{
    char *ptr;
    ptr = strchr(base64_table, c);
    if (!ptr)
        return 0;
    return ptr - base64_table;
}

static unsigned short decode_base64_block(char b64_block[])
{
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;
    unsigned int triple;
    unsigned short block;

    b0 = base64_value(b64_block[0]);
    b1 = base64_value(b64_block[1]);
    b2 = base64_value(b64_block[2]);

    triple = (b0 << 18) | (b1 << 12) | (b2 << 6);
    block = ((triple >> 16) & 0xFF) << 8 | ((triple >> 8) & 0xFF);

    return block;
}
static BlockCStatus load_key(char key_filename[], unsigned short *K)
{
    FILE *fp = fopen(key_filename, "r");
    if (!fp)
        return BLOCKC_KEY_OPEN_FILE_ERROR;

    char b64_key[6];
    // Leemos la cadena Base64 del archivo key.txt
    if (fscanf(fp, "%4s", b64_key) != 1)
    {
        fclose(fp);
        return BLOCKC_KEY_READ_ERROR;
    }
    fclose(fp);

    *K = decode_base64_block(b64_key);

    return BLOCKC_OK;
}

BlockCStatus ctr_decrypt_file(char output_filename[])
{
    unsigned short K;
    unsigned char sbox[256], P[8];
    BlockCStatus blockc_status;
    char cipher_filename[100];

    blockc_status = load_key("key.txt", &K);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    blockc_status = load_sbox("EDU_sbox.txt", sbox);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    blockc_status = load_permutation("permutation.txt", P);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    printf(">> Digite el nombre del archivo cifrado a abrir: ");
    read_string(sizeof(cipher_filename), cipher_filename);

    FILE *input_fp = fopen(cipher_filename, "r");
    if (!input_fp)
        return BLOCKC_FILE_ERROR;

    char b64_c0[5];
    int N;

    if (fscanf(input_fp, " C0: %4s", b64_c0) != 1 || fscanf(input_fp, " N: %d", &N) != 1)
    {
        fclose(input_fp);
        return BLOCKC_FILE_ERROR;
    }

    unsigned short decoded_c0 = decode_base64_block(b64_c0);
    unsigned char C0 = (unsigned char)(decoded_c0 & 0xFF);
    unsigned char C1 = 0;
    unsigned short recovered_counter = (C0 << 8) | C1;

    FILE *output_fp = fopen(output_filename, "wb");
    if (!output_fp)
    {
        fclose(input_fp);
        return BLOCKC_FILE_ERROR;
    }

    unsigned char *mensaje_completo = malloc(65536);
    int mensaje_len = 0;
    char b64_block[5];

    for (int i = 0; i < N; i++)
    {
        if (fscanf(input_fp, " %4s", b64_block) != 1)
            break;

        // Descifrar bloque usando TU función
        unsigned short C_block = decode_base64_block(b64_block);

        unsigned short cont = (C0 << 8) | C1;
        unsigned short X = tbc_encrypt(cont, K);
        unsigned short M = C_block ^ X;

        fputc((M >> 8) & 0xFF, output_fp);
        if (i + 1 < N || (M & 0xFF) != 0)
            fputc(M & 0xFF, output_fp);

        C1++;

        char caracteres[3];
        unformat_block(M, caracteres);
        mensaje_completo[mensaje_len++] = caracteres[0];
        mensaje_completo[mensaje_len++] = caracteres[1];

        // printf("\n>> Bloque %d descifrado: %02X%02X", i + 1, (M >> 8) & 0xFF, M & 0xFF);
    }

    mensaje_completo[mensaje_len] = '\0';
    fclose(input_fp);
    fclose(output_fp);

    printf("\n>> El contador inicial recuperado fue: %04X", recovered_counter);
    printf("\n>> Texto descifrado:\n%s\n", mensaje_completo);
    printf("\n>> Archivo descifrado guardado en: %s", output_filename);

    free(mensaje_completo);
    return BLOCKC_OK;
}