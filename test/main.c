#include <stdio.h>
#include <stdlib.h>

typedef long long int int64;

int64* add(int64 **v, int n, int64 num)
{
    if (n > 0)
    {
        *v = (int64 *)realloc(v, 1 * sizeof(int64));
        if (v != NULL)
        {
            *(v[n]) = num;
        }
    }
    else
    {
        *v = (int64 *)malloc(1 * sizeof(int64));
        **v = num;
    }
    
}

void printv(int64 *A, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("| %lld |", A[i]);
    }
    printf("\n");
}

int main(void)
{
    int64 *v;
    printf("0\n");
    add(&v, 0, 100);
    add(&v, 1, 10);
    add(&v, 2, 101);
    printv(v, 3);
    free(v);
    return 0;
}
