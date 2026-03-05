#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define SIZE 20

void random_permutation(int n, int P[]) {
    int i, j, temp;

    // inicializar el arreglo
    for (i = 0; i < n; i++){
        P[i] = i + 1;
    }

    // Mazclar arreglo
    for (i = n - 1; i > 0; i-- ) {
        j = rand() % (i + 1);
        
        temp = P[i];
        P[i] = P[j];
        P[j] = temp;
    }
}

void inverse_permutation(int P[], int Inv[], int n) {
    for (int i = 0; i < n; i++) {
        Inv[P[i] - 1] = i + 1;
    }
}

int main(int argc, char const *argv[]) {
    int n = SIZE;
    int P[SIZE];
    int P2[SIZE] = {1, 9, 8, 10, 20, 13, 18, 7, 4, 2, 6, 17, 11, 12, 3, 5, 14, 15, 16, 19};
    int Inv[SIZE];

    srand(time(NULL));

    random_permutation(n, P);

    printf("Permutación aleatoria:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", P[i]);
    }

    inverse_permutation(P2, Inv, n);

    printf("\nPermutación inversa:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", Inv[i]);
    }

    return 0;
}
