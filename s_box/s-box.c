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
static void read_string(int size, char string[size]);
static unsigned char get_4MSB(char c);
static unsigned char get_4LSB(char c);
static unsigned char shuffle_bits(unsigned char high_nibble, unsigned char low_nibble);
static SBoxStatus load_sbox(char textfile_name[], int sbox[], int bits);
static int hex_to_int(char c);
SBoxStatus sbox_4bits(char M[]);
SBoxStatus sbox_8bits(char M[]);
static unsigned char get_8MSB(unsigned short K);
static unsigned char get_8LSB(unsigned short K);
static unsigned short shuffle_bytes(unsigned char high_byte, unsigned char low_byte);
SBoxStatus sbox_16bits(unsigned short K);
SBoxStatus sbox_4bits_k(unsigned short k);

int main(int argc, char const *argv[]) {
    int option = 0, option_bit  = 0, n = 0, temp = 0;
    char M[200];
    unsigned short K;
    SBoxStatus sbox_status;
    srand(time(NULL));

    do {
        printf("\033[2J\033[H");
        printf("/*------.\n");
        printf("| S-Box |\n");
        printf("`------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Generar una S-Box\n");
        printf("2.- Sustitucion con una S-Box de 4 o 8 bits\n");
        printf("3.- Sustitucion con dos S-Box de 8 bits en 16 bits\n");
        printf("4.- Sustitucion en bloques de 4 bits sobre 16 bits\n");
        printf("5.- Salir de programa\n\n");
        printf("Opcion: ");
        scanf("%d", &option);
        clean_buffer();

        switch (option) {
            case 1:
                printf("\n>> Escribe el valor de n tal que l = 2^n: ");
                scanf("%d", &n);
                
                sbox_status = sbox_generator(n);
                if (SBOX_OK == sbox_status) printf("\n>>> La S-Box se guardo correctamente");
                if (SBOX_OUT_OF_RANGE == sbox_status) printf("\n>>> Debe ser 2 o 3");
                if (SBOX_MEMORY_ERROR == sbox_status) printf("\n>>> Hubo un error al reservar memoria para la S-Box");
                if (SBOX_OPEN_FILE_ERROR == sbox_status) printf("\n>>> Hubo un error al abrir el archivo de la S-Box");
                wait_key();
                break;
            case 2:
                printf("\n>> Escribe la cadena M: ");
                read_string(sizeof(M), M);
                printf("\n>> Elije una de las opciones:\n\n");
                printf("1.- Para una S-Box de 4 bits\n");
                printf("2.- Para una S-Box de 8 bits\n\n");
                printf("Opcion: ");
                scanf("%d", &option_bit);
                clean_buffer();

                switch (option_bit) {
                    case 1:
                        sbox_status = sbox_4bits(M);
                        if (SBOX_OK != sbox_status) printf("\n>>> Hubo un error al cargar la S-Box de 4 bits");
                        break;
                    case 2:
                        sbox_status = sbox_8bits(M);
                        if (SBOX_OK != sbox_status) printf("\n>>> Hubo un error al cargar la S-Box de 8 bits");
                        break;
                    default:
                        printf("\n>> Opcion no valida");
                        break;
                }

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
                    sbox_status = sbox_16bits(K);
                    if (SBOX_OK != sbox_status) printf("\n>>> Hubo un error al cargar alguna S-Box de 8 bits");
                } 

                wait_key();
                break;
            case 4:
                printf("\n>> Escribe un numero (0 a 65535): ");
                scanf("%d", &temp);
                clean_buffer();

                if (temp < 0 || temp > 65535) {
                    printf("\n>>> El numero no es valido");
                } else {
                    K = (unsigned short)temp;
                    sbox_status = sbox_4bits_k(K);
                    if (SBOX_OK != sbox_status) printf("\n>>> Hubo un error al cargar la S-Box de 4 bits");
                }

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

/**
 * @brief Genera una S-Box de 4 u 8 bits.
 * 
 * @param n Tal que el tamaño de la S-Box es `l` = 2^`n` bits.
 * 
 * @return
 * - SBOX_OK se la S-Box se genero correctamente.
 * - SBOX_OPEN_FILE_ERROR si hubo un error al guardar la S-Box.
 * - SBOX_MEMORY_ERROR si hubo un error al reservar memoria para la S-Box.
 */
SBoxStatus sbox_generator(int n) {
    if (!(n == 2 || n == 3)) return SBOX_OUT_OF_RANGE;

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

/**
 * @brief Obtiene el nibble alto (4 bits más significativos) de un byte.
 * 
 * @param c Un byte.
 * 
 * @return
 * Nibble alto del byte.
 */
static unsigned char get_4MSB(char c) {
    return (c >> 4) & 0x0F;
}

/**
 * @brief Obtiene el nibble bajo (4 bits menos significativos) de un byte.
 * 
 * @param c Un byte.
 * 
 * @return
 * Nibble bajo del byte.
 */
static unsigned char get_4LSB(char c) {
    return c & 0x0F;
}

/**
 * @brief Combina el nibble alto y bajo en un byte.
 * 
 * @param high_nibble Nibble alto de un byte.
 * @param low_nibble Nibble bajo de un byte.
 * 
 * @return
 * Un Byte.
 */
static unsigned char shuffle_bits(unsigned char high_nibble, unsigned char low_nibble) {
    return (high_nibble << 4) | low_nibble;
}

static int hex_to_int(char c) {
    int n;
    if (c >= '0' && c <= '9') {
        n = c - '0';
    } else {
        n = c - 'A' + 10;
    }

    return n;
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
static SBoxStatus load_sbox(char textfile_name[], int sbox[], int bits) {
    FILE *fp = fopen(textfile_name, "r");
    if (!fp) return SBOX_OPEN_FILE_ERROR;

    int input, output;

    if (bits == 4) {    
        char in, out;

        while (fscanf(fp, " %c -> %c", &in, &out) == 2) {
            input = hex_to_int(in);
            output = hex_to_int(out);
            sbox[input] = output;
        }
    }
    else if (bits == 8) {
        while (fscanf(fp, " %X -> %X", &input, &output) == 2) {
            sbox[input] = output;
        }
    }

    fclose(fp);
    return SBOX_OK;
}

/**
 * @brief Sustitución con una S-Box de 4 bits sobre una cadena caracteres.
 * 
 * @param M Arreglo con la cadena de caracteres.
 * 
 * @return
 * - SBOX_OK si la sustitución fue correcta.
 * - SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 */
SBoxStatus sbox_4bits(char M[]) {
    unsigned char high_nibble, low_nibble, temp;
    int sbox_4[16], new_high_nibble, new_low_nibble;
    size_t size_m = strlen(M);
    unsigned char MS[size_m];

    SBoxStatus sbox_status = load_sbox("sbox_4bits.txt", sbox_4, 4);
    if (SBOX_OK != sbox_status) return sbox_status;
    
    for (int i = 0; i < size_m; i++) {
        printf("\nM[%d] (%c) = %02X", i, (unsigned char)M[i], (unsigned char)M[i]);
        
        high_nibble = get_4MSB(M[i]);
        low_nibble = get_4LSB(M[i]);
        new_high_nibble = sbox_4[high_nibble];
        new_low_nibble = sbox_4[low_nibble];
        
        printf("\nNibble alto = %01X -> S(%01X) = %01X", high_nibble, high_nibble, new_high_nibble);
        printf("\nNibble bajo = %01X -> S(%01X) = %01X", low_nibble, low_nibble, new_low_nibble);

        temp = shuffle_bits(new_high_nibble, new_low_nibble);
        printf("\nResultado = %02X\n", temp);

        MS[i] = temp;
    }

    printf("\nLa cadena despues de la sustitucion es: ");
    for (int i = 0; i < size_m; i++) {
        printf("%02X", MS[i]);
    }

    return SBOX_OK;
}

/**
 * @brief Sustitución con una S-Box de 8 bits sobre una cadena caracteres.
 * 
 * @param M Arreglo con la cadena de caracteres.
 * 
 * @return
 * - SBOX_OK si la sustitución fue correcta.
 * - SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 */
SBoxStatus sbox_8bits(char M[]) {
    int sbox_8[256];
    size_t size_m = strlen(M);
    unsigned char MS[size_m], new_byte;

    SBoxStatus sbox_status = load_sbox("sbox_8bits.txt", sbox_8, 8);
    if (SBOX_OK != sbox_status) return sbox_status;

    for (int i = 0; i < size_m; i++) {
        printf("\nM[%d] (%c) = %02X", i, (unsigned char)M[i], (unsigned char)M[i]);

        new_byte = sbox_8[(unsigned char)M[i]];

        printf("\nResultado = %02X -> S(%02X) = %02X\n", (unsigned char)M[i], (unsigned char)M[i], new_byte);

        MS[i] = new_byte;
    }

    printf("\nLa cadena despues de la sustitucion es: ");
    for (int i = 0; i < size_m; i++) {
        printf("%02X", MS[i]);
    }

    return SBOX_OK;
}

/**
 * @brief Obtiene el byte alto (8 bits más significativos) de dos bytes.
 * 
 * @param K Un número válido de 16 bits.
 * 
 * @return
 * Byte alto de dos bytes.
 */
static unsigned char get_8MSB(unsigned short K) {
    return (K >> 8) & 0xFF;
}

/**
 * @brief Obtiene el byte bajo (8 bits menos significativos) de dos bytes.
 * 
 * @param K Un número válido de 16 bits.
 * 
 * @return
 * Byte bajo de dos bytes.
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
 * @return
 * Dos bytes.
 */
static unsigned short shuffle_bytes(unsigned char high_byte, unsigned char low_byte) {
    return ((unsigned short)high_byte << 8) | low_byte;
}

/**
 * @brief Sustitución con dos S-Box de 8 bits sobre un número de 16 bits.
 * 
 * @param K Número de 16 bits.
 * 
 * @return
 * - SBOX_OK si la sustitución fue correcta.
 * - SBOX_OPEN_FILE_ERROR si hubo un error al cargar alguna de las S-Box.
 */
SBoxStatus sbox_16bits(unsigned short K) {
    unsigned char high_byte, low_byte, new_high_byte, new_low_byte;
    int sbox_8[256], sbox_8_1[256];
    unsigned short KS;

    SBoxStatus sbox_status = load_sbox("sbox_8bits.txt", sbox_8, 8);
    if (SBOX_OK != sbox_status) return sbox_status;
    SBoxStatus sbox_status_1 = load_sbox("sbox_8bits_1.txt", sbox_8_1, 8);
    if (SBOX_OK != sbox_status_1) return sbox_status_1;

    printf("\nK = %04X", K);
    
    high_byte = get_8MSB(K);
    low_byte = get_8LSB(K);
    new_high_byte = sbox_8[high_byte];
    new_low_byte = sbox_8_1[low_byte];

    printf("\nByte alto = %02X -> S(%02X) = %02X", high_byte, high_byte, new_high_byte);
    printf("\nByte bajo = %02X -> S(%02X) = %02X\n", low_byte, low_byte, new_low_byte);

    KS = shuffle_bytes(new_high_byte, new_low_byte);
    printf("\nEl numero despues de la sustitucion es: %04X", KS);

    return SBOX_OK;
}

/**
 * @brief Sustitución en blques de 4 bits sobre un número de 16 bits.
 * 
 * @param K Número de 16 bits.
 * 
 * - SBOX_OK si la sustitución fue correcta.
 * - SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 */
SBoxStatus sbox_4bits_k(unsigned short K) {
    unsigned char high_byte, low_byte, new_high_byte, new_low_byte;
    unsigned char high_nibble, low_nibble, new_high_nibble, new_low_nibble;
    int sbox_4[16];
    unsigned short KS;

    SBoxStatus sbox_status = load_sbox("sbox_4bits.txt", sbox_4, 4);
    if (SBOX_OK != sbox_status) return sbox_status;

    printf("\nK = %04X", K);

    // Obtenemos los bytes
    high_byte = get_8MSB(K);
    low_byte = get_8LSB(K);

    printf("\nByte alto = %02X", high_byte);

    // Obtenemos los nibbles del byte alto
    high_nibble = get_4MSB(high_byte);
    low_nibble = get_4LSB(high_byte);
    new_high_nibble = sbox_4[high_nibble];
    new_low_nibble = sbox_4[low_nibble];

    printf("\n\nNibble alto = %01X -> S(%01X) = %01X", high_nibble, high_nibble, new_high_nibble);
    printf("\nNibble bajo = %01X -> S(%01X) = %01X", low_nibble, low_nibble, new_low_nibble);

    new_high_byte = shuffle_bits(new_high_nibble, new_low_nibble);
    printf("\nResultado = %02X", new_high_byte);

    printf("\n\nByte bajo = %02X", low_byte);

    // Obtenemos lo nibbles del byte bajo
    high_nibble = get_4MSB(low_byte);
    low_nibble = get_4LSB(low_byte);
    new_high_nibble = sbox_4[high_nibble];
    new_low_nibble = sbox_4[low_nibble];

    printf("\n\nNibble alto = %01X -> S(%01X) = %01X", high_nibble, high_nibble, new_high_nibble);
    printf("\nNibble bajo = %01X -> S(%01X) = %01X", low_nibble, low_nibble, new_low_nibble);

    new_low_byte = shuffle_bits(new_high_nibble, new_low_nibble);
    printf("\nResulatdo = %02X\n", new_low_byte);

    KS = shuffle_bytes(new_high_byte, new_low_byte); 
    printf("\nEl numero despues de la sustitucion es: %04X", KS);

    return SBOX_OK;
}