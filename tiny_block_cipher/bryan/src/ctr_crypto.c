#include "ctr_crypto.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define DATA_DIR "data/"
#define PBOX_SIZE 8
#define SBOX_SIZE 256

/**
 * @brief Rellena una la S-Box con los números desde 0 a l-1.
 * 
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo para la S-Box.
 */
static void fill_array(int size, unsigned char array[size]) {
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
static void shuffle_array(int size, unsigned char array[size]) {
    for (int i = size - 1; i > 0; i--) {
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
static void generate_sbox_path(char name[], int max_size, int sbox_size) {
    snprintf(name, max_size, DATA_DIR "sbox_%dbits.txt", sbox_size);
}

/**
 * @brief Almacena la S-Box en un archivo de texto.
 * 
 * @param size Número de entradas de la S-Box.
 * @param array Arreglo con la S-Box.
 * @param textfile_name Nombre del archivo.
 * 
 * @return
 * - CTR_OK si se guardo correctamente.
 * - CTR_SBOX_GENERATION_ERROR si hubo un al generar y guardar la S-Box.
 */
static CTRStatus store_sbox(int size, unsigned char array[size], char textfile_name[]) {
    FILE *fp = fopen(textfile_name, "w");
    if (!fp) return CTR_SBOX_GENERATION_ERROR;

    for (int i = 0; i < size; i++) {
        fprintf(fp, "%02X -> %02X\n", i, array[i]);
    }

    fclose(fp);
    return CTR_OK;
}

/**
 * @brief Genera una S-Box de 8 bits.
 * 
 * @return
 * - CTR_OK si la S-Box se genero correctamente.
 * - CTR_SBOX_GENERATION_ERROR si hubo un al generar y guardar la S-Box.
 * - CTR_SBOX_MEMORY_ERROR si hubo un error al reservar memoria para la S-Box.
 */
CTRStatus sbox_generator() {
    int l = 1 << 3;
    int size_permutation = 1 << l;

    unsigned char *array = malloc(size_permutation * sizeof(*array));
    if (!array) return CTR_SBOX_MEMORY_ERROR;

    char textfile_name[50];

    fill_array(size_permutation, array);
    shuffle_array(size_permutation, array);
    generate_sbox_path(textfile_name, sizeof(textfile_name), l);
    
    CTRStatus ctr_status = store_sbox(size_permutation, array, textfile_name);
    
    free(array);
    return ctr_status;
}

/**
 * @brief Genera una llave secreta `K` de 16 bits.
 * 
 * @return
 * - CTR_OK si la llave se genero correctamente.
 * - CTR_KEY_GENERATION_ERROR si hubo un error al generar y guardar la llave.
 */
CTRStatus secret_key_generator() {
    unsigned short K = rand() % 65536;

    FILE *fp = fopen(DATA_DIR "key.txt", "w");
    if (!fp) return CTR_KEY_GENERATION_ERROR;

    fprintf(fp, "%04X\n", K);
    fclose(fp);
    return CTR_OK;
}

/**
 * @brief Carga la S-Box desde un archivo de texto.
 * 
 * @param sbox_filename Nombre del archivo.
 * @param sbox Arreglo en donde se guarda la S-Box.
 * 
 * @return
 * - CTR_OK si se cargo correctamente.
 * - CTR_SBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo de la S-Box.
 */
static CTRStatus load_sbox(char sbox_filename[], unsigned char sbox[]) {
    // Ruta completa para la S-Box
    char full_path[150];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", sbox_filename);
    printf("Abriendo archivo: %s\n", full_path);

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

/**
 * @brief Carga la llave desde un archivo de texto.
 * 
 * @param key_filename Nombre del archivo.
 * @param K Puntero donde se guarda la llave.
 * 
 * @return
 * - CTR_OK si se cargo correctamente.
 * - CTR_KEY_OPEN_FILE_ERROR si hubo un error al abrir al archivo de la llave.
 * - CTR_KEY_READ_ERROR si hubo un error al leer la llave.
 */
static CTRStatus load_key(char key_filename[], unsigned short *K) {
    // Ruta completa para la llave
    char full_path[150];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", key_filename);
    printf("Abriendo archivo: %s\n", full_path);

    FILE *fp = fopen(full_path, "r");
    if (!fp) return CTR_KEY_OPEN_FILE_ERROR;

    if (fscanf(fp, "%X", K) != 1) {
        fclose(fp);
        return CTR_KEY_READ_ERROR;
    }

    fclose(fp);

    return CTR_OK;
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
 * @brief Expande la llave secreta en sub-llaves.
 * 
 * @param K LLave original.
 * @param sbox S-Box de 8 bits.
 * @param sub_keys Arreglo donde se guardan las sub-llaves.
 */
static void key_expansion(unsigned short K, unsigned char sbox[], unsigned short sub_keys[]) {
    unsigned char w0 = get_8MSB(K);
    unsigned char w1 = get_8LSB(K);
    unsigned char w2 = w0 ^ 0x80 ^ sbox[swap_nibbles(w1)];
    unsigned char w3 = w2 ^ w1;
    unsigned char w4 = w2 ^ 0x30 ^ sbox[swap_nibbles(w3)];
    unsigned char w5 = w4 ^ w3;

    printf("\n>> Los valores son: \n\n");
    printf("w0: %02X\n", w0);
    printf("w1: %02X\n", w1);
    printf("w2: %02X\n", w2);
    printf("w3: %02X\n", w3);
    printf("w4: %02X\n", w4);
    printf("w5: %02X\n", w5);

    // Guardamos las sub-llaves
    sub_keys[0] = shuffle_bytes(w0, w1);
    sub_keys[1] = shuffle_bytes(w2, w3);
    sub_keys[2] = shuffle_bytes(w4, w5);
}

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
 * @param textfile_name Nombre del archivo.
 * 
 * @return
 * - CTR_OK si se guardo correctamente.
 * - CTR_PBOX_GENERATION_ERROR si hubo un error al generar y guardar la P-Box.
 */
static CTRStatus store_pbox(int P[PBOX_SIZE], char textfile_name[]) {
    FILE *fp = fopen(textfile_name, "w");
    if (!fp) return CTR_PBOX_GENERATION_ERROR;

    for (int i = 0; i < PBOX_SIZE; i++) {
        fprintf(fp, "%d\n", P[i]);
    }

    fclose(fp);
    return CTR_OK;
}

/**
 * @brief Genera una P-Box de tamaño 8.
 * 
 * @return
 * - CTR_OK si la P-Box se genero correctamente.
 * - CTR_PBOX_GENERATION_ERROR si hubo un error al generar y guardar la P-Box.
 */
CTRStatus pbox_generator() {
    int P[PBOX_SIZE] = {0};

    fill_pbox(P);
    shuffle_pbox(P);

    CTRStatus ctr_status = store_pbox(P, DATA_DIR "permutation.txt");

    return ctr_status;
}

/**
 * @brief Carga la P-Box desde un archivo de texto.
 * 
 * @param pbox_filename Nombre del archivo.
 * @param P Arreglo donde se guarda la P-Box.
 * 
 * @return
 * - CTR_OK si se cargo correctamente.
 * - CTR_PBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo de la P-Box.
 * - CTR_PBOX_OUT_OF_THE_RANGE_ERROR si el tamaño de la P-Box es incorrecto. 
 * - CTR_PBOX_REPEATED_VALUES_ERROR si hay valores repetidos en la P-Box.
 */
static CTRStatus load_pbox(char pbox_filename[], int P[PBOX_SIZE]) {
    // Ruta completa para la P-Box
    char full_path[150];
    snprintf(full_path, sizeof(full_path), DATA_DIR "%s", pbox_filename);
    printf("Abriendo archivo: %s\n", full_path);

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

/**
 * @brief Genera la permutación inversa de tamaño 8.
 * 
 * @param P Arreglo con la permutación original.
 * @param inv_P Arreglo donde se guarda la permutación inversa.
 */
static void inverse_pbox(int P[PBOX_SIZE], int inv_P[PBOX_SIZE]) {
    for (int i = 0; i < PBOX_SIZE; i++) {
        inv_P[P[i] - 1] = i + 1;
    }
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
 * @brief Cifra un bloque de 2 bytes.
 * 
 * @param sbox_filename Archivo con la S-Box.
 * @param key_filename Archivo con la llave secreta.
 * @param pbox_filename Archivo con la P-Box.
 * @param block Bloque a cifrar.
 * @param sub_keys Arreglo con las sub-llaves.
 * 
 * @return
 * - CTR_OK si el cifrado se hizo correctamente.
 * - CTR_KEY_OPEN_FILE_ERROR si hubo un error al cargar la llave.
 * - CTR_KEY_READ_ERROR si hubo un error al leer la llave.
 * - CTR_SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 * - CTR_PBOX_OPEN_FILE_ERROR si hubo un error al cargar la P-Box.
 * - CTR_PBOX_OUT_OF_THE_RANGE_ERROR si el tamaño de la P-Box es incorrecto.
 * - CTR_PBOX_REPEATED_VALUES_ERROR si hay valores repetidos en la P-Box.
 */
CTRStatus encrypt(char sbox_filename[], char key_filename[], char pbox_filename[], char block[], unsigned short sub_keys[]) {
    unsigned short K, M = format_block(block);
    unsigned char sbox[SBOX_SIZE], L, R;
    char string[3];
    int pbox[PBOX_SIZE];
    CTRStatus ctr_status;

    // Cargar la llave, la S-Box y la permutación
    ctr_status = load_key(key_filename, &K);
    if (CTR_OK != ctr_status) return ctr_status;

    ctr_status = load_sbox(sbox_filename, sbox);
    if (CTR_OK != ctr_status) return ctr_status;

    ctr_status = load_pbox(pbox_filename, pbox);
    if (CTR_OK != ctr_status) return ctr_status;

    // Expandir la llave
    key_expansion(K, sbox, sub_keys);

    // Cifrado
   for (int i = 0; i < 3; i ++) {
        printf("\nIteracion %d:\n", i + 1);
        
        // xor con la sub-llave
        printf("\nM <- %04X xor %04X", M, sub_keys[i]);
        M = M ^ sub_keys[i];
        printf(": %04X", M);

        // Aplicamos la S-Box
        L = get_8MSB(M); R = get_8LSB(M);
        L = sbox[L]; R = sbox[R];

        printf("\nM <- S(%04X)", M);
        M = shuffle_bytes(L, R);
        printf(": %04X (sustitucion)", M);

        // Aplicamos la P-Box
        L = get_8MSB(M); R = get_8LSB(M);
        L = permute_bits(L, pbox); R = permute_bits(R, pbox);

        printf("\nM <- P(%04X)", M);
        M = shuffle_bytes(L, R);
        printf(": %04X (permutacion)\n", M);
    }

    unformat_block(M, string);
    printf("\n>> El bloque cifrado es: %04X -> %c%c", M, string[0], string[1]);

    return CTR_OK;
}

/**
 * @brief Genera la S-Box inversa.
 * 
 * @param sbox Arreglo con la S-Box original.
 * @param inv_sbox Arreglo en donde se guarga la S-Box inversa.
 */
static void inverse_sbox(unsigned char sbox[SBOX_SIZE], unsigned char inv_sbox[SBOX_SIZE]) {
    for (int i = 0; i < SBOX_SIZE; i++) {
        inv_sbox[sbox[i]] = i;
    }
}

/**
 * @brief Descifra un bloque de 2 bytes.
 * 
 * @param sbox_filename Archivo con la S-Box.
 * @param key_filename Archivo con la llave secreta.
 * @param pbox_filename Archivo con la P-Box.
 * @param block Bloque a descifrar.
 * @param sub_keys Arreglo con las sub-llaves.
 * 
 * @return 
 * - CTR_OK si el descifrado se hizo correctamente. 
 * - CTR_KEY_OPEN_FILE_ERROR si hubo un error al cargar la llave.
 * - CTR_KEY_READ_ERROR si hubo un error al leer la llave.
 * - CTR_SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 * - CTR_PBOX_OPEN_FILE_ERROR si hubo un error al cargar la P-Box.
 * - CTR_PBOX_OUT_OF_THE_RANGE_ERROR si el tamaño de la P-Box es incorrecto.
 * - CTR_PBOX_REPEATED_VALUES_ERROR si hay valores repetidos en la P-Box.
 */
CTRStatus decrypt(char sbox_filename[], char key_filename[], char pbox_filename[], unsigned short C, unsigned short sub_keys[]) {
    //unsigned short K, C = format_block(block);
    unsigned short K;
    unsigned char sbox[SBOX_SIZE], inv_sbox[SBOX_SIZE], L, R;
    char string[3];
    int pbox[PBOX_SIZE], inv_pbox[PBOX_SIZE];
    CTRStatus ctr_status;   

    // Cargar la llave, la S-Box y la permutación
    ctr_status = load_key(key_filename, &K);
    if (CTR_OK != ctr_status) return ctr_status;

    ctr_status = load_sbox(sbox_filename, sbox);
    if (CTR_OK != ctr_status) return ctr_status;

    ctr_status = load_pbox(pbox_filename, pbox);
    if (CTR_OK != ctr_status) return ctr_status;

    // Generar la S-Box y P-Box inversa
    inverse_sbox(sbox, inv_sbox);
    inverse_pbox(pbox, inv_pbox);

    // Expandir la llave
    key_expansion(K, sbox, sub_keys);

    // Descifrado
    for (int i = 2; i >= 0; i--) {
        printf("\nIteracion %d:\n", 3 - i);
        
        // Aplicamos la P-Box inversa
        L = get_8MSB(C); R = get_8LSB(C);
        L = permute_bits(L, inv_pbox); R = permute_bits(R, inv_pbox);

        printf("\nC <- P^-1(%04X)", C);
        C = shuffle_bytes(L, R);
        printf(": %04X (permutacion inv)", C);

        // Aplicamos la S-Box inversa
        L = get_8MSB(C); R = get_8LSB(C);
        L = inv_sbox[L]; R = inv_sbox[R];

        printf("\nC <- S^-1(%04X)", C);
        C = shuffle_bytes(L, R);
        printf(": %04X (sustitucion inv)", C);

        // xor con la sub-llave
        printf("\nC <- %04X xor %04X", C, sub_keys[i]);
        C = C ^sub_keys[i];
        printf(": %04X\n", C);
    }

    unformat_block(C, string);
    printf("\n>> El bloque descifrado es: %04X -> %c%c", C, string[0], string[1]);

    return CTR_OK;
}