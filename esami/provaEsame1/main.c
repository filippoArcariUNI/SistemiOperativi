#define _POSIX_C_SOURCE 200809L
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
#define MAX_BUFFER 255
typedef struct Message
{
    long type;
    char string[MAX_BUFFER];
} Msg;

typedef struct argKill
{
    int signo;
    int pid ;
}argKill;
typedef struct argQueue
{
    char path[MAX_BUFFER];
    long category;
    char word[MAX_BUFFER];
}argQueue;
typedef struct argFifo
{
    char name[MAX_BUFFER];
    char word[MAX_BUFFER];
}argFifo;

int readNextLine = 0;


void * handleKill(void* arg ){
    argKill *a = (argKill*) arg;
    printf("sending signal %d to %d\n",a->signo, a->pid);
    kill(a->pid,a->signo);
    free(a);
    return NULL;
}
void * handleQueue(void* arg){
    argQueue* a = (argQueue*) arg;
    key_t key = ftok(a->path,1);
    int idqueue = msgget(key, IPC_CREAT | 0666);
    Msg m;
    m.type = a->category;
    strcpy(m.string,a->word);
    int res = msgsnd(idqueue,&m,sizeof(m.string),0);
    
    if (res == -1 )
    {
        fprintf(stderr,"Error with the queue\n");
    }
    free(a);
}
void *  handleFifo(void* arg){
    argFifo* a = (argFifo*) arg;
    int fd[2];
    printf("in the fifo handler: %s %s\n", a->name ,a->word);
    mkfifo(a->name, 0666);
    int fifoFd = open(a->name, O_RDWR);
    write(fifoFd,a->word,strlen(a->word));
    free(a);    
}
void handler(int signo){
    int read = 0;
}
int main(int argc, char *argv[]){
    if (argc != 2)
    {
        fprintf(stderr,"Usage: %s <path/to/file.txt>\n", argv[0]);
        return 1 ;
    }
    FILE * input = fopen(argv[1],"r");
    printf("My pid is:%d\n", getpid());
    if (input == NULL)
    {
        fprintf(stderr,"Invalid path\n");
        return 2;
    }
    char line[MAX_BUFFER];
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sigaction(SIGUSR1,&sa,NULL);
    while (1)
    {
        
        
        pause();
        
        if(fgets(line,MAX_BUFFER,input)){
            readNextLine = 0;
            char op[MAX_BUFFER];
            int dummy1,dummy2;
            sscanf(line,"%s %d  %d",op,&dummy1,&dummy2);
            printf("Current Operation: %s\n", op);
            if (strcmp(op,"kill") == 0 )  
            {
                int pid ,signo;
                sscanf(line,"%s %d %d\n",op, &pid, &signo);
                pthread_t id;
                argKill *arg = malloc(sizeof(argKill));
            arg->signo = signo;
            arg->pid = pid;
            pthread_create(&id, NULL, handleKill, (void*)arg);
            pthread_join(id,NULL);
        }else if (strcmp(op,"queue") == 0)
        {
            long category;
            char word[MAX_BUFFER];
            sscanf(line,"%s %ld %s", op, &category,word);
            pthread_t id;
            argQueue * q = malloc(sizeof(argQueue));
            q->category = category;
            strcpy(q->path, argv[1]);
            strcpy(q->word,word);
            pthread_create(&id,NULL,handleQueue,(void*)q);
            pthread_join(id,NULL);
        }else if(strcmp(op,"fifo") == 0){
            char name[MAX_BUFFER];
            char word[MAX_BUFFER];
            sscanf(line,"%s %s %s",op, name,word);
            pthread_t id;
            argFifo* f = malloc(sizeof(argFifo));
            strcpy(f->name, name);
            strcpy(f->word,word);
            printf("in main %s %s %d\n", f->name, f->word, (int) id);
            pthread_create(&id, NULL,handleFifo,(void*)f);
            pthread_join(id,NULL);
        }
        
    }   
    
}
    
    
    
    
} 