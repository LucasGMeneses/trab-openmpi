/* Aluno: Lucas Gomes Meneses */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef long long int int64;

/* Imprime vetor */
void printv(int64 *A, int n)
{

    int i, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    for (i = 0; i < n; i++)
    {
        printf("[p:%d %lld]", rank, A[i]);
    }
    printf("\n");
}
/* compara os numeros bit a bit */
int comparetor(const void *a, const void *b)
{

    /* pega os 22 primeiros bits*/
    int64 temp1 = *(int64 *)a >> 42;
    int64 temp2 = *(int64 *)b >> 42;

    if (temp1 > temp2)
    {
        return 1;
    }
    else if (temp1 < temp2)
    {
        return -1;
    }
    else
    {
        /* pega os 20 bits do meio */
        temp1 = *(int64 *)a << 22 >> 20;
        temp2 = *(int64 *)b << 22 >> 20;

        if (temp1 > temp2)
        {
            return 1;
        }
        else if (temp1 < temp2)
        {
            return -1;
        }

        else
        {
            /* pega os 22 ultimos bits */
            temp1 = temp1 << 20;
            temp2 = temp2 << 20;
            if (temp1 > temp2)
            {
                return 1;
            }
            else if (temp1 < temp2)
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}

/* int64 *addNum(int64 *v, int n, int64 num)
{
    if (n == 0 && v == NULL)
    {
        v = (int64 *)malloc(1 * sizeof(int64));
        *v = num;
    }
    else
    {
        v = (int64 *)realloc(v, sizeof(int64));
        v[n] = num;
    }
    return v;
} */

/* realiza a funcao compara-troca */
void compareSplit(int64 nlocal, int64 *vlocal, int64 *rvlocal, int keepsmall)
{
    int i, j, k;
    int64 wspace[nlocal];
    for (i = 0; i < nlocal; i++)
        wspace[i] = vlocal[i];

    if (keepsmall)
    {
        for (i = j = k = 0; k < nlocal; k++)
        {
            if (j == nlocal || (i < nlocal && wspace[i] < rvlocal[j]))
            {
                vlocal[k] = wspace[i++];
            }
            else
            {
                vlocal[k] = rvlocal[j++];
            }
        }
    }
    else
    {
        for (i = j = nlocal - 1, j = nlocal - 1; k >= 0; k--)
        {
            if (j == 0 || (i >= 0 && wspace[i] >= rvlocal[j]))
            {
                vlocal[k] = wspace[i--];
            }
            else
            {
                vlocal[k] = rvlocal[j--];
            }
        }
    }
}
/* implementa o algorimo oddEvenSort */
void oddEvenSort(int64 *vlocal, int64 nlocal, int myrank, int npes)
{
    int oddrank;
    int evenrank;
    int64 *vlocalrec;
    vlocalrec = malloc(nlocal * sizeof(int64));
    MPI_Status status;

    qsort(vlocal, nlocal, sizeof(int64), comparetor);

    if (myrank % 2 == 0)
    {
        oddrank = myrank - 1;
        evenrank = myrank + 1;
    }
    else
    {
        oddrank = myrank + 1;
        evenrank = myrank - 1;
    }

    if (oddrank == -1 || oddrank == npes)
        oddrank = MPI_PROC_NULL;

    if (evenrank == -1 || evenrank == npes)
        evenrank = MPI_PROC_NULL;

    int i;
    for (i = 0; i < npes - 1; i++)
    {
        if (i % 2 == 1)
        {
            MPI_Sendrecv(vlocal, nlocal, MPI_LONG_LONG_INT, oddrank, 1, vlocalrec, nlocal, MPI_LONG_LONG_INT, oddrank, 1, MPI_COMM_WORLD, &status);
        }
        else
        {
            MPI_Sendrecv(vlocal, nlocal, MPI_LONG_LONG_INT, evenrank, 1, vlocalrec, nlocal, MPI_LONG_LONG_INT, evenrank, 1, MPI_COMM_WORLD, &status);
        }
        compareSplit(nlocal, vlocal, vlocalrec, myrank < status.MPI_SOURCE);
    }
}

void qsortParallel(int64 *v, int64 *vlocal, int64 nlocal, int myrank, int npes)
{
    int ln, un = 0;
    int lnrec, unrec;
    int64 *llocal;
    int64 *ulocal;
    int64 *vrecv;
    int64 pivolt;

    MPI_Status status;

    llocal = (int64 *)malloc(nlocal * sizeof(int64));
    ulocal = (int64 *)malloc(nlocal * sizeof(int64));

    pivolt = vlocal[0];

    MPI_Bcast(&pivolt, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    int i;
    for (i = 0; i < nlocal; i++)
    {
        if (vlocal[i] < pivolt)
        {
            llocal[ln] = vlocal[i];
            ln++;
        }
        else
        {
            ulocal[un] = vlocal[i];
            un++;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (myrank == 1)
    {
        printf("pvlt: %lld\n", pivolt);
        printv(ulocal, un);
        printf("\n\n");
        printv(llocal, ln);
    }

    int temp;
    if (myrank < (npes / 2))
    {
        temp = abs((npes / 2) + myrank);

        MPI_Sendrecv(llocal, ln, MPI_LONG_LONG_INT, temp, 1, vrecv, nlocal, MPI_INT, temp, 1, MPI_COMM_WORLD, &status);
        compareSplit(nlocal, llocal, vrecv, myrank < status.MPI_SOURCE);
        free(vlocal);
        vlocal = llocal;
    }
    else
    {
        temp = abs((npes / 2) - myrank);
        MPI_Sendrecv(ulocal, un, MPI_INT, temp, 1, vrecv, nlocal, MPI_INT, temp, 1, MPI_COMM_WORLD, &status);
        compareSplit(nlocal, ulocal, vrecv, myrank < status.MPI_SOURCE);
        free(vlocal);
        vlocal = ulocal;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (myrank == 1)
    {
        printf("pvlt: %lld\n", pivolt);
        printv(vlocal, nlocal);
        printf("\n");
        printv(vrecv, nlocal);
        printf("\n\n");
    }
}

int main(int argc, char *argv[])
{
    int npes, myrank;
    int n, nlocal = 0;
    int64 *v;      /* vetor da entrada */
    int64 *result; /* vetor ordenado*/
    int64 *vlocal; /* vetor local de cada processador */

    clock_t start, end;
    double stime, etime;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npes);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    /* processador principal */
    if (myrank == 0)
    {

        /* Le o arquivo com as entradas */
        FILE *file = fopen(argv[argc - 1], "r"); /* abre o arquivo */

        if (file != NULL)
        {
            fscanf(file, "%d", &n); /* le o tamanho do v */
            nlocal = n / npes;
            v = malloc(n * sizeof(int64)); /* aloca memoria */

            int i;
            for (i = 0; i < n; i++)
            {
                fscanf(file, "%lld", &v[i]);
            }
            fclose(file);
        }
    }
    /* Distribui os dados entre todos os processadores */
    MPI_Bcast(&nlocal, 1, MPI_INT, 0, MPI_COMM_WORLD); /* envia o tamanho do vlocal para todos os processadores*/
    vlocal = malloc(nlocal * sizeof(int64));

    if (myrank == 0)
    {

        /* qtd de dados q cada processador deve receber */
        int counts[npes];
        /* deslocamentos */
        int displ[npes];
        int i;
        for (i = 0; i < npes; i++)
        {
            counts[i] = nlocal;
            displ[i] = i * nlocal;
        }

        MPI_Scatterv(v, counts, displ, MPI_LONG_LONG_INT, vlocal, nlocal, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Scatterv(NULL, NULL, NULL, NULL, vlocal, nlocal, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    /* faz uma copia do vetor local em cada processador */
    int i;
    int64 vlocalcopy[nlocal]; /* copia do vetor local */
    for (i = 0; i < nlocal; i++)
    {
        vlocalcopy[i] = vlocal[i];
    }
    /* executando o qsort sequencial *
    if (myrank == 0)
    {
        stime = MPI_Wtime();
        qsort(v, n, sizeof(int64), comparetor);
        etime = MPI_Wtime();
        printf("tempo:%lf\n", etime - stime);
        printv(v, n);
        printf("\n\n");
        free(v);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    /* executando o oddEvenSort *
    stime = MPI_Wtime();
    oddEvenSort(vlocal, nlocal, myrank, npes);
    etime = MPI_Wtime();
    printf("tempo:%lf\n", etime - stime);
    printv(vlocal, nlocal); */

    MPI_Barrier(MPI_COMM_WORLD);

    qsortParallel(v, vlocal, nlocal, myrank, npes);

    MPI_Finalize();

    /* free(vlocal); */

    return 0;
}
