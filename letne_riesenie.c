#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define EPSILON 1e-9
#define BUFFER_SIZE 256
#define STRASSEN_CUTOFF 64

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

// nove deklaracie po oprave
int nacitajVolbuRezim(void);
int nacitajVolbuMetody(void);
void vypisMaticu(double **M, int riadky, int stlpce, char nazov);
void generujMaticu(double **matica, int riadky, int stlpce, char nazov);
void nasobenieMaticStrassen(double **A, double **B, double **C, int rowsA, int colsA, int colsB);
void spustiTestovanie(void);

static void odstranKoniecRiadka(char *retazec) {
    retazec[strcspn(retazec, "\n")] = '\0';
}

static int obsahujeLenMedzery(const char *retazec) {
    while (*retazec != '\0') {
        if (!isspace((unsigned char)*retazec)) return 0;
        retazec++;
    }
    return 1;
}

// nova
static void vypocitajAVypis(double **A, double **B, int rowsA, int colsA, int colsB, int metoda) {
    double **C = alokujMaticu(rowsA, colsB);
    double *sumyRiadkov = (double *)malloc(rowsA * sizeof(double));

    if (sumyRiadkov == NULL) {
        printf("chyba alokacie pamate!\n");
        freeMaticu(C, rowsA);
        exit(1);
    }

    if (metoda == 2)
        nasobenieMaticStrassen(A, B, C, rowsA, colsA, colsB);
    else
        nasobenieMatic(A, B, C, rowsA, colsA, colsB);

    vypisMaticu(C, rowsA, colsB, 'C');

    vypocitajSumyRiadkov(C, sumyRiadkov, rowsA, colsB);
    printf("\nsucty riadkov vyslednej matice C:\n");
    vypisSumyRiadkov(sumyRiadkov, rowsA);

    double maxSuma = najdiMaximalnuSumu(sumyRiadkov, rowsA);
    vypisNajvacsieRiadky(sumyRiadkov, rowsA, maxSuma);

    free(sumyRiadkov);
    freeMaticu(C, rowsA);
}

int main(void) {
    srand((unsigned int)time(NULL));

    char opakovat = 'a';

    while (opakovat == 'a' || opakovat == 'A') {
        int rezim = nacitajVolbuRezim();

        if (rezim == 3) {
            spustiTestovanie();
        } else {
            int rowsA, colsA, colsB;
            nacitajRozmery(&rowsA, &colsA, &colsB);

            int metoda = nacitajVolbuMetody();

            double **A = alokujMaticu(rowsA, colsA);
            double **B = alokujMaticu(colsA, colsB);

            if (rezim == 2) {
                generujMaticu(A, rowsA, colsA, 'A');
                generujMaticu(B, colsA, colsB, 'B');
            } else {
                nacitajMaticu(A, rowsA, colsA, 'A');
                nacitajMaticu(B, colsA, colsB, 'B');
            }

            vypocitajAVypis(A, B, rowsA, colsA, colsB, metoda);

            freeMaticu(A, rowsA);
            freeMaticu(B, colsA);
        }

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

        while (isspace((unsigned char)*endptr)) endptr++;

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

        while (isspace((unsigned char)*endptr)) endptr++;

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

        if (buffer[0] == 'a' || buffer[0] == 'A' || buffer[0] == 'n' || buffer[0] == 'N')
            return buffer[0];

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

    for (int i = 0; i < riadky; i++)
        for (int j = 0; j < stlpce; j++) {
            snprintf(vyzva, sizeof(vyzva), "  matica %c[%d][%d] = ", nazov, i, j);
            matica[i][j] = nacitajRealneCislo(vyzva);
        }
}

void nasobenieMatic(double **A, double **B, double **C, int rowsA, int colsA, int colsB) {
    for (int i = 0; i < rowsA; i++)
        for (int j = 0; j < colsB; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < colsA; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
}

void vypocitajSumyRiadkov(double **C, double *sumyRiadkov, int rowsA, int colsB) {
    for (int i = 0; i < rowsA; i++) {
        sumyRiadkov[i] = 0.0;
        for (int j = 0; j < colsB; j++)
            sumyRiadkov[i] += C[i][j];
    }
}

double najdiMaximalnuSumu(const double *sumyRiadkov, int rowsA) {
    double maxSuma = sumyRiadkov[0];
    for (int i = 1; i < rowsA; i++)
        if (sumyRiadkov[i] > maxSuma) maxSuma = sumyRiadkov[i];
    return maxSuma;
}

void vypisSumyRiadkov(const double *sumyRiadkov, int rowsA) {
    for (int i = 0; i < rowsA; i++)
        printf("  suma riadku %d = %.2f\n", i + 1, sumyRiadkov[i]);
}

void vypisNajvacsieRiadky(const double *sumyRiadkov, int rowsA, double maxSuma) {
    printf("\nriadok/riadky s najvacsou sumou (%.2f) su:\n", maxSuma);
    for (int i = 0; i < rowsA; i++)
        if (fabs(sumyRiadkov[i] - maxSuma) < EPSILON)
            printf("  -> riadok %d\n", i + 1);
}

// nove funkcie po oprave

void vypisMaticu(double **M, int riadky, int stlpce, char nazov) {
    printf("\nvysledna matica %c (%d x %d):\n", nazov, riadky, stlpce);
    for (int i = 0; i < riadky; i++) {
        printf("  [");
        for (int j = 0; j < stlpce; j++)
            printf(" %8.2f", M[i][j]);
        printf("  ]\n");
    }
}

void generujMaticu(double **matica, int riadky, int stlpce, char nazov) {
    printf("\nmatica %c (%d x %d): prvky su automaticky vygenerovane\n", nazov, riadky, stlpce);
    for (int i = 0; i < riadky; i++)
        for (int j = 0; j < stlpce; j++)
            matica[i][j] = (double)(rand() % 20001 - 10000) / 100.0;
}

int nacitajVolbuRezim(void) {
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("=== Nasobenie matic ===\n");
        printf("  1 - Normalny program       (rozmery aj prvky zadava pouzivatel)\n");
        printf("  2 - Automaticke prvky      (pouzivatel zada rozmery, prvky sa dogeneruju)\n");
        printf("  3 - Testovanie rychlosti   (automaticky benchmark casu a pamate)\n");
        printf("volba: ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("chyba pri nacitani vstupu!\n");
            exit(1);
        }

        odstranKoniecRiadka(buffer);

        if (buffer[0] == '1' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 1;
        if (buffer[0] == '2' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 2;
        if (buffer[0] == '3' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 3;

        printf("neplatna volba, zadaj 1, 2 alebo 3.\n\n");
    }
}

int nacitajVolbuMetody(void) {
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\nvyber metodu nasobenia matic:\n");
        printf("  1 - standardny algoritmus O(n^3)     [vhodny pre male matice]\n");
        printf("  2 - Strassenov algoritmus O(n^2.807) [efektivnejsi pre velke matice]\n");
        printf("volba: ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("chyba pri nacitani vstupu!\n");
            exit(1);
        }

        odstranKoniecRiadka(buffer);

        if (buffer[0] == '1' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 1;
        if (buffer[0] == '2' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 2;

        printf("neplatna volba, zadaj 1 alebo 2.\n");
    }
}

// implementacia strassenov algoritmus

static int dalsiaPotenciu2(int n) {
    int p = 1;
    while (p < n) p *= 2;
    return p;
}

static void addMatice(double **A, double **B, double **C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] + B[i][j];
}

static void odocitajMatice(double **A, double **B, double **C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] - B[i][j];
}

static void strassenRekurzivne(double **A, double **B, double **C, int n) {
    if (n <= STRASSEN_CUTOFF) {
        nasobenieMatic(A, B, C, n, n, n);
        return;
    }

    int h = n / 2;

    double **A11 = alokujMaticu(h, h), **A12 = alokujMaticu(h, h);
    double **A21 = alokujMaticu(h, h), **A22 = alokujMaticu(h, h);
    double **B11 = alokujMaticu(h, h), **B12 = alokujMaticu(h, h);
    double **B21 = alokujMaticu(h, h), **B22 = alokujMaticu(h, h);
    double **M1  = alokujMaticu(h, h), **M2  = alokujMaticu(h, h);
    double **M3  = alokujMaticu(h, h), **M4  = alokujMaticu(h, h);
    double **M5  = alokujMaticu(h, h), **M6  = alokujMaticu(h, h);
    double **M7  = alokujMaticu(h, h);
    double **T1  = alokujMaticu(h, h), **T2  = alokujMaticu(h, h);

    for (int i = 0; i < h; i++)
        for (int j = 0; j < h; j++) {
            A11[i][j] = A[i][j];     A12[i][j] = A[i][j+h];
            A21[i][j] = A[i+h][j];   A22[i][j] = A[i+h][j+h];
            B11[i][j] = B[i][j];     B12[i][j] = B[i][j+h];
            B21[i][j] = B[i+h][j];   B22[i][j] = B[i+h][j+h];
        }

    addMatice(A11, A22, T1, h); addMatice(B11, B22, T2, h); strassenRekurzivne(T1, T2, M1, h); // M1
    addMatice(A21, A22, T1, h);                              strassenRekurzivne(T1, B11, M2, h); // M2
    odocitajMatice(B12, B22, T1, h);                         strassenRekurzivne(A11, T1, M3, h); // M3
    odocitajMatice(B21, B11, T1, h);                         strassenRekurzivne(A22, T1, M4, h); // M4
    addMatice(A11, A12, T1, h);                              strassenRekurzivne(T1, B22, M5, h); // M5
    odocitajMatice(A21, A11, T1, h); addMatice(B11, B12, T2, h); strassenRekurzivne(T1, T2, M6, h); // M6
    odocitajMatice(A12, A22, T1, h); addMatice(B21, B22, T2, h); strassenRekurzivne(T1, T2, M7, h); // M7

    for (int i = 0; i < h; i++)
        for (int j = 0; j < h; j++) {
            C[i][j]       = M1[i][j] + M4[i][j] - M5[i][j] + M7[i][j]; // C11
            C[i][j+h]     = M3[i][j] + M5[i][j];                         // C12
            C[i+h][j]     = M2[i][j] + M4[i][j];                         // C21
            C[i+h][j+h]   = M1[i][j] - M2[i][j] + M3[i][j] + M6[i][j]; // C22
        }

    freeMaticu(A11,h); freeMaticu(A12,h); freeMaticu(A21,h); freeMaticu(A22,h);
    freeMaticu(B11,h); freeMaticu(B12,h); freeMaticu(B21,h); freeMaticu(B22,h);
    freeMaticu(M1,h);  freeMaticu(M2,h);  freeMaticu(M3,h);  freeMaticu(M4,h);
    freeMaticu(M5,h);  freeMaticu(M6,h);  freeMaticu(M7,h);
    freeMaticu(T1,h);  freeMaticu(T2,h);
}

void nasobenieMaticStrassen(double **A, double **B, double **C, int rowsA, int colsA, int colsB) {
    int maxDim = rowsA > colsA ? rowsA : colsA;
    maxDim = maxDim > colsB ? maxDim : colsB;
    int n = dalsiaPotenciu2(maxDim);

    double **Ap = alokujMaticu(n, n);
    double **Bp = alokujMaticu(n, n);
    double **Cp = alokujMaticu(n, n);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            Ap[i][j] = 0.0;
            Bp[i][j] = 0.0;
            Cp[i][j] = 0.0;
        }

    for (int i = 0; i < rowsA; i++)
        for (int j = 0; j < colsA; j++)
            Ap[i][j] = A[i][j];

    for (int i = 0; i < colsA; i++)
        for (int j = 0; j < colsB; j++)
            Bp[i][j] = B[i][j];

    strassenRekurzivne(Ap, Bp, Cp, n);

    for (int i = 0; i < rowsA; i++)
        for (int j = 0; j < colsB; j++)
            C[i][j] = Cp[i][j];

    freeMaticu(Ap, n);
    freeMaticu(Bp, n);
    freeMaticu(Cp, n);
}

// testovanie

static long pamatStandard_KB(int rA, int cA, int cB) {
    long bytes = (long)(rA * cA + cA * cB + rA * cB) * (long)sizeof(double);
    return bytes / 1024;
}

static long pamatStrassen_KB(int rA, int cA, int cB) {
    int maxDim = rA > cA ? rA : cA;
    maxDim = maxDim > cB ? maxDim : cB;
    int n = dalsiaPotenciu2(maxDim);
    long bytes = 3L * n * n * (long)sizeof(double);
    return bytes / 1024;
}

void spustiTestovanie(void) {
    struct { int r, c1, c2; const char *label; } cases[] = {
        {  50,  50,  50,  "50x50x50"      },
        { 200, 200, 200,  "200x200x200"   },
        { 500, 500, 500,  "500x500x500"   },
        {1000,1000,1000,  "1000x1000x1000"},
        { 120,  80, 160,  "120x80x160"    },
        { 350, 200, 300,  "350x200x300"   },
    };
    int numCases = (int)(sizeof(cases) / sizeof(cases[0]));

    printf("\n");
    printf("%-16s  %-10s  %12s  %12s\n", "Rozmer", "Sposob", "Cas (s)", "Pamat (KB)");
    printf("%-16s  %-10s  %12s  %12s\n", "----------------", "----------", "------------", "------------");

    srand(42);

    for (int i = 0; i < numCases; i++) {
        int rA = cases[i].r, cA = cases[i].c1, cB = cases[i].c2;

        double **A = alokujMaticu(rA, cA);
        double **B = alokujMaticu(cA, cB);

        for (int ii = 0; ii < rA; ii++)
            for (int jj = 0; jj < cA; jj++)
                A[ii][jj] = (double)(rand() % 2001 - 1000) / 10.0;
        for (int ii = 0; ii < cA; ii++)
            for (int jj = 0; jj < cB; jj++)
                B[ii][jj] = (double)(rand() % 2001 - 1000) / 10.0;

        for (int method = 1; method <= 2; method++) {
            double **C = alokujMaticu(rA, cB);

            struct timespec t1, t2;
            clock_gettime(CLOCK_MONOTONIC, &t1);

            if (method == 1)
                nasobenieMatic(A, B, C, rA, cA, cB);
            else
                nasobenieMaticStrassen(A, B, C, rA, cA, cB);

            clock_gettime(CLOCK_MONOTONIC, &t2);

            double elapsed = (double)(t2.tv_sec  - t1.tv_sec)
                           + (double)(t2.tv_nsec - t1.tv_nsec) * 1e-9;

            long mem = (method == 1) ? pamatStandard_KB(rA, cA, cB)
                                     : pamatStrassen_KB(rA, cA, cB);

            const char *nazovMetody = (method == 1) ? "standard" : "strassen";
            printf("%-16s  %-10s  %12.6f  %12ld\n", cases[i].label, nazovMetody, elapsed, mem);

            freeMaticu(C, rA);
        }

        freeMaticu(A, rA);
        freeMaticu(B, cA);
        printf("\n");
    }
}
