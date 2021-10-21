/* Aluno: Lucas Gomes Meneses */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Imprime vetor */
void print(long long int *A, int n){
    int i;
    for (i = 0; i < n; i++){
        printf("%lld ", A[i]);
    }
    printf("\n");
}

int comparetor(long long int a, long long int b){
    /* long long int mask1 = 0b1111111111111111111111 << 22; */
    long long int mask1 = 0xfffffffffffffff;
    long long int mask2 = mask1;
    long long int mask3;
    
    printf("\n%lld size:%d\n", mask2);

}

int main(int argc, char *argv[]){
    int npes, myrank;
    int n;
    long long int *v;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npes);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
        
        /* processador principal */
        if (myrank == 0){
            
            /* Le o arquivo com as entradas */
            FILE *file = fopen(argv[argc-1],"r"); /* abre o arquivo */
            
            if(file != NULL){
                fscanf(file, "%d", &n); /* le o tamanho do v() */
                
                v = malloc (n * sizeof (long long int)); /* aloca memoria */
                int i;
                for (i = 0; i < n; i++){
                    fscanf(file, "%lld", &v[i]);
                }
            }

            fclose(file);
            print(v, n);
            comparetor(10,10);
            free(v);
        }
        
    MPI_Finalize();
    return 0;
}
