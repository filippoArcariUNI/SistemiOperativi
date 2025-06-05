    
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#define MAX_MESSAGE 10
#define MAXBUFFER 255
typedef struct Message{
    long type;
    char m;
} Message;
void printResult(char * matrix,int n){
    int * numbers = malloc(sizeof(int)*n);
    int countNumbers = 0;
    int total = 0;
    for (int i = n-1; i >=0; i--)
    {
        if (matrix[i] > 'a' && matrix[i] < 'z')
        {
            printf("%c\n",matrix[i]);
        }else if (matrix[i] >= '0' && matrix[i] <= '9')
        {
            numbers[countNumbers] =  (int)matrix[i]- '0';
            total += numbers[countNumbers];
            countNumbers++;

        }
    }

    for (int i = 0; i < countNumbers; i++)
    {
        if (i == countNumbers-1)
        {
            printf("%d=",numbers[i]);
        }else{
            printf("%d+",numbers[i]);
        }
        
    }

    printf("%d\n",total);

    
    
}
int main(int argc, char * argv[]){
    if (argc != 3)
    {
        fprintf(stderr,"Invalid number of arg\n");
        exit(1);
    }
    int n = atoi(argv[2]);
    int fdFifo = open(argv[1],O_RDONLY);
    if (fdFifo == -1)
    {
        fprintf(stderr,"Invalid path of Fifo\n");
        exit(1);
    }
    char matrix[MAX_MESSAGE];
    int arrivedAt = MAX_MESSAGE;
    
    for (int i = 0; i < n; i++)
    {
        
        ssize_t dim = read(fdFifo,(void*)&matrix[i],sizeof(matrix[i]));
        if (dim == 0)
        {
            printResult(matrix,arrivedAt);
            return 1;
        }
        arrivedAt = i+1;
    }

    printResult(matrix,arrivedAt);

    
    
    
    
    return 0;
}