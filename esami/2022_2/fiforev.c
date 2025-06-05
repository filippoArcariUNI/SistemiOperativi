    
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
    char matrix[MAX_MESSAGE][2];
    int arrivedAt = MAX_MESSAGE;
    
    for (int i = 0; i < n; i++)
    {
        
        ssize_t dim = read(fdFifo,matrix[i],sizeof(matrix[i]));
        if (dim == 0)
        {
            for (int j = i-1; j >= 0; j--)
            {
                printf("%s\n",matrix[j]);
            }
            return 1;
        }
        matrix[i][1] = '\0';
        arrivedAt = i+1;
    }
    for (int i = 0; i < arrivedAt; i++)
    {
        printf("%s\n",matrix[i]);
    }
    
    
    
    return 0;
}