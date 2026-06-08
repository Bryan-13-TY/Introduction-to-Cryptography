#include "ctr_crypto.h"
#include "utils_crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define DIR_BASE "data/"
#define PLAIN_FILENAME "plaintext.txt"
#define SBOX_FILENAME "EDU_sbox.txt"
#define PBOX_FILENAME "permutation.txt"
#define KEY_FILENAME "key.txt"
#define PBOX_SIZE 8
#define SBOX_SIZE 256

typedef struct {
    unsigned char sbox[SBOX_SIZE];
    int pbox[PBOX_SIZE];
    unsigned short K;
    unsigned short sub_keys[3];
} CTRContext;

static CTRContext ctr_data;

static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

/* ---------- FUNCIONES PARA OBTENER 4MSB, 4LSB, 8MSB, 8LSB ---------- */

/**
 * @brief Obtiene el nibble alto (4 bits más significativos) de un byte.
 * 
 * @param byte Un byte.
 * 
 * @return Nibble alto del byte.
 */
static unsigned char get_4MSB(unsigned char byte) {
    return (byte >> 4) & 0x0F;
}

/**
 * @brief Obtiene el nibble bajo (4 bits menos significativos) de un byte.
 * 
 * @param byte Un byte.
 * 
 * @return Nibble bajo del byte.
 */
static unsigned char get_4LSB(unsigned char byte) {
    return byte & 0x0F;
}

/**
 * @brief Obtiene el byte alto (8 bits más significativos) de dos bytes.
 * 
 * @param K Un número válido de 16 bits.
 * 
 * @return Byte alto de dos bytes.
 */
static unsigned char get_8MSB(unsigned short K) {
    return (K >> 8) & 0xFF;
}

/**
 * @brief Obtiene el byte bajo (8 bits menos significativos) de dos bytes.
 * 
 * @param K Un número válido de 16 bits.
 * 
 * @return Byte bajo de dos bytes.
 */
static unsigned char get_8LSB(unsigned short K) {
    return K & 0XFF;
}

/* ---------- FUNCIONES PARA MANEJAR BITS Y BYTES ---------- */

/**
 * @brief Combina el byte alto y bajo en dos bytes.
 * 
 * @param high_byte Byte alto de dos bytes.
 * @param low_byte Byte bajo de dos bytes.
 * 
 * @return Dos bytes.
 */
static unsigned short shuffle_bytes(unsigned char high_byte, unsigned char low_byte) {
    return ((unsigned short)high_byte << 8) | low_byte;
}

/**
 * @brief Intercambia el nibble alto y el bajo de un byte.
 * 
 * @param byte Un byte.
 * 
 * @return Byte con los nibble intercambiados.
 */
static unsigned char swap_nibbles(unsigned char byte) {
    return (get_4LSB(byte) << 4) | get_4MSB(byte);
}

/**
 * @brief Permuta los bits de `S` usando una permutación de tamaño 8.
 * 
 * @param S Un byte.
 * @param P Arreglo con la permutación.
 */
static unsigned char permute_bits(unsigned char S, int P[PBOX_SIZE]) {
    unsigned char new_s = 0;

    for (int i = 0; i < PBOX_SIZE; i++) {
        unsigned char bit = (S >> (8 - P[i])) & 1;
        new_s |= bit << (7 - i);
    }

    return new_s;
}

/**
 * @brief Convierte una cadena de 2 caracteres a un bloque de 2 bytes.
 * 
 * @param block Cadena.
 * 
 * @return Cadena como bloque de 2 bytes.
 */
static unsigned short format_block(unsigned char block[]) {
    return (block[0] << 8) | block[1];
}

/**
 * @brief Convierte un bloque de 2 bytes a una cadena de 2 caracteres.
 * 
 * @param value Bloque de 2 bytes.
 * @param block Arreglo donde se guardan los 2 caracteres.
 */
static void unformat_block(unsigned short value, char block[]) {
    block[0] = (unsigned char)(value >> 8) & 0xFF;
    block[1] = (unsigned char)value & 0xFF;
}

/* ---------- FUNCIONA PARA BASE64 ---------- */

/**
 * @brief Codifica un bloque de 2 bytes en Base64.
 * 
 * @param block Bloque de 2 bytes.
 * @param b64_block Bloque codificado en Base64.
 */
static void encode_base64_block(
    unsigned short block,
    char b64_block[]
) { 
    unsigned char byte1;
    unsigned char byte2;

    unsigned int triple;

    byte1 = (block >> 8) % 0xFF;
    byte2 = block & 0xFF;

    // Formar grupo de 24 bits
    triple = (byte1 << 16) | (byte2 << 8);

    // Convertir a Base64
    b64_block[0] = base64_table[(triple >> 18) & 0x3F];
    b64_block[1] = base64_table[(triple >> 12) & 0x3F];
    b64_block[2] = base64_table[(triple >> 6) & 0x3F];
    b64_block[3] = '=';
    b64_block[4] = '\0';
}

static unsigned char base64_value(char c) {
    char *ptr;

    ptr = strchr(base64_table, c);
    if (!ptr) return 0;

    return ptr - base64_table;
}

/**
 * @brief Decodifica un bloque en Base64 en 2 bytes.
 * 
 * @param b64_block Bloque codificado en Base64.
 * 
 * @return Bloque de 2 bytes.
 */
static unsigned short decode_base64_block(
    char b64_block[]
) {
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;

    unsigned int triple;

    unsigned short block;

    b0 = base64_value(b64_block[0]);
    b1 = base64_value(b64_block[1]);
    b2 = base64_value(b64_block[2]);

    // Reconstruir 24 bits
    triple = (b0 << 18) | (b1 << 12) | (b2 << 6);

    // Extraer solo los 2 bytes originales
    block = ((triple >> 16) & 0xFF) << 8 | ((triple >> 8) & 0xFF);

    return block;
}

/* ---------- FUNCIONES PARA ARCHIVOS Y CADENAS ---------- */

/**
 * @brief Genera la ruta completa de los archivos .txt usados por el programa.
 * 
 * @param full_path Arrglo en donde se guarda la ruta.
 * @param size Tamaño de `full_path`.
 * @param filename Nombre del archivo .txt.
 */
static void generate_full_path(char full_path[], size_t size, char filename[]) {
    snprintf(full_path, size, "%s%s", DIR_BASE, filename);
}

/**
 * @brief Obtiene al tamaño de un archivo.
 * 
 * Verifica que el tamaño de un archivo sea de al menos
 * de 1 KB.
 * 
 * @param filename Nombre del archivo.
 * @param size Apuntador en donde se guarda el tamaño del archivo en bytes.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus get_file_size(char filename[], long *size) {
    struct stat st;

    // Ruta completa para el plaintext
    char full_path[150]; 
    generate_full_path(full_path, sizeof(full_path), filename);

    if (stat(full_path, &st) != 0) return CTR_PLAINT_GET_SIZE_FILE_ERROR;

    if (st.st_size < 1024) return CTR_PLAINT_SIZE_FILE_ERROR;

    *size = st.st_size;
    return CTR_OK;
}

/**
 * @brief Carga el plaintext desde un archivo de texto.
 * 
 * @param plaintext_filename Nombre del archivo.
 * @param plaintext Puntero donde se guarda el plaintext.
 * @param size_p Tamaño en bytes del archivo.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus load_plaintext(
    char plaintext_filename[],
    char **plaintext,
    long size_p
) {
    *plaintext = (char *)malloc(size_p + 1);
    if (!(*plaintext)) return CTR_PLAINT_MEMORY_ERROR;

    // Ruta completa para el plaintext
    char full_path[150];
    generate_full_path(full_path, sizeof(full_path), plaintext_filename);
    printf(">>> Abriendo archivo: %s\n", full_path);
    
    FILE *fp = fopen(full_path, "rb");
    if (!fp) {
        free(*plaintext);
        return CTR_PLAINT_OPEN_FILE_ERROR;
    }

    // Leer todo el archivo
    fread(*plaintext, sizeof(char), size_p, fp);
    (*plaintext)[size_p] = '\0';
    fclose(fp);
    return CTR_OK;
}

/* ---------- FUNCIONES PARA LA LLAVE ---------- */

CTRStatus secret_key_generator() {
    unsigned short K = rand() % 65536;

    FILE *fp = fopen(DIR_BASE KEY_FILENAME, "w");
    if (!fp) return CTR_KEY_GENERATION_ERROR;

    char b64_block[5];

    encode_base64_block(K, b64_block);

    fprintf(fp, "%s\n", b64_block);
    fclose(fp);
    return CTR_OK;
}

/**
 * @brief Carga la llave desde un archivo de texto.
 * 
 * @param K Puntero donde se guarda la llave.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus load_key(unsigned short *K) {
    // Ruta completa para la llave
    char full_path[150];
    generate_full_path(full_path, sizeof(full_path), KEY_FILENAME);
    printf(">>> Abriendo archivo: %s\n", full_path);

    FILE *fp = fopen(full_path, "r");
    if (!fp) return CTR_KEY_OPEN_FILE_ERROR;

    char b64_block[6];

    if (fscanf(fp, "%4s", b64_block) != 1) {
        fclose(fp);
        return CTR_KEY_READ_ERROR;
    }

    *K = decode_base64_block(b64_block);

    fclose(fp);

    return CTR_OK;
}

/**
 * @brief Expande la llave secreta en sub-llaves.
 * 
 * @param K LLave original.
 */
static void key_expansion(unsigned short K) {
    unsigned char w0 = get_8MSB(K);
    unsigned char w1 = get_8LSB(K);
    unsigned char w2 = w0 ^ 0x80 ^ ctr_data.sbox[swap_nibbles(w1)];
    unsigned char w3 = w2 ^ w1;
    unsigned char w4 = w2 ^ 0x30 ^ ctr_data.sbox[swap_nibbles(w3)];
    unsigned char w5 = w4 ^ w3;

    // Guardamos las sub-llaves
    ctr_data.sub_keys[0] = shuffle_bytes(w0, w1);
    ctr_data.sub_keys[1] = shuffle_bytes(w2, w3);
    ctr_data.sub_keys[2] = shuffle_bytes(w4, w5);
}

/* ---------- FUNCIONES PARA LA S-BOX ---------- */

/**
 * @brief Rellena una la S-Box con los números desde 0 a l-1.
 * 
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo para la S-Box.
 */
static void fill_array(int size, unsigned char array[]) {
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
static void shuffle_array(int size, unsigned char array[]) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        unsigned char temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    } 
}

/**
 * @brief Almacena la S-Box en un archivo de texto.
 * 
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo con la S-Box.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus store_sbox(int size, unsigned char array[]) {
    FILE *fp = fopen(DIR_BASE SBOX_FILENAME, "w");
    if (!fp) return CTR_SBOX_GENERATION_ERROR;

    for (int i = 0; i < size; i++) {
        fprintf(fp, "%02X -> %02X\n", i, array[i]);
    }

    fclose(fp);
    return CTR_OK;
}

CTRStatus sbox_generator() {
    int l = 1 << 3;
    int size_permutation = 1 << l;

    unsigned char *array = malloc(size_permutation * sizeof(*array));
    if (!array) return CTR_SBOX_MEMORY_ERROR;

    fill_array(size_permutation, array);
    shuffle_array(size_permutation, array);
    
    CTRStatus ctr_status = store_sbox(size_permutation, array);
    
    free(array);
    return ctr_status;
}

/**
 * @brief Carga la S-Box desde un archivo de texto.
 * 
 * @param sbox Arreglo en donde se guarda la S-Box.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus load_sbox(unsigned char sbox[]) {
    // Ruta completa para la S-Box
    char full_path[150];
    generate_full_path(full_path, sizeof(full_path), SBOX_FILENAME);
    printf(">>> Abriendo archivo: %s\n", full_path);

    FILE *fp = fopen(full_path, "r");
    if (!fp) return CTR_SBOX_OPEN_FILE_ERROR;

    // Inicializa la S-Box
    int size = 1 << 8;
    for (int i = 0; i < size; i++) {
        sbox[i] = 0;
    }

    int input, output;
    while (fscanf(fp, " %X -> %X", &input, &output) == 2) {
        if (input >= 0 && input < SBOX_SIZE && output >= 0 && output < SBOX_SIZE) {
            sbox[input] = (unsigned char)output;
        }
    }

    fclose(fp);
    return CTR_OK;
}

/* ---------- FUNCIONES PARA LA P-BOX ---------- */

/**
 * @brief Inicializa una permutación de tamaño 8.
 * 
 * @param P Arreglo donde se inicializa la permutación.
 */
static void fill_pbox(int P[PBOX_SIZE]) {
    for (int i = 0; i < PBOX_SIZE; i++) {
        P[i] = i + 1;
    }
}

/**
 * @brief Combina los valores de la permutación inicializada.
 * 
 * @param P Arreglo donde se guarda la permutación.
 */
static void shuffle_pbox(int P[PBOX_SIZE]) {
    for (int i = PBOX_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        int temp = P[i];
        P[i] = P[j];
        P[j] = temp;
    }
}

/**
 * @brief Almacena la P-Box en un archivo de texto.
 * 
 * @param P Arreglo con la P-Box.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus store_pbox(int P[PBOX_SIZE]) {
    FILE *fp = fopen(DIR_BASE PBOX_FILENAME, "w");
    if (!fp) return CTR_PBOX_GENERATION_ERROR;

    for (int i = 0; i < PBOX_SIZE; i++) {
        fprintf(fp, "%d\n", P[i]);
    }

    fclose(fp);
    return CTR_OK;
}

CTRStatus pbox_generator() {
    int P[PBOX_SIZE] = {0};

    fill_pbox(P);
    shuffle_pbox(P);

    CTRStatus ctr_status = store_pbox(P);

    return ctr_status;
}

/**
 * @brief Carga la P-Box desde un archivo de texto.
 * 
 * @param P Arreglo donde se guarda la P-Box.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus load_pbox(int P[PBOX_SIZE]) {
    // Ruta completa para la P-Box
    char full_path[150];
    generate_full_path(full_path, sizeof(full_path), PBOX_FILENAME);
    printf(">>> Abriendo archivo: %s\n", full_path);

    FILE *fp = fopen(full_path, "r");
    if (!fp) return CTR_PBOX_OPEN_FILE_ERROR;

    int seen[PBOX_SIZE] = {0};
    int value, i = 0;
    while (i < PBOX_SIZE && fscanf(fp, "%d", &value) == 1) {
        // Rango válido
        if (value < 1 || value > PBOX_SIZE) {
            fclose(fp);
            return CTR_PBOX_OUT_OF_THE_RANGE_ERROR;
        }

        // Repetidos
        if (seen[value - 1]) {
            fclose(fp);
            return CTR_PBOX_REPEATED_VALUES_ERROR;
        }

        seen[value - 1] = 1;
        P[i++] = value;
    }

    fclose(fp);

    // Exactamente 8 entradas
    if (i != PBOX_SIZE) {
        return CTR_PBOX_OUT_OF_THE_RANGE_ERROR;
    }

    return CTR_OK;
}

/* ---------- FUNCIONES PARA CIFRAR Y DESCIFRAR ---------- */

/**
 * @brief Carga el contexto para el modo CTR.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus load_ctr_context() {
    CTRStatus ctr_status;
    
    ctr_status = load_sbox(ctr_data.sbox);
    if (CTR_OK != ctr_status) return ctr_status;

    ctr_status = load_pbox(ctr_data.pbox);
    if (CTR_OK != ctr_status) return ctr_status;

    ctr_status = load_key(&(ctr_data.K));
    if (CTR_OK != ctr_status) return ctr_status;

    return CTR_OK;
}

/**
 * @brief Cifra un bloque de dos bytes.
 * 
 * @param M Bloque de dos bytes.
 * @param K Llave secreta.
 * 
 * @return Bloque cifrado.
 */
static unsigned short TBC_encrypt(unsigned short M, unsigned short K) {
    unsigned short L, R;

    // Expandir la llave
    key_expansion(K);

    for (int i = 0; i < 3; i++) {
        M = M ^ ctr_data.sub_keys[i];

        L = get_8MSB(M);
        R = get_8LSB(M);
        L = ctr_data.sbox[L];
        R = ctr_data.sbox[R];

        M = shuffle_bytes(L, R);

        L = get_8MSB(M);
        R = get_8LSB(M);
        L = permute_bits(L, ctr_data.pbox);
        R = permute_bits(R, ctr_data.pbox);

        M = shuffle_bytes(L, R);
    }

    return M;
}

/* ---------- FUNCIONES PARA CIFRAR ---------- */

/**
 * @brief Divide un texto en bloques de 2 bytes.
 * 
 * @param plaintext Arreglo con el texto.
 * @param size_plaintext Tamaño del `plaintext`.
 * @param blocks Arreglo en donde se guardan los bloques del `plaintext`.
 */
static void divide_blocks(
    char plaintext[],
    int size_plaintext,
    unsigned short blocks[]
) {
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
        i += 2;
        j++;
    }
}

/**
 * @brief Almacena el ciphertext en un archivo de texto.
 * 
 * Se incluye el valor de `C0` y el número de bloques del
 * `ciphertext`.
 * 
 * @param C0 Byte alto del contador del modo CTR.
 * @param ciphertext Arreglo con el texto cifrado.
 * @param num_blocks Número de bloques de `ciphertext`.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus store_ciphertext(
    unsigned char C0,
    unsigned short ciphertext[],
    int num_blocks
) {
    char ciphertext_filename[60];

    printf("\n>>> Escribe el nombre del archivo para guardar el ciphertext: ");
    read_string(sizeof(ciphertext_filename), ciphertext_filename);

    // Ruta completa para el ciphertext
    char full_path[150];
    generate_full_path(full_path, sizeof(full_path), ciphertext_filename);

    FILE *fp = fopen(full_path, "w");
    if (!fp) return CTR_CIPHER_GENERATION_ERROR;

    char b64_block[5];

    encode_base64_block(C0, b64_block);
    fprintf(fp, "C0: %s\n", b64_block);

    fprintf(fp, "N: %d\n", num_blocks);

    for (int i = 0; i < num_blocks; i++) {
        encode_base64_block(ciphertext[i], b64_block);
        fprintf(fp, "%s\n", b64_block);
    }

    fclose(fp);
    return CTR_OK;
}

CTRStatus encrypt_ctr(char plaintext_filename[]) {
    CTRStatus ctr_status;

    // Cargar el plaintext
    char *plaintext = NULL;
    long plain_size = 0;
    
    ctr_status = get_file_size(plaintext_filename, &plain_size);
    if (CTR_OK != ctr_status) return ctr_status;
    ctr_status = load_plaintext(plaintext_filename, &plaintext, plain_size);
    if (CTR_OK != ctr_status) return ctr_status;

    int plaintext_size = strlen(plaintext);
    
    // Cargar la llave, S-Box y P-Box
    ctr_status = load_ctr_context();
    if (CTR_OK != ctr_status) return ctr_status;

    // Dividir en bloques
    int num_blocks = (plaintext_size + 1) / 2; // Considerando padding
    unsigned short blocks[num_blocks];
    unsigned short ciphertext[num_blocks];

    divide_blocks(plaintext, plaintext_size, blocks);

    // CTR
    unsigned char C0 = rand() % 256;
    unsigned char C1 = 0;

    printf("\nC0: %02X\n", C0);

    for (int i = 0; i < num_blocks; i++) {
        unsigned short counter = ((unsigned short)C0 << 8) | C1;
        unsigned short keystream = TBC_encrypt(counter, ctr_data.K);

        ciphertext[i] = blocks[i] ^ keystream;

        printf("\nBloque %d:\n", i + 1);
        printf("\nCounter: %04X", counter);
        printf("\nKeystream: %04X", keystream);
        printf("\nCipher: %04X\n", ciphertext[i]);

        C1++;
    }

    ctr_status = store_ciphertext(C0, ciphertext, num_blocks);
    if (CTR_OK != ctr_status) return ctr_status;

    return CTR_OK;
}

/* ---------- FUNCIONES PARA DESCIFRAR ---------- */

/**
 * @brief Carga el texto cifrado desde un archivo de texto.
 * 
 * @param C0 Variable donde se guarda el byte alto del contador del modo CTR.
 * @param ciphertext Arreglo en donde se guarda el texto cifrado.
 * @param num_blocks Variable en donde se guarda el número de bloques del `ciphertext`.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
static CTRStatus load_ciphertext(
    unsigned char *C0,
    unsigned short ciphertext[],
    int *num_blocks,
    char ciphertext_filename[]
) {
    // Ruta completa para la P-Box
    char full_path[150];
    generate_full_path(full_path, sizeof(full_path), ciphertext_filename);
    printf(">>> Abriendo archivo: %s\n", full_path);

    FILE *fp = fopen(full_path, "r");
    if (!fp) return CTR_CIPHER_OPEN_FILE_ERROR;

    char b64_block[6];

    if (fscanf(fp, " C0: %4s", b64_block) != 1) {
        fclose(fp);
        return CTR_COUNTER_READ_ERROR;
    }

    *C0 = decode_base64_block(b64_block);

    if (fscanf(fp, " N: %d", num_blocks) != 1) {
        fclose(fp);
        return CTR_NUM_BLOCKS_READ_ERROR;
    }

    for (int i = 0; i < *num_blocks; i++) {
        // Leer bloque Base64
        if (fscanf(fp, " %4s", b64_block) != 1) {
            fclose(fp);
            return CTR_CIPHER_READ_ERROR;
        }

        ciphertext[i] = decode_base64_block(b64_block);
    }

    fclose(fp);
    return CTR_OK;
}

/**
 * @brief Reconstruye el texto cifrado y lo imprime.
 * 
 * @param plaintext Arreglo con los bloques descrifrados.
 * @param num_blocks Número de blques del texto cifrado.
 */
static void rebuild_text(unsigned short plaintext[], int num_blocks) {
    for (int i = 0; i < num_blocks; i++) {
        char block[2];
        unformat_block(plaintext[i], block);

        printf("%c%c", block[0], block[1]);
    }
}

CTRStatus decrypt_ctr(char ciphertext_filename[]) {
    unsigned char C0;
    int num_blocks;
    unsigned short ciphertext[5000];
    unsigned short plaintext[5000];

    CTRStatus ctr_status;

    // Cargar la llave, S-Box y P-Box
    ctr_status = load_ctr_context();
    if (CTR_OK != ctr_status) return ctr_status;

    // Cargar el ciphertext
    ctr_status = load_ciphertext(&C0, ciphertext, &num_blocks, ciphertext_filename);
    if (CTR_OK != ctr_status) return ctr_status;

    // CTR
    unsigned char C1 = 0;

    printf("\nC0: %02X\n", C0);

    for (int i = 0; i < num_blocks; i++) {
        unsigned short counter = ((unsigned short)C0 << 8) | C1;
        unsigned short keystream = TBC_encrypt(counter, ctr_data.K);

        plaintext[i] = ciphertext[i] ^ keystream;

        printf("\nBloque %d:\n", i + 1);
        printf("\nCounter: %04X", counter);
        printf("\nKeystream: %04X", keystream);
        printf("\nPlain: %04X\n", plaintext[i]);

        C1++;
    }

    // Reconstruir texto
    printf("\n>> El texto original recuperado es:\n\n");
    rebuild_text(plaintext, num_blocks);

    return CTR_OK;
}
