#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>

int main(int argc, char *argv[]){
    FILE *file;
    int n[] = {100,100000, 1000000, 10000000};
    int k = 0;
    for (int i = 1; i < argc; i++){
        file = fopen(argv[i], "w");
        fprintf(file, "%d\n", n[k]);
        srand(time(NULL));
        
        for (int j = 0; j < n[k]; j++){
            fprintf(file, "%lld ", (long long int) rand() % 500);
        }
        
        k++;
        printf("%s\n", argv[i]);
    }

    fclose(file);
    return 0;
}