#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

typedef enum {
    S_BOX_OK = 0,
    S_BOX_OUT_OF_RANGE = -1,
    S_BOX_OPEN_FILE_ERROR = -2,
} SBoxStatus;

static void wait_key();
static void read_string(int size, char string[size]);
static void clean_buffer();
static void fill_array(int size, int array[size]);
static void print_array(int size, int array[size]);
static void shuffle_array(int size, int array[size]);
static SBoxStatus store_permutation(int size_permutation, int array[size_permutation]);
SBoxStatus s_boxes_generator(int n);

int main(int argc, char const *argv[]) {
    char textfile_name[100] = {0};
    SBoxStatus s_box_status;

    srand(time(NULL));
    s_box_status = s_boxes_generator(3);

    //read_string(sizeof(textfile_name), textfile_name);
    //printf("El nombre del archivo es: %s", textfile_name);

    return 0;
}

static void wait_key() {
    printf("\n\nPresiona Enter para continuar...");
    getchar();
    getchar();
}

static void read_string(int size, char string[size]) {
    if (fgets(string, size, stdin)) {
        string[strcspn(string, "\n")] = '\0';
    } else {
        string[0] = '\0';
    }
}

static void clean_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void print_array(int size, int array[size]) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
}

static void fill_array(int size, int array[size]) {
    for (int i = 0; i < size; i++) {
        array[i] = i;
    }
}

static void shuffle_array(int size, int array[size]) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    } 
}

static SBoxStatus store_permutation(int size_permutation, int array[size_permutation]) {
    FILE *fp = fopen("sbox.txt", "w");

    if (!fp) return S_BOX_OPEN_FILE_ERROR;

    for (int i = 0; i < size_permutation; i++) {
        fprintf(fp, "%X -> %X\n", i, array[i]);
    }

    fclose(fp);
}

SBoxStatus s_boxes_generator(int n) {
    if (!(n >= 2 && n <= 4)) return S_BOX_OUT_OF_RANGE;

    int l = 1 << n;
    int size_permutation = 1 << l;
    int array[size_permutation];

    fill_array(size_permutation, array);
    shuffle_array(size_permutation, array);
    store_permutation(size_permutation, array);
}
