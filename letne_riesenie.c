#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define EPSILON 1e-9
#define BUFFER_SIZE 256

int nacitajKladneCislo(const char *vyzva);
double nacitajRealneCislo(const char *vyzva);
char nacitajVolbuPokracovania(void);
void nacitajRozmery(int *rowsA, int *colsA, int *colsB);
double **alokujMaticu(int riadky, int stlpce);
void freeMaticu(double **matica, int riadky);
void nacitajMaticu(double **matica, int riadky, int stlpce, char nazov);
void nasobenieMatic(double **A, double **B, double **C, int rowsA, int colsA, int colsB);
void vypocitajSumyRiadkov(double **C, double *sumyRiadkov, int rowsA, int colsB);
double najdiMaximalnuSumu(const double *sumyRiadkov, int rowsA);
void vypisSumyRiadkov(const double *sumyRiadkov, int rowsA);
void vypisNajvacsieRiadky(const double *sumyRiadkov, int rowsA, double maxSuma);

static void odstranKoniecRiadka(char *retazec) {
    retazec[strcspn(retazec, "\n")] = '\0';
}

static int obsahujeLenMedzery(const char *retazec) {
    while (*retazec != '\0') {
        if (!isspace((unsigned char)*retazec)) {
            return 0;
        }
        retazec++;
    }
    return 1;
}

int main(void) {
    char opakovat = 'a';

    while (opakovat == 'a' || opakovat == 'A') {
        int rowsA, colsA, colsB;

        nacitajRozmery(&rowsA, &colsA, &colsB);

        double **A = alokujMaticu(rowsA, colsA);
        double **B = alokujMaticu(colsA, colsB);
        double **C = alokujMaticu(rowsA, colsB);
        double *sumyRiadkov = (double *)malloc(rowsA * sizeof(double));

        if (sumyRiadkov == NULL) {
            printf("chyba alokacie pamate pre pole sum riadkov!\n");
            freeMaticu(A, rowsA);
            freeMaticu(B, colsA);
            freeMaticu(C, rowsA);
            return 1;
        }

        nacitajMaticu(A, rowsA, colsA, 'A');
        nacitajMaticu(B, colsA, colsB, 'B');

        nasobenieMatic(A, B, C, rowsA, colsA, colsB);
        vypocitajSumyRiadkov(C, sumyRiadkov, rowsA, colsB);

        printf("\nsucty riadkov vyslednej matice C:\n");
        vypisSumyRiadkov(sumyRiadkov, rowsA);

        double maxSuma = najdiMaximalnuSumu(sumyRiadkov, rowsA);
        vypisNajvacsieRiadky(sumyRiadkov, rowsA, maxSuma);

        free(sumyRiadkov);
        freeMaticu(A, rowsA);
        freeMaticu(B, colsA);
        freeMaticu(C, rowsA);

        opakovat = nacitajVolbuPokracovania();
    }

    printf("program skoncil!\n");
    return 0;
}

int nacitajKladneCislo(const char *vyzva) {
    char buffer[BUFFER_SIZE];
    char *endptr;
    long hodnota;

    while (1) {
        printf("%s ", vyzva);
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("chyba pri nacitani vstupu!\n");
            exit(1);
        }

        odstranKoniecRiadka(buffer);

        if (buffer[0] == '\0' || obsahujeLenMedzery(buffer)) {
            printf("neplatny vstup, zadaj kladne cele cislo.\n");
            continue;
        }

        hodnota = strtol(buffer, &endptr, 10);

        while (isspace((unsigned char)*endptr)) {
            endptr++;
        }

        if (*endptr != '\0' || hodnota <= 0) {
            printf("neplatny vstup, zadaj kladne cele cislo.\n");
            continue;
        }

        return (int)hodnota;
    }
}

double nacitajRealneCislo(const char *vyzva) {
    char buffer[BUFFER_SIZE];
    char *endptr;
    double hodnota;

    while (1) {
        printf("%s", vyzva);
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("chyba pri nacitani vstupu!\n");
            exit(1);
        }

        odstranKoniecRiadka(buffer);

        if (buffer[0] == '\0' || obsahujeLenMedzery(buffer)) {
            printf("neplatny vstup, zadaj realne cislo.\n");
            continue;
        }

        hodnota = strtod(buffer, &endptr);

        while (isspace((unsigned char)*endptr)) {
            endptr++;
        }

        if (*endptr != '\0') {
            printf("neplatny vstup, zadaj realne cislo.\n");
            continue;
        }

        return hodnota;
    }
}

char nacitajVolbuPokracovania(void) {
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\nchces pokracovat odznova? (a/n): ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("chyba pri nacitani vstupu!\n");
            exit(1);
        }

        odstranKoniecRiadka(buffer);

        if (buffer[0] == 'a' || buffer[0] == 'A' || buffer[0] == 'n' || buffer[0] == 'N') {
            return buffer[0];
        }

        printf("neplatna volba, zadaj 'a' alebo 'n'.\n");
    }
}

void nacitajRozmery(int *rowsA, int *colsA, int *colsB) {
    *rowsA = nacitajKladneCislo("pocet riadkov matice A:");
    *colsA = nacitajKladneCislo("pocet stlpcov matice A:");
    *colsB = nacitajKladneCislo("pocet stlpcov matice B:");
}

double **alokujMaticu(int riadky, int stlpce) {
    double **matica = (double **)malloc(riadky * sizeof(double *));
    if (matica == NULL) exit(1);

    for (int i = 0; i < riadky; i++) {
        matica[i] = (double *)malloc(stlpce * sizeof(double));
        if (matica[i] == NULL) exit(1);
    }
    return matica;
}

void freeMaticu(double **matica, int riadky) {
    for (int i = 0; i < riadky; i++) free(matica[i]);
    free(matica);
}

void nacitajMaticu(double **matica, int riadky, int stlpce, char nazov) {
    char vyzva[BUFFER_SIZE];
    printf("\nnacitaj prvky matice %c (%d x %d):\n", nazov, riadky, stlpce);

    for (int i = 0; i < riadky; i++) {
        for (int j = 0; j < stlpce; j++) {
            snprintf(vyzva, sizeof(vyzva), "  matica %c[%d][%d] = ", nazov, i, j);
            matica[i][j] = nacitajRealneCislo(vyzva);
        }
    }
}

void nasobenieMatic(double **A, double **B, double **C, int rowsA, int colsA, int colsB) {
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < colsA; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void vypocitajSumyRiadkov(double **C, double *sumyRiadkov, int rowsA, int colsB) {
    for (int i = 0; i < rowsA; i++) {
        sumyRiadkov[i] = 0.0;
        for (int j = 0; j < colsB; j++) {
            sumyRiadkov[i] += C[i][j];
        }
    }
}

double najdiMaximalnuSumu(const double *sumyRiadkov, int rowsA) {
    double maxSuma = sumyRiadkov[0];
    for (int i = 1; i < rowsA; i++) {
        if (sumyRiadkov[i] > maxSuma) maxSuma = sumyRiadkov[i];
    }
    return maxSuma;
}

void vypisSumyRiadkov(const double *sumyRiadkov, int rowsA) {
    for (int i = 0; i < rowsA; i++) {
        printf("  suma riadku %d = %.2f\n", i + 1, sumyRiadkov[i]);
    }
}

void vypisNajvacsieRiadky(const double *sumyRiadkov, int rowsA, double maxSuma) {
    printf("\nriadok/riadky s najvacsou sumou (%.2f) su:\n", maxSuma);
    for (int i = 0; i < rowsA; i++) {
        if (fabs(sumyRiadkov[i] - maxSuma) < EPSILON) {
            printf("  -> riadok %d\n", i + 1);
        }
    }
}