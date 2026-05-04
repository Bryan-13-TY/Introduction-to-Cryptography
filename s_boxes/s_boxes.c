#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef enum
{
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
static SBoxStatus load_sbox(const char *filename, int block_bits, int sbox_array[]);
void sbox_4bits_k(unsigned short int K);

int main(int argc, char const *argv[])
{
    int option = 0, n = 0;
    SBoxStatus sbox_status;
    unsigned short int l;
    srand(time(NULL));

    do
    {
        printf("\033[2J\033[H");
        printf("/*------.\n");
        printf("| S-Box |\n");
        printf("`------*/\n\n");
        printf(">> Elije una de las opciones:\n\n");
        printf("1.- Generar una S-Box\n");
        printf("2.- Opcion 2\n");
        printf("3.- Opcion 3\n");
        printf("4.- Aplicar una S-Box de 4 bits a una K (short int): \n");
        printf("5.- Salir de programa\n\n");
        printf("Opcion: ");
        scanf("%d", &option);
        clean_buffer();

        switch (option)
        {
        case 1:
            printf("\n>> Escribe el tamano de la S-Box: ");
            scanf("%d", &n);

            sbox_status = sbox_generator(n);
            if (SBOX_OK == sbox_status)
                printf("\n>>> La S-Box se guardo correctamente");
            if (SBOX_OUT_OF_RANGE == sbox_status)
                printf("\n>>> Deber estar entre 2 y 4");
            if (SBOX_MEMORY_ERROR == sbox_status)
                printf("\n>> Hubo un error al reservar memoria para la S-Box");
            if (SBOX_OPEN_FILE_ERROR == sbox_status)
                printf("\n>> Hubo un error al abrir el archivo de la S-Box");
            wait_key();
            break;
        case 2:
            wait_key();
            break;
        case 3:
            wait_key();
            break;
        case 4:
            printf("\nEscribe la K: ");
            scanf("%x", &l);
            // Primero verificar que exista la S-box de 4 bits
            FILE *test = fopen("sbox_4bits.txt", "r");
            if (!test)
            {
                printf("\n>>> No existe una S-box de 4 bits\n");
                printf(">>> Generando una automaticamente...\n");
                sbox_generator(2); // n=2 genera S-box de 4 bits
                printf(">>> S-box generada y guardada\n");
            }
            else
            {
                fclose(test);
            }
            sbox_4bits_k(l);
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
static void fill_array(int size, int array[size])
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
static void shuffle_array(int size, int array[size])
{
    for (int i = size - 1; i > 0; i--)
    {
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
static void generate_textfile_name(char name[], int max_size, int num_bits)
{
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
static SBoxStatus store_permutation(int size, int array[size], char textfile_name[])
{
    FILE *fp = fopen(textfile_name, "w");
    if (!fp)
        return SBOX_OPEN_FILE_ERROR;

    for (int i = 0; i < size; i++)
    {
        fprintf(fp, "%X -> %X\n", i, array[i]);
    }

    fclose(fp);
    return SBOX_OK;
}

SBoxStatus sbox_generator(int n)
{
    if (!(n >= 2 && n <= 4))
        return SBOX_OUT_OF_RANGE;

    int l = 1 << n;
    int size_permutation = 1 << l;

    int *array = malloc(size_permutation * sizeof(int));
    if (!array)
        return SBOX_MEMORY_ERROR;

    char textfile_name[50];

    fill_array(size_permutation, array);
    shuffle_array(size_permutation, array);
    generate_textfile_name(textfile_name, sizeof(textfile_name), l);

    SBoxStatus sbox_status = store_permutation(size_permutation, array, textfile_name);

    free(array);
    return sbox_status;
}
/**
 * @brief Carga una S-box desde un archivo
 *
 * @param filename Nombre del archivo
 * @param block_bits Número de bits de la S-box (4 u 8)
 * @param sbox_array Arreglo donde se guardará la S-box (debe tener tamaño 2^block_bits)
 * @return SBoxStatus
 */
static SBoxStatus load_sbox(const char *filename, int block_bits, int sbox_array[])
{
    int table_size = 1 << block_bits;

    FILE *fp = fopen(filename, "r");
    if (!fp)
        return SBOX_OPEN_FILE_ERROR;

    // Leer cada línea del archivo con formato "i -> valor"
    for (int i = 0; i < table_size; i++)
    {
        int input, output;
        // Leer formato hexadecimal: X -> X
        if (fscanf(fp, "%X -> %X", &input, &output) != 2)
        {
            fclose(fp);
            return SBOX_OPEN_FILE_ERROR;
        }
        sbox_array[input] = output;
    }

    fclose(fp);
    return SBOX_OK;
}

void sbox_4bits_k(unsigned short int K)
{
    int sbox[16]; // 2^4 = 16 entradas
    char filename[50];
    snprintf(filename, sizeof(filename), "sbox_4bits.txt");
    SBoxStatus status = load_sbox(filename, 4, sbox);
    if (status != SBOX_OK)
    {
        printf("\n>>> Error: No se pudo cargar la S-box de 4 bits\n");
        printf(">>> Primero genera una S-box con la opcion 1 (n=2)\n");
        return;
    }

    printf("Sustitución de bloques de 4 bits: \n");

    printf("K original = 0x%04X\n", K);
    printf("Bits: ");
    for (int i = 15; i >= 0; i--)
    {
        printf("%d", (K >> i) & 1);
        if (i % 4 == 0 && i != 0)
            printf(" | ");
    }
    printf("\n\n");

    // Extraer cada bloque de 4 bits
    int bloque3 = (K >> 12) & 0xF; // bits 15-12
    int bloque2 = (K >> 8) & 0xF;  // bits 11-8
    int bloque1 = (K >> 4) & 0xF;  // bits 7-4
    int bloque0 = K & 0xF;         // bits 3-0

    printf("Bloques de 4 bits extraidos de la K original:\n");
    printf("  Bloque 3 (bits 15-12): 0x%X\n", bloque3);
    printf("  Bloque 2 (bits 11-8):  0x%X\n", bloque2);
    printf("  Bloque 1 (bits 7-4):   0x%X\n", bloque1);
    printf("  Bloque 0 (bits 3-0):   0x%X\n\n", bloque0);

    int s3 = sbox[bloque3];
    int s2 = sbox[bloque2];
    int s1 = sbox[bloque1];
    int s0 = sbox[bloque0];

    printf("Aplicar la S-box de 4 bits:\n");
    printf("  S(0x%X) = 0x%X\n", bloque3, s3);
    printf("  S(0x%X) = 0x%X\n", bloque2, s2);
    printf("  S(0x%X) = 0x%X\n", bloque1, s1);
    printf("  S(0x%X) = 0x%X\n\n", bloque0, s0);

    unsigned short int resultado = (s3 << 12) | (s2 << 8) | (s1 << 4) | s0;

    printf("Recombinar bloques:\n");
    printf("  0x%X << 12 = 0x%04X\n", s3, s3 << 12);
    printf("  0x%X << 8  = 0x%04X\n", s2, s2 << 8);
    printf("  0x%X << 4  = 0x%04X\n", s1, s1 << 4);
    printf("  0x%X << 0  = 0x%04X\n", s0, s0);
    printf("  OR         = 0x%04X\n\n", resultado);

    printf("Resultado final: 0x%04X\n", resultado);
}
