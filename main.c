#include <stdio.h>
#define N 7
#define K 4
#define NUM_OF_DATAWORDS 16

void main(int argc, char *argv[]) {

    /********************************************
    *           declaration variables           *
    ********************************************/
    int G[K][N];
    int datawords[NUM_OF_DATAWORDS][K];
    int codewords[NUM_OF_DATAWORDS][N];
    int i, j, k;
    FILE *G_matrix_pointer;
    FILE *datawords_pointer;
    FILE *codewords_pointer;
    const char *G_matrix_input = argv[1];
    const char *datawords_input = argv[2];
    /********************************************
    *           file input for matrix           *
    ********************************************/
    G_matrix_pointer = fopen (G_matrix_input,"rb");
    if (G_matrix_pointer==NULL) {
       printf("Open file error: %s\n", G_matrix_input);
       return;
    }
    printf("generation matrix input:\n\n");
    for (i=0; i<K; i++) {
        for(j=0; j<N; j++) {
            fscanf (G_matrix_pointer, "%d", &G[i][j]);
        }
    }
    for (i =0 ; i< K; i++) {
        for(j=0;j< N;j++) {
            printf("%d\t", G[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    /********************************************
    *         file input for datawords          *
    ********************************************/
    datawords_pointer = fopen (datawords_input,"rb");
    if (datawords_pointer==NULL) {
       printf("Open file error: %s\n", datawords_input);
       return;
    }
    printf("datawords input:\n\n");
    for (i=0; i<NUM_OF_DATAWORDS; i++) {
        for(j=0; j<K; j++) {
            fscanf (datawords_pointer, "%d", &datawords[i][j]);
        }
    }
    for (i =0 ; i<NUM_OF_DATAWORDS; i++) {
        for(j=0;j<K;j++) {
            printf("%d\t", datawords[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    /**************************************************************
    *     matrix multiplication and file output for codewords     *
    ***************************************************************/
    codewords_pointer = fopen ("codewords_","w");
    if (codewords_pointer==NULL) {
       printf("Open file error: %s\n", "codewords_");
       return;
    }
    printf(" =codewords calculation=\n\n");
    for (i=0; i<NUM_OF_DATAWORDS; i++) {
        for(j=0; j<N; j++) {
            codewords[i][j] = 0;
            for (k = 0; k < K; k++)
                codewords[i][j] = codewords[i][j] ^ ( datawords[i][k] & G[k][j] );// according to the def: * is AND op, + is XOR op
            fprintf(codewords_pointer, "%d\t", codewords[i][j]);
            printf("%d\t", codewords[i][j]);
        }
        fprintf(codewords_pointer,"\n");
        printf("\n");
    }

    return;
}