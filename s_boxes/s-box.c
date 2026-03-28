#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

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
static void print_binary(char c, int num_bits);
static unsigned char get_MSB(char c);
static unsigned char get_LSB(char c);
static unsigned char shuffle_bits(unsigned char high_nibble, unsigned char low_nibble);
static SBoxStatus load_sbox(char textfile_name[], int sbox[]);
static int hex_to_int(char c);
SBoxStatus sbox_4_8bits(char M[]);

int main(int argc, char const *argv[]) {
    int option = 0, option_bit  = 0, n = 0;
    char M[200];
    unsigned short int K;
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
        printf("3.- Opcion 3\n");
        printf("4.- Opcion 4\n");
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
                if (SBOX_OUT_OF_RANGE == sbox_status) printf("\n>>> Deber estar entre 2 y 4");
                if (SBOX_MEMORY_ERROR == sbox_status) printf("\n>> Hubo un error al reservar memoria para la S-Box");
                if (SBOX_OPEN_FILE_ERROR == sbox_status) printf("\n>> Hubo un error al abrir el archivo de la S-Box");
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
                        sbox_4_8bits(M);
                        break;
                    case 2:
                        break;
                    default:
                        printf("\n>> Opcion no valida");
                        break;
                }

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

static void read_string(int size, char string[size]) {
    if (fgets(string, size, stdin)) {
        string[strcspn(string, "\n")] = '\0';
    } else {
        string[0] = '\0';
    }
}

static void print_binary(char c, int num_bits) {
    for (int i = num_bits - 1; i >= 0; i--) {
        printf("%u", (c >> i) & 1);
    }
}

static unsigned char get_MSB(char c) {
    return (c >> 4) & 0x0F;
}

static unsigned char get_LSB(char c) {
    return c & 0x0F;
}

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

static SBoxStatus load_sbox(char textfile_name[], int sbox[]) {
    FILE *fp = fopen(textfile_name, "r");
    if (!fp) return SBOX_OPEN_FILE_ERROR;
    
    char in, out;
    int input, output;

    while (fscanf(fp, " %c -> %c", &in, &out) == 2) {
        input = hex_to_int(in);
        output = hex_to_int(out);
        sbox[input] = output;
    }

    fclose(fp);
    return SBOX_OK;
}

SBoxStatus sbox_4_8bits(char M[]) {
    unsigned char high_nibble, low_nibble;
    unsigned char temp;
    int sbox_4[16], sbox_8[256], new_high_nibble, new_low_nibble;
    size_t size_m = strlen(M);
    unsigned char MS[size_m];

    SBoxStatus sbox_status = load_sbox("sbox_4bits.txt", sbox_4);
    if (sbox_status != SBOX_OK) return sbox_status;
    
    //load_sbox("sbox_8bits.txt", sbox_8);
    
    for (int i = 0; i < size_m; i++) {
        printf("\nM[%d] (%c) = %02x", i, (unsigned char)M[i], (unsigned char)M[i]);
        
        high_nibble = get_MSB(M[i]);
        low_nibble = get_LSB(M[i]);
        new_high_nibble = sbox_4[high_nibble];
        new_low_nibble = sbox_4[low_nibble];
        
        printf("\nNibble alto = %x -> S(%x) = %x", high_nibble, high_nibble, new_high_nibble);
        printf("\nNibble bajo = %x -> S(%x) = %x", low_nibble, low_nibble, new_low_nibble);

        temp = shuffle_bits(new_high_nibble, new_low_nibble);
        printf("\nResultado = %x\n", temp);

        MS[i] = temp;
    }

    printf("\nLa cadena despues de la sustitucion es: ");
    for (int i = 0; i < size_m; i++) {
        printf("%x", MS[i]);
    }

    return SBOX_OK;
}