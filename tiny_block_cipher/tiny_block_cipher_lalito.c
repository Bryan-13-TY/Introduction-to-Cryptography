/**
 * Pequeño cifrador de bloque.
 *
 *
 */

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
} BlockCStatus;

static void wait_key();
static void clean_buffer();
static void fill_array(int size, unsigned char array[size]);
static void shuffle_array(int size, unsigned char array[size]);
static void generate_textfile_name(char name[], int max_size, int sbox_size);
static BlockCStatus store_permutation(int size, unsigned char array[size], char textfile_name[]);
BlockCStatus sbox_generator();
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
static void key_expansion(unsigned short K, unsigned char sbox[], unsigned short sub_keys[], int c);
BlockCStatus encrypt(char sbox_filename[], char key_filename[], char block[], unsigned short sub_keys[]);
static BlockCStatus load_inverse_sbox(char sbox_filename[], unsigned char inverse_sbox[]);
static void unformat_block(unsigned short value, char block[]);
BlockCStatus decrypt(char sbox_filename[], char key_filename[], char block[], unsigned short sub_keys[]);

int main(int argc, char const *argv[])
{
    int option = 0;
    char key_filename[100], sbox_filename[100], block[10];
    unsigned short sub_keys[3];
    BlockCStatus blockc_status;

    srand(time(NULL));

    do
    {
        printf("\033[2J\033[H");
        printf("/*------------------.\n");
        printf("| Tiny Block Cipher |\n");
        printf("`------------------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Generar una S-Box de 8 bits\n");
        printf("2.- Generar una clave secreta\n");
        printf("3.- Cifrar un plaintext\n");
        printf("4.- Descifrar un ciphertext\n");
        printf("5.- Salir del programa\n\n");
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
            printf("\n>> Escribe el nombre del archivo con la llave: ");
            read_string(sizeof(key_filename), key_filename);
            printf(">> Escribe el nombre del archivo con la S-Box: ");
            read_string(sizeof(sbox_filename), sbox_filename);
            printf(">> Escribe el bloque a cifrar (2 caracteres): ");
            read_string(sizeof(block), block);
            if (strlen(block) == 2)
            {
                blockc_status = encrypt(sbox_filename, key_filename, block, sub_keys);
                if (BLOCKC_KEY_OPEN_FILE_ERROR == blockc_status)
                    printf("\n>>> Hubo un error al cargar la llave");
                if (BLOCKC_SBOX_OPEN_FILE_ERROR == blockc_status)
                    printf("\n>>> Hubo un error al cargar la S-Box");
                if (BLOCKC_KEY_READ_ERROR == blockc_status)
                    printf("\n>>> Hubo un erro al leer la llave");
            }
            else
            {
                printf("\n>> El tamano del bloque no es correcto");
            }
            wait_key();
            break;
        case 4:
            printf("\n>> Nombre del la llave (.txt): ");
            read_string(sizeof(key_filename), key_filename);
            printf(">> Nombre de la S-Box (.txt): ");
            read_string(sizeof(sbox_filename), sbox_filename);
            printf(">> Escribe el bloque a descifrar (2 caracteres): ");
            read_string(sizeof(block), block);
            if (strlen(block) == 2)
            {
                blockc_status = decrypt(sbox_filename, key_filename, block, sub_keys);
                if (BLOCKC_KEY_OPEN_FILE_ERROR == blockc_status)
                    printf("\n>>> Hubo un error al cargar la llave");
                if (BLOCKC_SBOX_OPEN_FILE_ERROR == blockc_status)
                    printf("\n>>> Hubo un error al cargar la S-Box o la S-Box inversa");
                if (BLOCKC_KEY_READ_ERROR == blockc_status)
                    printf("\n>>> Hubo un erro al leer la llave");
            }
            else
            {
                printf("\n>> El tamano del bloque no es correcto");
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
static void key_expansion(unsigned short K, unsigned char sbox[], unsigned short sub_keys[], int c)
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

    printf("\n>> Las sub-llaves son: \n\n");
    if (c == 0)
    {
        for (int i = 0; i < 3; i++)
        {
            printf("K%d: %04X\n", i, sub_keys[i]);
        }
    }
    else
    {
        for (int i = 3; i > 0; i--)
        {
            printf("K%d: %04X\n", i, sub_keys[i]);
        }
    }
}

/**
 * @brief Cifra un bloque de 2 bytes.
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
BlockCStatus encrypt(char sbox_filename[], char key_filename[], char block[], unsigned short sub_keys[])
{
    unsigned short K;
    unsigned char sbox[256], L, R;
    unsigned short M = format_block(block);
    char string[3];
    BlockCStatus blockc_status;

    // Cargar la llave y la S-Box
    blockc_status = load_key(key_filename, &K);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    blockc_status = load_sbox(sbox_filename, sbox);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    // Expandir la llave
    key_expansion(K, sbox, sub_keys, 0);

    // Cifrado
    for (int i = 0; i < 3; i++)
    {
        printf("\nIteracion %d:\n", i + 1);
        printf("\nM <- %04X ^ %04X", M, sub_keys[i]);
        M = M ^ sub_keys[i];
        printf(": %04X", M);

        L = get_8MSB(M);
        R = get_8LSB(M);
        L = sbox[L];
        R = sbox[R];

        printf("\nM <- S(%04X)", M);
        M = shuffle_bytes(L, R);
        printf(": %04X\n", M);
    }

    unformat_block(M, string);

    printf("\n>> El bloque cifrado es: %04X -> %s", M, string);

    return BLOCKC_OK;
}

/**
 * @brief Carga la S-Box inversa desde un archivo de texto.
 *
 * @param sbox_filename Nombre del archivo.
 * @param inverse_sbox Arreglo en donde se guarga la S-Box inversa.
 *
 * @return
 * - BLOCKC_OK si se cargo correctamente.
 * - BLOCKC_SBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo.
 */
static BlockCStatus load_inverse_sbox(char sbox_filename[], unsigned char sbox_inversa[])
{
    FILE *fp = fopen(sbox_filename, "r");
    if (!fp)
        return BLOCKC_SBOX_OPEN_FILE_ERROR;

    int size = 1 << 8;
    for (int i = 0; i < size; i++)
    {
        sbox_inversa[i] = 0;
    }

    int input, output;
    while (fscanf(fp, " %X -> %X", &input, &output) == 2)
    {
        if (input >= 0 && input < 256 && output >= 0 && output < 256)
        {
            sbox_inversa[output] = (unsigned char)input;
        }
    }

    fclose(fp);
    return BLOCKC_OK;
}

/**
 * @brief Convierte un bloque de 2 bytes a una cadena de 2 caracteres.
 *
 * @param value Bloque de dos 2 bytes.
 * @param block Arreglo donde se guardan los 2 caracteres.
 */
static void unformat_block(unsigned short value, char block[])
{
    block[0] = (value >> 8) & 0xFF;
    block[1] = value & 0xFF;
    block[2] = '\0';
}

/**
 * @brief Descifra un bloque de 2 bytes.
 *
 * @param sbox_filename Archivo con la S-Box.
 * @param key_filename Archivo con la llave secreta.
 * @param block Bloque a descifrar.
 * @param sub_keys Arreglo con las sub-llaves.
 *
 * @return
 * - BLOCKC_OK si el cifrado se hizo correctamente.
 * - BLOCKC_SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box o la S-Box inversa.
 * - BLOCKC_KEY_OPEN_FILE_ERROR si hubo un error al cargar la llave.
 * - BLOCKC_KEY_READ_ERROR si hubo un error al leer la llave.
 */
BlockCStatus decrypt(char sbox_filename[], char key_filename[], char block[], unsigned short sub_keys[])
{
    unsigned short K;
    unsigned char sbox[256], sbox_inversa[256], A, B;
    unsigned short C = format_block(block);
    char string[3];
    BlockCStatus blockc_status;

    // Cargar la llave y la S-Box inversa
    blockc_status = load_key(key_filename, &K);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    blockc_status = load_sbox(sbox_filename, sbox);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;
    blockc_status = load_inverse_sbox(sbox_filename, sbox_inversa);
    if (BLOCKC_OK != blockc_status)
        return blockc_status;

    key_expansion(K, sbox, sub_keys, 1);

    printf("\n Caracteres cifrados: %04X (%c%c)\n", C, block[0], block[1]);

    for (int i = 2; i >= 0; i--)
    {
        printf("\nIteracion %d:\n", 3 - i);
        A = sbox_inversa[get_8MSB(C)];
        B = sbox_inversa[get_8LSB(C)];

        printf("\nC <- SBOX INVERSA( %04X )", C);
        C = shuffle_bytes(A, B);
        printf(". Cambio en la sbox inversa: %04X", C);

        printf("\nC <- %X XOR %X", C, sub_keys[i]);
        C = C ^ sub_keys[i];
        printf(": %04X\n", C);
    }

    unformat_block(C, string);

    printf("\n>> El bloque descifrado es: %04X -> %s", C, string);

    return BLOCKC_OK;
}
