    
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
    
    for (int i = 0; i < n; i++)
    {
        char buff[2];
        ssize_t dim = read(fdFifo,buff,sizeof(buff));
        if (dim == 0)
        {
            return 1;
        }
        
        buff[1] = '\0';
        if (i < n-1)
        {       
            printf("%s\n",buff);
        }
        
        
    }
    
    
    return 0;
}