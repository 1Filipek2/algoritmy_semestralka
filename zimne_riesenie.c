#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int nacitRiadokStlpec();
void nacitRozmery(int *rowsA, int *colsA, int *colsB);
double** alokujMaticu(int riadky, int stlpce);
void freeMaticu(double **matica, int riadky);
void nacitMaticu(double **matica, int riadky, int stlpce, char nazov);
void nasobenieMatice(double **A, double **B, double **C, int rowsA, int colsA, int colsB);
void najvacsiRiadok(double **C, int rowsA, int colsB);
void vypisSumy(double **C, int rowsA, int colsB);

int main () {
    char opakovat = 'a';

    while(opakovat == 'a' || opakovat == 'A') {
        int rowsA, colsA, colsB;

        nacitRozmery(&rowsA,&colsA,&colsB);

        double **A = alokujMaticu(rowsA,colsA);
        double **B = alokujMaticu(colsA,colsB);
        double **C = alokujMaticu(rowsA,colsB);

        nacitMaticu(A, rowsA, colsA, 'A');
        nacitMaticu(B, colsA, colsB, 'B');

        nasobenieMatice(A, B, C, rowsA, colsA, colsB);

        vypisSumy(C, rowsA, colsB);

        najvacsiRiadok(C, rowsA, colsB);

        freeMaticu(A,rowsA);
        freeMaticu(B,colsA);
        freeMaticu(C,rowsA);

        printf("\nchces pokracovat odznova? (a/n): ");

        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        char buffer[10];
        fgets(buffer, sizeof(buffer), stdin);
        opakovat = buffer[0];
    }

    printf("program skoncil!\n");
    return 0;
}

int nacitRiadokStlpec() {
    char buffer[100];

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        int valid = 1;

        if (strlen(buffer) == 0)
            valid = 0;

        for (int i = 0; buffer[i] != '\0'; i++) {
            if (!isdigit(buffer[i])) {
                valid = 0;
                break;
            }
        }

        if (valid) {
            int value = atoi(buffer);
            if (value > 0)
                return value;
        }

        printf("neplatny vstup, zadaj kladne cele cislo: ");
    }
}
void nacitRozmery(int *rowsA, int *colsA, int *colsB) {
    printf("pocet riadkov matice A: ");
    *rowsA = nacitRiadokStlpec();

    printf("pocet stlpcov matice A: ");
    *colsA = nacitRiadokStlpec();

    printf("pocet stlpcov matice B: ");
    *colsB = nacitRiadokStlpec();
}

double** alokujMaticu(int riadky, int stlpce) {
    double **matica = (double**) malloc(riadky * sizeof(double*));
    if(!matica){
        printf("chyba alokacie pamate!\n");
        exit(1);
    }

    for(int i = 0; i < riadky; i++) {
        matica[i] = (double*) malloc(stlpce * sizeof(double));
        if(!matica[i]) {
            printf("chyba alokacie pamate pre riadok %d!\n", i);
            for(int j = 0; j < i; j++)
                free(matica[j]);
            free(matica);
            exit(1);
        }
    }

    return matica;
}

void freeMaticu(double **matica, int riadky) {
    for(int i = 0; i < riadky; i++)
        free(matica[i]);
    free(matica);
}

void nacitMaticu(double **matica, int riadky, int stlpce, char nazov) {
    printf("\nnacitaj prvky matice %c (%d x %d):\n", nazov, riadky, stlpce);
    for(int i = 0; i < riadky; i++){
        for(int j = 0; j < stlpce; j++){
            printf("matica %c[%d][%d] = ", nazov, i, j);
            while (scanf("%lf", &matica[i][j]) != 1){
                printf("neplatny vstup, zadaj realne cislo: ");
                while(getchar() != '\n');
            }
        }
    }
}

void nasobenieMatice(double **A, double **B, double **C, int rowsA, int colsA, int colsB) {
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            C[i][j] = 0;
            for (int k = 0; k < colsA; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void najvacsiRiadok(double **C, int rowsA, int colsB) {
    double maxSuma;

    for (int i = 0; i < rowsA; i++) {
        double suma = 0;
        for (int j = 0; j < colsB; j++) {
            suma += C[i][j];
        }

        if (i == 0 || suma > maxSuma) {
            maxSuma = suma;
        }
    }

    printf("riadok/riadky s najvacsou sumou (%.2f) su:\n", maxSuma);
    for (int i = 0; i < rowsA; i++) {
        double suma = 0;
        for (int j = 0; j < colsB; j++) {
            suma += C[i][j];
        }

        if (suma == maxSuma) {
            printf("riadok %d\n", i + 1);
        }
    }
}


void vypisSumy(double **C, int rowsA, int colsB) {
    for (int i = 0; i < rowsA; i++) {
        double suma = 0;
        for (int j = 0; j < colsB; j++)
            suma += C[i][j];
        printf("suma riadku %d = %.2lf\n", i + 1, suma);
    }
}