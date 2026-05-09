#include "utils_crypto.h"
#include <stdio.h>
#include <string.h>

void wait_key() {
    printf("\n\nPresiona Enter para continuar...");
    getchar();
    getchar();
}

void clean_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void read_string(int size, char string[]) {
    if (fgets(string, size, stdin)) {
        string[strcspn(string, "\n")] = '\0';
    } else {
        string[0] = '\0';
    }
}
