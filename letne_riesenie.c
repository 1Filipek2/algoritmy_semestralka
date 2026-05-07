#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define EPSILON 1e-9
#define BUFFER_SIZE 256
#define STRASSEN_CUTOFF 64

int readPositiveInt(const char *prompt);
double readDouble(const char *prompt);
char readContinueChoice(void);
void readDimensions(int *rowsA, int *colsA, int *colsB);
double **allocMatrix(int rows, int cols);
void freeMatrix(double **matrix, int rows);
void readMatrix(double **matrix, int rows, int cols, char name);
void multiplyMatrices(double **A, double **B, double **C, int rowsA, int colsA, int colsB);
void computeRowSums(double **C, double *rowSums, int rowsA, int colsB);
double findMaxSum(const double *rowSums, int rowsA);
void printRowSums(const double *rowSums, int rowsA);
void printMaxRows(const double *rowSums, int rowsA, double maxSum);

// new declarations after refactor
int readModeChoice(void);
int readMethodChoice(void);
void printMatrix(double **M, int rows, int cols, char name);
void generateMatrix(double **matrix, int rows, int cols, char name);
void multiplyMatricesStrassen(double **A, double **B, double **C, int rowsA, int colsA, int colsB);
void runBenchmark(void);

static void stripNewline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

static int isBlank(const char *str) {
    while (*str != '\0') {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

// new
static void computeAndPrint(double **A, double **B, int rowsA, int colsA, int colsB, int method) {
    double **C = allocMatrix(rowsA, colsB);
    double *rowSums = (double *)malloc(rowsA * sizeof(double));

    if (rowSums == NULL) {
        printf("memory allocation error!\n");
        freeMatrix(C, rowsA);
        exit(1);
    }

    if (method == 2)
        multiplyMatricesStrassen(A, B, C, rowsA, colsA, colsB);
    else
        multiplyMatrices(A, B, C, rowsA, colsA, colsB);

    printMatrix(C, rowsA, colsB, 'C');

    computeRowSums(C, rowSums, rowsA, colsB);
    printf("\nrow sums of result matrix C:\n");
    printRowSums(rowSums, rowsA);

    double maxSum = findMaxSum(rowSums, rowsA);
    printMaxRows(rowSums, rowsA, maxSum);

    free(rowSums);
    freeMatrix(C, rowsA);
}

int main(void) {
    srand((unsigned int)time(NULL));

    char repeat = 'y';

    while (repeat == 'y' || repeat == 'Y') {
        int mode = readModeChoice();

        if (mode == 3) {
            runBenchmark();
        } else {
            int rowsA, colsA, colsB;
            readDimensions(&rowsA, &colsA, &colsB);

            int method = readMethodChoice();

            double **A = allocMatrix(rowsA, colsA);
            double **B = allocMatrix(colsA, colsB);

            if (mode == 2) {
                generateMatrix(A, rowsA, colsA, 'A');
                generateMatrix(B, colsA, colsB, 'B');
            } else {
                readMatrix(A, rowsA, colsA, 'A');
                readMatrix(B, colsA, colsB, 'B');
            }

            computeAndPrint(A, B, rowsA, colsA, colsB, method);

            freeMatrix(A, rowsA);
            freeMatrix(B, colsA);
        }

        repeat = readContinueChoice();
    }

    printf("program finished!\n");
    return 0;
}

int readPositiveInt(const char *prompt) {
    char buffer[BUFFER_SIZE];
    char *endptr;
    long value;

    while (1) {
        printf("%s ", prompt);
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("input read error!\n");
            exit(1);
        }

        stripNewline(buffer);

        if (buffer[0] == '\0' || isBlank(buffer)) {
            printf("invalid input, enter a positive integer.\n");
            continue;
        }

        value = strtol(buffer, &endptr, 10);

        while (isspace((unsigned char)*endptr)) endptr++;

        if (*endptr != '\0' || value <= 0) {
            printf("invalid input, enter a positive integer.\n");
            continue;
        }

        return (int)value;
    }
}

double readDouble(const char *prompt) {
    char buffer[BUFFER_SIZE];
    char *endptr;
    double value;

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("input read error!\n");
            exit(1);
        }

        stripNewline(buffer);

        if (buffer[0] == '\0' || isBlank(buffer)) {
            printf("invalid input, enter a real number.\n");
            continue;
        }

        value = strtod(buffer, &endptr);

        while (isspace((unsigned char)*endptr)) endptr++;

        if (*endptr != '\0') {
            printf("invalid input, enter a real number.\n");
            continue;
        }

        return value;
    }
}

char readContinueChoice(void) {
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\ndo you want to continue? (y/n): ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("input read error!\n");
            exit(1);
        }

        stripNewline(buffer);

        if (buffer[0] == 'y' || buffer[0] == 'Y' || buffer[0] == 'n' || buffer[0] == 'N')
            return buffer[0];

        printf("invalid choice, enter 'y' or 'n'.\n");
    }
}

void readDimensions(int *rowsA, int *colsA, int *colsB) {
    *rowsA = readPositiveInt("number of rows of matrix A:");
    *colsA = readPositiveInt("number of columns of matrix A:");
    *colsB = readPositiveInt("number of columns of matrix B:");
}

double **allocMatrix(int rows, int cols) {
    double **matrix = (double **)malloc(rows * sizeof(double *));
    if (matrix == NULL) exit(1);

    for (int i = 0; i < rows; i++) {
        matrix[i] = (double *)malloc(cols * sizeof(double));
        if (matrix[i] == NULL) exit(1);
    }
    return matrix;
}

void freeMatrix(double **matrix, int rows) {
    for (int i = 0; i < rows; i++) free(matrix[i]);
    free(matrix);
}

void readMatrix(double **matrix, int rows, int cols, char name) {
    char prompt[BUFFER_SIZE];
    printf("\nenter elements of matrix %c (%d x %d):\n", name, rows, cols);

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            snprintf(prompt, sizeof(prompt), "  matrix %c[%d][%d] = ", name, i, j);
            matrix[i][j] = readDouble(prompt);
        }
}

void multiplyMatrices(double **A, double **B, double **C, int rowsA, int colsA, int colsB) {
    for (int i = 0; i < rowsA; i++)
        for (int j = 0; j < colsB; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < colsA; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
}

void computeRowSums(double **C, double *rowSums, int rowsA, int colsB) {
    for (int i = 0; i < rowsA; i++) {
        rowSums[i] = 0.0;
        for (int j = 0; j < colsB; j++)
            rowSums[i] += C[i][j];
    }
}

double findMaxSum(const double *rowSums, int rowsA) {
    double maxSum = rowSums[0];
    for (int i = 1; i < rowsA; i++)
        if (rowSums[i] > maxSum) maxSum = rowSums[i];
    return maxSum;
}

void printRowSums(const double *rowSums, int rowsA) {
    for (int i = 0; i < rowsA; i++)
        printf("  row %d sum = %.2f\n", i + 1, rowSums[i]);
}

void printMaxRows(const double *rowSums, int rowsA, double maxSum) {
    printf("\nrow(s) with maximum sum (%.2f):\n", maxSum);
    for (int i = 0; i < rowsA; i++)
        if (fabs(rowSums[i] - maxSum) < EPSILON)
            printf("  -> row %d\n", i + 1);
}

// new functions after refactor

void printMatrix(double **M, int rows, int cols, char name) {
    printf("\nresult matrix %c (%d x %d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++)
            printf(" %8.2f", M[i][j]);
        printf("  ]\n");
    }
}

void generateMatrix(double **matrix, int rows, int cols, char name) {
    printf("\nmatrix %c (%d x %d): values are randomly generated\n", name, rows, cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            matrix[i][j] = (double)(rand() % 20001 - 10000) / 100.0;
}

int readModeChoice(void) {
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("=== Matrix Multiplication ===\n");
        printf("  1 - Manual       (user provides dimensions and values)\n");
        printf("  2 - Auto         (user provides dimensions, values are generated)\n");
        printf("  3 - Benchmark    (automated time and memory test)\n");
        printf("choice: ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("input read error!\n");
            exit(1);
        }

        stripNewline(buffer);

        if (buffer[0] == '1' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 1;
        if (buffer[0] == '2' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 2;
        if (buffer[0] == '3' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 3;

        printf("invalid choice, enter 1, 2 or 3.\n\n");
    }
}

int readMethodChoice(void) {
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\nselect multiplication method:\n");
        printf("  1 - standard O(n^3)     [suitable for small matrices]\n");
        printf("  2 - Strassen O(n^2.807) [more efficient for large matrices]\n");
        printf("choice: ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("input read error!\n");
            exit(1);
        }

        stripNewline(buffer);

        if (buffer[0] == '1' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 1;
        if (buffer[0] == '2' && (buffer[1] == '\0' || isspace((unsigned char)buffer[1]))) return 2;

        printf("invalid choice, enter 1 or 2.\n");
    }
}

// Strassen algorithm implementation

static int nextPowerOf2(int n) {
    int p = 1;
    while (p < n) p *= 2;
    return p;
}

static void addMatrices(double **A, double **B, double **C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] + B[i][j];
}

static void subtractMatrices(double **A, double **B, double **C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] - B[i][j];
}

static void strassenRecursive(double **A, double **B, double **C, int n) {
    if (n <= STRASSEN_CUTOFF) {
        multiplyMatrices(A, B, C, n, n, n);
        return;
    }

    int h = n / 2;

    double **A11 = allocMatrix(h, h), **A12 = allocMatrix(h, h);
    double **A21 = allocMatrix(h, h), **A22 = allocMatrix(h, h);
    double **B11 = allocMatrix(h, h), **B12 = allocMatrix(h, h);
    double **B21 = allocMatrix(h, h), **B22 = allocMatrix(h, h);
    double **M1  = allocMatrix(h, h), **M2  = allocMatrix(h, h);
    double **M3  = allocMatrix(h, h), **M4  = allocMatrix(h, h);
    double **M5  = allocMatrix(h, h), **M6  = allocMatrix(h, h);
    double **M7  = allocMatrix(h, h);
    double **T1  = allocMatrix(h, h), **T2  = allocMatrix(h, h);

    for (int i = 0; i < h; i++)
        for (int j = 0; j < h; j++) {
            A11[i][j] = A[i][j];     A12[i][j] = A[i][j+h];
            A21[i][j] = A[i+h][j];   A22[i][j] = A[i+h][j+h];
            B11[i][j] = B[i][j];     B12[i][j] = B[i][j+h];
            B21[i][j] = B[i+h][j];   B22[i][j] = B[i+h][j+h];
        }

    addMatrices(A11, A22, T1, h); addMatrices(B11, B22, T2, h); strassenRecursive(T1, T2, M1, h); // M1
    addMatrices(A21, A22, T1, h);                                strassenRecursive(T1, B11, M2, h); // M2
    subtractMatrices(B12, B22, T1, h);                           strassenRecursive(A11, T1, M3, h); // M3
    subtractMatrices(B21, B11, T1, h);                           strassenRecursive(A22, T1, M4, h); // M4
    addMatrices(A11, A12, T1, h);                                strassenRecursive(T1, B22, M5, h); // M5
    subtractMatrices(A21, A11, T1, h); addMatrices(B11, B12, T2, h); strassenRecursive(T1, T2, M6, h); // M6
    subtractMatrices(A12, A22, T1, h); addMatrices(B21, B22, T2, h); strassenRecursive(T1, T2, M7, h); // M7

    for (int i = 0; i < h; i++)
        for (int j = 0; j < h; j++) {
            C[i][j]     = M1[i][j] + M4[i][j] - M5[i][j] + M7[i][j]; // C11
            C[i][j+h]   = M3[i][j] + M5[i][j];                         // C12
            C[i+h][j]   = M2[i][j] + M4[i][j];                         // C21
            C[i+h][j+h] = M1[i][j] - M2[i][j] + M3[i][j] + M6[i][j]; // C22
        }

    freeMatrix(A11,h); freeMatrix(A12,h); freeMatrix(A21,h); freeMatrix(A22,h);
    freeMatrix(B11,h); freeMatrix(B12,h); freeMatrix(B21,h); freeMatrix(B22,h);
    freeMatrix(M1,h);  freeMatrix(M2,h);  freeMatrix(M3,h);  freeMatrix(M4,h);
    freeMatrix(M5,h);  freeMatrix(M6,h);  freeMatrix(M7,h);
    freeMatrix(T1,h);  freeMatrix(T2,h);
}

void multiplyMatricesStrassen(double **A, double **B, double **C, int rowsA, int colsA, int colsB) {
    int maxDim = rowsA > colsA ? rowsA : colsA;
    maxDim = maxDim > colsB ? maxDim : colsB;
    int n = nextPowerOf2(maxDim);

    double **Ap = allocMatrix(n, n);
    double **Bp = allocMatrix(n, n);
    double **Cp = allocMatrix(n, n);

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

    strassenRecursive(Ap, Bp, Cp, n);

    for (int i = 0; i < rowsA; i++)
        for (int j = 0; j < colsB; j++)
            C[i][j] = Cp[i][j];

    freeMatrix(Ap, n);
    freeMatrix(Bp, n);
    freeMatrix(Cp, n);
}

// benchmarking

static long memoryStandardKB(int rA, int cA, int cB) {
    long bytes = (long)(rA * cA + cA * cB + rA * cB) * (long)sizeof(double);
    return bytes / 1024;
}

static long memoryStrassenKB(int rA, int cA, int cB) {
    int maxDim = rA > cA ? rA : cA;
    maxDim = maxDim > cB ? maxDim : cB;
    int n = nextPowerOf2(maxDim);
    long bytes = 3L * n * n * (long)sizeof(double);
    return bytes / 1024;
}

void runBenchmark(void) {
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
    printf("%-16s  %-10s  %12s  %12s\n", "Size", "Method", "Time (s)", "Memory (KB)");
    printf("%-16s  %-10s  %12s  %12s\n", "----------------", "----------", "------------", "------------");

    srand(42);

    for (int i = 0; i < numCases; i++) {
        int rA = cases[i].r, cA = cases[i].c1, cB = cases[i].c2;

        double **A = allocMatrix(rA, cA);
        double **B = allocMatrix(cA, cB);

        for (int ii = 0; ii < rA; ii++)
            for (int jj = 0; jj < cA; jj++)
                A[ii][jj] = (double)(rand() % 2001 - 1000) / 10.0;
        for (int ii = 0; ii < cA; ii++)
            for (int jj = 0; jj < cB; jj++)
                B[ii][jj] = (double)(rand() % 2001 - 1000) / 10.0;

        for (int method = 1; method <= 2; method++) {
            double **C = allocMatrix(rA, cB);

            struct timespec t1, t2;
            clock_gettime(CLOCK_MONOTONIC, &t1);

            if (method == 1)
                multiplyMatrices(A, B, C, rA, cA, cB);
            else
                multiplyMatricesStrassen(A, B, C, rA, cA, cB);

            clock_gettime(CLOCK_MONOTONIC, &t2);

            double elapsed = (double)(t2.tv_sec  - t1.tv_sec)
                           + (double)(t2.tv_nsec - t1.tv_nsec) * 1e-9;

            long mem = (method == 1) ? memoryStandardKB(rA, cA, cB)
                                     : memoryStrassenKB(rA, cA, cB);

            const char *methodName = (method == 1) ? "standard" : "strassen";
            printf("%-16s  %-10s  %12.6f  %12ld\n", cases[i].label, methodName, elapsed, mem);

            freeMatrix(C, rA);
        }

        freeMatrix(A, rA);
        freeMatrix(B, cA);
        printf("\n");
    }
}
