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
#define MAX_SIZE 255
#define MAX_PROCESS 10
#define MIN_PROCESS 1

typedef struct Message{
    long type;
    char message[MAX_SIZE];
}Message, InternalMessage;

int firstUSR1 = 1;
char path[MAX_SIZE];
FILE * input;
int numProcess;
static int countWorker;
int* pids;
static int end = 0;
void usr1Handler(int signo, siginfo_t *info, void* context){
    if (firstUSR1)
    {
        kill(info->si_pid,SIGUSR1);
        firstUSR1 = 0;
    }else{
        printf("Exit from child with pid:%d\n",getpid());
        exit(0);
    }
    
}

void usr2Handler(int signo, siginfo_t *info, void* context){
    kill(info->si_pid,SIGUSR2);
}
void makeQueue(int signo){
      //create a queue for internal message 
      key_t key = ftok("coda",0);
      int idInternalQueue = msgget(key,IPC_CREAT | 0666);
      key_t externalKey = ftok(path,getpid());
      int idExternalQueue = msgget(key, IPC_CREAT | 0777);

      input = fopen(path,"r");
      char buffer[MAX_SIZE];
      while (fgets(buffer,sizeof(buffer),input) != NULL)
      {
          InternalMessage i;
          strcpy(i.message,buffer);
          i.type = pids[(countWorker++)%numProcess];
          msgsnd(idInternalQueue,(void*)&i,sizeof(i.message),0);
          printf("Parent send message to the private queue\n at process:%ld\n",i.type);
          sleep(1);
      }
      InternalMessage termMsg;
        strcpy(termMsg.message, "TERMINATE");
        for (int i = 0; i < numProcess; i++) {
            termMsg.type = pids[i];
            msgsnd(idInternalQueue, (void*)&termMsg, sizeof(termMsg.message), 0);
        }
      
      fclose(input);
}


int main(int argc , char * argv[]){
    // Check initial condition
    if (argc != 4)
    {
        fprintf(stderr,"Usage: %s <numProcess> </path/to/file.txt> <Pid>  \n",argv[0]);
        return 1;
    }
    numProcess = atoi(argv[1]);
    strcpy(path, argv[2]);
    int pidToSend = atoi(argv[3]);
    if (numProcess < MIN_PROCESS || numProcess > MAX_PROCESS)
    {
        fprintf(stderr,"Insert a valid number of process\n");
        return 1;
    }
    
    // if(kill(pidToSend,0) == -1)
    // {
    //     fprintf(stderr,"Inset pid of a process, no process with pid: %d\n", pidToSend);
    //     return 1;
    // }
    pids = malloc(sizeof(int)*numProcess);
    printf("Parent pid:%d\n" ,getpid());
    //create n child 
    for (int i = 0; i < numProcess; i++)
    {
        int childPid = fork();
        if (childPid == 0)
        {
            // child code
            printf("My pid is:%d\n", getpid());
            // kill(pidToSend,SIGTERM);
            sigset_t mask;
            sigfillset(&mask);
            sigdelset(&mask,SIGUSR1);
            sigdelset(&mask,SIGUSR2);
            sigdelset(&mask,SIGWINCH);
            sigprocmask(SIG_BLOCK,&mask,NULL);
            // handle SIGUSR1
            struct sigaction sa1;
            sa1.sa_flags = SA_SIGINFO;
            sa1.sa_sigaction  = usr1Handler;
            sigaction(SIGUSR1,&sa1,NULL);
            //handle SIGUSR2
            struct sigaction sa2;
            sa2.sa_flags = SA_SIGINFO;
            sa2.sa_sigaction  = usr2Handler;
            sigaction(SIGUSR2,&sa2,NULL);

            //handle Possibile message
            key_t key = ftok("coda",0);
            int id = msgget(key,0);

            while (1)
            {    InternalMessage i;
                key_t externalKey = ftok(path,getppid());
                int idExternalQueue = msgget(externalKey, IPC_CREAT | 0777);
                

                if (msgrcv(id, (void*)&i, sizeof(i.message), getpid(), 0) > 0) {
                    if (strcmp(i.message, "TERMINATE") == 0) {
                        printf("Worker %d terminating\n", getpid());
                        exit(0);
                    }
                    // Process normal message
                    printf("Worker %d received: %s", getpid(), i.message);
                    msgsnd(idExternalQueue, (void*)&i, sizeof(i.message), 0);
                }
            
            }
        }else{
            pids[i] = childPid;
            struct sigaction saQueue;
            saQueue.sa_handler = makeQueue;
            sigaction(SIGWINCH,&saQueue,NULL);
        }
        
        
    }
    
    while (wait(NULL)> 0 );
    free(pids);
    return 0;
   
}