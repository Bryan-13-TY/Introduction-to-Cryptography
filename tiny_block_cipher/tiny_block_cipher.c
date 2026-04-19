/**
 * Pequeño cifrador de bloque.
 * 
 * @author García Escamilla Bryan Alexis.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef enum {
    BLOCKC_OK = 0,
    BLOCKC_SBOX_OUT_OF_RANGE = -1,
    BLOCKC_SBOX_OPEN_FILE_ERROR = -2,
    BLOCKC_SBOX_MEMORY_ERROR = -3,
} BlockCStatus;

typedef struct {
    unsigned short K0;
    unsigned short K1;
    unsigned short K2;
} SubKeys;

static void wait_key();
static void clean_buffer();
static void fill_array(int size, unsigned char array[size]);
static void shuffle_array(int size, unsigned char array[size]);
static void generate_textfile_name(char name[], int max_size, int sbox_size);
static BlockCStatus store_permutation(int size, unsigned char array[size], char textfile_name[]);
BlockCStatus sbox_generator(int n);
BlockCStatus secret_key_generator();


static unsigned char get_8MSB(unsigned short K);
static unsigned char get_8LSB(unsigned short K);
static unsigned short shuffle_bytes(unsigned char high_byte, unsigned char low_byte);
static unsigned char get_4MSB(unsigned char c);
static unsigned char get_4LSB(unsigned char c);
static unsigned char shuffle_nibbles(unsigned char high_nibble, unsigned char low_nibble);
static int hex_to_int(char c);
static BlockCStatus load_sbox(char textfile_name[], unsigned char sbox[], int bits);
BlockCStatus key_expansion(unsigned short K, SubKeys *sub_keys);
static void read_string(int size, char string[size]);

int main(int argc, char const *argv[]) {
    int option = 0, n = 0, temp = 0;
    unsigned short K, sbox_filename;
    BlockCStatus blockc_status;
    SubKeys sub_keys;
    srand(time(NULL));

    do {
        printf("\033[2J\033[H");
        printf("/*------------------.\n");
        printf("| Tiny Block Cipher |\n");
        printf("`------------------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Generar una S-Box\n");
        printf("2.- Generar una clave secreta\n");
        printf("3.- Generar subclaves\n");
        printf("4.- Cifrar un plaintext\n");
        printf("5.- Salir de programa\n\n");
        printf("Opcion: ");
        scanf("%d", &option);
        clean_buffer();

        switch (option) {
            case 1:
                printf("\n>> Escribe el valor de n tal que l = 2^n: ");
                scanf("%d", &n);
                
                blockc_status = sbox_generator(n);
                if (BLOCKC_OK == blockc_status) printf("\n>>> La S-Box se guardo correctamente");
                if (BLOCKC_SBOX_OUT_OF_RANGE == blockc_status) printf("\n>>> Debe ser 2 o 3");
                if (BLOCKC_SBOX_MEMORY_ERROR == blockc_status) printf("\n>>> Hubo un error al reservar memoria para la S-Box");
                if (BLOCKC_SBOX_OPEN_FILE_ERROR == blockc_status) printf("\n>>> Hubo un error al abrir el archivo de la S-Box");
                wait_key();
                break;
            case 2:
                blockc_status = secret_key_generator();
                if (BLOCKC_OK == blockc_status) printf("\n>>> La llave secreta se guardo correctamente");
                if (BLOCKC_SBOX_OPEN_FILE_ERROR == blockc_status) printf("\n>>> Hubo un error al guardar la llave");
                wait_key();
                break;
            case 3:
                printf("\n>> Escribe un numero (0 a 65535): ");
                scanf("%d", &temp);
                clean_buffer();

                if (temp < 0 || temp > 65535) {
                    printf("\n>>> El numero no es valido");
                } else {
                    K = (unsigned short)temp;
                    blockc_status = key_expansion(K, &sub_keys);
                    if (BLOCKC_OK != blockc_status) printf("\n>>> Hubo un error al cargar la S-Box de 8 bits");
                } 

                wait_key();
                break;
            case 4:
                printf("\n>> Escribe el plaintext: ");
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
static void generate_textfile_name(char name[], int max_size, int sbox_size) {
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
 * - SBOX_OK si se guardo correctamente.
 * - SBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo.
 */
static BlockCStatus store_permutation(int size, unsigned char array[size], char textfile_name[]) {
    FILE *fp = fopen(textfile_name, "w");
    if (!fp) return BLOCKC_SBOX_OPEN_FILE_ERROR;

    for (int i = 0; i < size; i++) {
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
 * - SBOX_OK si la S-Box se genero correctamente.
 * - SBOX_OPEN_FILE_ERROR si hubo un error al guardar la S-Box.
 * - SBOX_MEMORY_ERROR si hubo un error al reservar memoria para la S-Box.
 */
BlockCStatus sbox_generator(int n) {
    if (n < 2 || n > 3) return BLOCKC_SBOX_OUT_OF_RANGE;

    int l = 1 << n;
    int size_permutation = 1 << l;

    unsigned char *array = malloc(size_permutation * sizeof(*array));
    if (!array) return BLOCKC_SBOX_MEMORY_ERROR;

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
 * - BLOCKC_SBOX_OPEN_FILE_ERROR si hubo un error al guardar la llave.
 */
BlockCStatus secret_key_generator() {
    unsigned short K = rand() % 65535;

    FILE *fp = fopen("key.txt", "w");
    if (!fp) return BLOCKC_SBOX_OPEN_FILE_ERROR;

    fprintf(fp, "%02X", K);
    fclose(fp);
    return BLOCKC_OK;
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
 * @param c Un byte.
 * 
 * @return Nibble alto del byte.
 */
static unsigned char get_4MSB(unsigned char c) {
    return (c >> 4) & 0x0F;
}

/**
 * @brief Obtiene el nibble bajo (4 bits menos significativos) de un byte.
 * 
 * @param c Un byte.
 * 
 * @return Nibble bajo del byte.
 */
static unsigned char get_4LSB(unsigned char c) {
    return c & 0x0F;
}

/**
 * @brief Combina el nibble alto y bajo en un byte.
 * 
 * @param high_nibble Nibble alto de un byte.
 * @param low_nibble Nibble bajo de un byte.
 * 
 * @return Un Byte.
 */
static unsigned char shuffle_nibbles(unsigned char high_nibble, unsigned char low_nibble) {
    return (high_nibble << 4) | low_nibble;
}

static unsigned char swap_nibbles(unsigned char byte) {
    return (get_4LSB(byte) << 4) | get_4MSB(byte);
}

static int hex_to_int(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else {
        return -1;
    }
}

/**
 * @brief Carga la S-Box desde un archivo de texto.
 * 
 * @param textfile_name Nombre del archivo.
 * @param sbox Arreglo en donde se carga la S-Box.
 * @param bits Tamaño de la S-Box.
 * 
 * @return
 * - SBOX_OK si se cargo correctamente.
 * - SBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo.
 */
static BlockCStatus load_sbox(char textfile_name[], unsigned char sbox[], int bits) {
    FILE *fp = fopen(textfile_name, "r");
    if (!fp) return BLOCKC_SBOX_OPEN_FILE_ERROR;

    int input, output;

    // Inicializar s-box
    int size = 1 << bits;
    for (int i = 0; i < size; i++) {
        sbox[i] = 0;
    }

    if (bits == 4) {    
        char in, out;

        while (fscanf(fp, " %c -> %c", &in, &out) == 2) {
            input = hex_to_int(in);
            output = hex_to_int(out);

            if (input != -1 && output != -1) {
                if (input >= 0 && input < 15 && output >= 0 && output < 16) {
                    sbox[input] = (unsigned char)output;
                }
            }
        }
    } else if (bits == 8) {
        // Evitamos escribir fuera del arreglo
        while (fscanf(fp, " %X -> %X", &input, &output) == 2) {
            if (input >= 0 && input < 256 && output >= 0 && output < 256) {
                sbox[input] = (unsigned char)output;
            }
        }
    } else {
        fclose(fp);
        return BLOCKC_SBOX_OUT_OF_RANGE;
    }

    fclose(fp);
    return BLOCKC_OK;
}

BlockCStatus key_expansion(unsigned short K, SubKeys *sub_keys) {
    unsigned char sbox_8[256];
    
    BlockCStatus blockc_status = load_sbox("sbox_8bits.txt", sbox_8, 8);
    if (BLOCKC_OK != blockc_status) return blockc_status;
    
    unsigned char w0 = get_8MSB(K);
    unsigned char w1 = get_8LSB(K);
    unsigned char w2 = w0 ^ 0x80 ^ sbox_8[swap_nibbles(w1)];
    unsigned char w3 = w2 ^ w1;
    unsigned char w4 = w2 ^ 0x30 ^ sbox_8[swap_nibbles(w3)];
    unsigned char w5 = w4 ^ w3;

    printf("\n>> Los valores son: \n\n");
    printf("w0: %02X\n", w0);
    printf("w1: %02X\n", w1);
    printf("w2: %02X\n", w2);
    printf("w3: %02X\n", w3);
    printf("w4: %02X\n", w4);
    printf("w5: %02X", w5);

    sub_keys->K0 = shuffle_bytes(w0, w1);
    sub_keys->K1 = shuffle_bytes(w2, w3);
    sub_keys->K2 = shuffle_bytes(w4, w5);

    return BLOCKC_OK;
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

