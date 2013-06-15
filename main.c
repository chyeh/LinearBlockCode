#include <stdio.h>
#include <string.h>
#define N 7
#define K 4
#define NUM_OF_DATAWORDS 16
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define BER 3
#define LENGTH_OF_READ 1
#define IF_ERROR_OCCURS_WITH_PROBABILITY(BER)  if( ((rand()%100)+1)<=BER )

int SameSyndrome(unsigned char * a, unsigned char * b)
{
    int i;
    for(i=0;i<N-K;i++)
        if(a[i]!=b[i])
            return 0;
    return 1;
}
void bit_error_generator(unsigned char *buffer_data_ptr)
{
    IF_ERROR_OCCURS_WITH_PROBABILITY(BER)
    {
      buffer_data_ptr[0] = buffer_data_ptr[0]^1;  //masking
    }
}
int find_syndrome_and_correct(unsigned char syndrome_of_received[N-K], unsigned char syndrome_table[N][N-K], unsigned char *received_codeword)
{
    int i,j;
    unsigned char no_error_pattern[N-K]={0};
    if (SameSyndrome(no_error_pattern,syndrome_of_received) )
    {
        printf("no error occur!!\n");
        return 1;
    }
    for (i=0; i<N ; i++)
    {
        //printf("i_1 = %d\n", i);
        //    for (j = 0; j < 3; ++j)
        //        printf("%d ", syndrome_table[i][j]);
        //    printf(" vs ");
        //    for (j = 0; j < 3; ++j)
        //        printf("%d ", syndrome_of_received[j]);
        if (SameSyndrome(syndrome_table[i],syndrome_of_received) )
        {
        //    printf("i_2 = %d\n", i);
            printf("find a pre-compute syndrome, correct it!!!\n");
            //for (j = 0; j < 7; ++j)
            //    printf("%d ", received_codeword[j]);
            //printf("---\n" );
            received_codeword[i] = received_codeword[i]^1;
            //for (j = 0; j < 7; ++j)
            //    printf("%d ", received_codeword[j]);
            //printf("---\n" );
            return 1;
        }
    }
    return 0;
}

void main(int argc, char *argv[]) {

    /********************************************
    *           declaration variables           *
    ********************************************/
    srand(time(NULL));
    unsigned char generation_matrix[K][N];                    //usually called G
    unsigned char transpose_parity_ckeck_matrix[N][N-K];      //usually called H^T
    unsigned char error_matrix[K][N];
    unsigned char syndrome_matrix[K][N-K];
    unsigned char datawords[NUM_OF_DATAWORDS][K];
    unsigned char codewords[NUM_OF_DATAWORDS][N];
    unsigned char decoded_datawords[NUM_OF_DATAWORDS][K];
    unsigned char received[NUM_OF_DATAWORDS][N];  //codewords which may be added some errors
    unsigned char syndrome_of_received[N-K];
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
            fscanf (G_matrix_pointer, "%d", &generation_matrix[i][j]);
        }
    }
    for (i =0 ; i< K; i++) {
        for(j=0;j< N;j++) {
            printf("%d\t", generation_matrix[i][j]);
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
    /****************************************************************
    *   encoding by matrix multiplication and  output the result    *
    ****************************************************************/
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
                codewords[i][j] = codewords[i][j] ^ ( datawords[i][k] & generation_matrix[k][j] );// according to the def: * is AND op, + is XOR op
            fprintf(codewords_pointer, "%d\t", codewords[i][j]);
            printf("%d\t", codewords[i][j]);
        }
        fprintf(codewords_pointer,"\n");
        printf("\n");
    }
    printf("\n\n");
    /********************************************
    *     calculate the H transpose matrix      *
    ********************************************/
    printf(" =calculate the H transpose matrix=:\n\n");
    i=0;
    for (; i<K; i++) {          //for 1st row to K-th row, parity part: equals to the parity bits part of G
        for(j=0; j<N-K; j++) {
            transpose_parity_ckeck_matrix[i][j] = generation_matrix[i][(N-K+1)+j];
        }
    }
    for (; i<N; i++) {          //for (K+1)th row to N-th row, identity part: (N-K)x(N-K) identity matrix
        for(j=0; j<N-K; j++) {
            transpose_parity_ckeck_matrix[i][j] = 0;
            if ( j == i-(N-K+1) )
                transpose_parity_ckeck_matrix[i][j] = 1;
        }
    }  
    for (i =0 ; i< N; i++) {
        for(j=0;j< N-K;j++) {
            printf("%d\t", transpose_parity_ckeck_matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    /********************************************
    *         pre-compute the syndrome          *
    ********************************************/
    //assign error matrix as idendidy matrix, just need correction the first K bits;
    //actually, error matrix equals to identity matrix
    for (i=0; i<K; i++) {
        for(j=0; j<N; j++) {
            error_matrix[i][j] = 0;
            if ( j == i )
                error_matrix[i][j] = 1;
        }
    }
    printf(" =pre-compute syndrome =\n\n");
    for (i=0; i<K; i++) {
        for(j=0; j<N-K; j++) {
            syndrome_matrix[i][j] = 0;
            for (k = 0; k < N; k++)
                syndrome_matrix[i][j] = syndrome_matrix[i][j] ^ ( error_matrix[i][k] & transpose_parity_ckeck_matrix[k][j] );   // according to the def: * is AND op, + is XOR op
            printf("%d\t", syndrome_matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    /********************************************* 
    *   received codewards may have some errors  *
    **********************************************/
    printf(" = received codewords =\n\n");
    for (i=0; i<NUM_OF_DATAWORDS; i++) {
        for(j=0; j<N; j++) {
            received[i][j]=codewords[i][j];
            bit_error_generator(&(received[i][j]));
            printf("%d\t", received[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    /**********************************
    *   decoding received codewards   *
    **********************************/
    printf(" =syndrome of received codewords =\n\n");
    for (i=0; i < NUM_OF_DATAWORDS; i++) {
        for(j=0; j<N-K; j++) {
            syndrome_of_received[j] = 0;
            for (k = 0; k < N; k++)
                syndrome_of_received[j] = syndrome_of_received[j] ^ ( received[i][k] & transpose_parity_ckeck_matrix[k][j] );   // according to the def: * is AND op, + is XOR op
            printf("%d\t", syndrome_of_received[j]);
        }
        if ( !find_syndrome_and_correct(syndrome_of_received, syndrome_matrix, received[i]) )
            printf("cannot find a pre-compute syndrome!!!\n");
        printf("\n");
        for (j=0; j<K; j++)
            decoded_datawords[i][j]=received[i][j];
    }
    printf("\n");

    printf("decoded datawords output:\n\n");
    for (i =0 ; i<NUM_OF_DATAWORDS; i++) {
        for(j=0;j<K;j++) {
            printf("%d\t", decoded_datawords[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    return;
}