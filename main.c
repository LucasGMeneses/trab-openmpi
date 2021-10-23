/* Aluno: Lucas Gomes Meneses */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef long long int int64;

/* Imprime vetor */
void printv(int64 *A, int n){
    int i;
    for (i = 0; i < n; i++){
        printf("%lld ", A[i]);
    }
    printf("\n");
}
/* compara os numeros bit a bit */
int comparetor(const void *a, const void *b){
/*     int64 aux1 = a << 42;
    int64 aux2 = b << 42; */
    if( *(int64*)a > *(int64*)b){
        return 1;
    }
    else if (*(int64*)a < *(int64*)b){
        return -1;
    }
    else{
        return 0;
    }

}

int main(int argc, char *argv[]){
    int npes, myrank;
    int n, nlocal = 0;
    int64 *v;               /* vetor da entrada */
    int64 *vcopy;         /* copia do vetor da entrada */
    int64 *vlocal;       /* vetor local de cada processador */

    clock_t start, end;

    MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
        /* processador principal */
        if (myrank == 0){
            
            /* Le o arquivo com as entradas */
            FILE *file = fopen(argv[argc-1],"r"); /* abre o arquivo */
            
            if(file != NULL){
                fscanf(file, "%d", &n); /* le o tamanho do v */
                nlocal = n / npes;
                v = malloc (n * sizeof (int64)); /* aloca memoria */
                vcopy = malloc (n * sizeof (int64)); /* aloca memoria */
                
                int i;
                for (i = 0; i < n; i++){
                    fscanf(file, "%lld", &v[i]);
                    vcopy[i] = v[i];
                }
                fclose(file);
            }
        }
        /* Distribui os dados entre todos os processadores */
        MPI_Bcast(&nlocal, 1, MPI_INT, 0, MPI_COMM_WORLD);/* envia o tamanho do vlocal para todos os processadores*/
        vlocal = malloc (nlocal * sizeof (int64));

        if (myrank == 0){
        
            /* qtd de dados q cada processador deve receber */
            int counts[npes];
            /* deslocamentos */
            int displ[npes];
            int i;
            for (i = 0; i < npes; i++){
                counts[i] = nlocal;
                displ[i] = i*nlocal;
            }

            MPI_Scatterv(v, counts, displ,MPI_INT64_T, vlocal, nlocal, MPI_INT64_T, 0, MPI_COMM_WORLD);
        }
        else{
            MPI_Scatterv(NULL, NULL, NULL, NULL, vlocal, nlocal, MPI_INT64_T, 0, MPI_COMM_WORLD);

        } 


        MPI_Barrier(MPI_COMM_WORLD);
        /* faz uma copia do vetor local em cada processador */
        int i;
        int64 vlocalcopy[nlocal]; /* copia do vetor local */
        for ( i = 0; i < nlocal; i++){
            vlocalcopy[i] = vlocal[i];
        }
        /* executando o qsort sequencial */
        if(myrank == 0){
            start = clock();
            qsort(v,n, sizeof(int64), comparetor);
            end = clock();
            /* printv(v,n); */
            printf("tempo:%lf\n",(double)(end - start)/CLOCKS_PER_SEC);
            free(v);
            free(vcopy);  
        }



    MPI_Finalize();

    free(vlocal);

    return 0;
}
