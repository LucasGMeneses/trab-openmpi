/* Aluno: Lucas Gomes Meneses */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef long long int int64;

/* Imprime vetor */
void printv(int64 *A, int n){

    int i, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    for (i = 0; i < n; i++){
        printf("| p:%d %lld |",rank, A[i]);
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
/* realiza a funcao compara-troca */
void compareSplit(int64 nlocal, int64 *vlocal, int64 *rvlocal, int keepsmall){
    int i, j, k;
    int64 wspace[nlocal];
    for (i = 0; i < nlocal; i++)
        wspace[i] = vlocal[i];

    if (keepsmall){
        for (i = j = k = 0; k < nlocal; k++){
            if(j == nlocal || (i < nlocal && wspace[i] < rvlocal[j])){
                vlocal[k] = wspace[i++];
            }
            else {
                vlocal[k] = rvlocal[j++];
            }
        } 
    }
    else {
        for (i = j = nlocal - 1, j = nlocal - 1; k >= 0; k--){
            if(j == 0 || (i >= 0 && wspace[i] >= rvlocal[j])){
                vlocal[k] = wspace[i--];
            }
            else {
                vlocal[k] = rvlocal[j--];
            }
        } 
        
    }
    
}
/* implementa o algorimo oddEvenSort */
void oddEvenSort(int64 *vlocal, int64 nlocal, int myrank, int npes){
    int oddrank;
    int evenrank;
    int64 *vlocalrec;
    vlocalrec = malloc (nlocal * sizeof (int64));
    MPI_Status status;

    qsort(vlocal, nlocal, sizeof(int64), comparetor);
    
    if (myrank%2 == 0){
        oddrank = myrank - 1;
        evenrank = myrank + 1;
    }
    else{
        oddrank = myrank + 1;
        evenrank = myrank - 1;
    }

    if (oddrank == -1 || oddrank == npes)
        oddrank = MPI_PROC_NULL;
    
    if (evenrank == -1 || evenrank == npes)
        evenrank = MPI_PROC_NULL;
    
    int i;
    for (i = 0; i < npes-1; i++){
        if (i%2 == 1){
            MPI_Sendrecv(vlocal, nlocal, MPI_LONG_LONG_INT, oddrank, 1, vlocalrec, nlocal, MPI_LONG_LONG_INT, oddrank, 1, MPI_COMM_WORLD, &status);
        }
        else{
            MPI_Sendrecv(vlocal, nlocal, MPI_LONG_LONG_INT, evenrank, 1, vlocalrec, nlocal, MPI_LONG_LONG_INT, evenrank, 1,MPI_COMM_WORLD, &status);
        }

        compareSplit(nlocal, vlocal, vlocalrec, myrank < status.MPI_SOURCE);

    }
    
    
}
int main(int argc, char *argv[]){
    int npes, myrank;
    int n, nlocal = 0;
    int64 *v;               /* vetor da entrada */
    int64 *result;         /* vetor ordenado*/
    int64 *vlocal;       /* vetor local de cada processador */

    clock_t start, end;
    double stime, etime;

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
                /*result = malloc (n * sizeof (int64)); /* aloca memoria */
                
                int i;
                for (i = 0; i < n; i++){
                    fscanf(file, "%lld", &v[i]);
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

            MPI_Scatterv(v, counts, displ,MPI_LONG_LONG_INT, vlocal, nlocal, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
        }
        else{
            MPI_Scatterv(NULL, NULL, NULL, NULL, vlocal, nlocal, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

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
            stime = MPI_Wtime();
            qsort(v,n, sizeof(int64), comparetor);
            etime = MPI_Wtime();
            printf("tempo:%lf\n", etime - stime);
            printv(v, n);
            printf("\n\n");
            free(v);
        }

    MPI_Barrier(MPI_COMM_WORLD);
    /* executando o oddEvenSort */
    stime = MPI_Wtime();
    oddEvenSort(vlocal, nlocal, myrank, npes);
    etime = MPI_Wtime();
    printf("tempo:%lf\n",etime - stime);
    printv(vlocal, nlocal);
    
    MPI_Barrier(MPI_COMM_WORLD);



    MPI_Finalize();

    free(vlocal);

    return 0;
}
