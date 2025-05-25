#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#define MAXCHILD 50
struct ChildPipes
{
    pid_t pid;
    int dataPipe[2];
};
struct ChildPipes children[MAXCHILD];
int myIndex = -1;

void handler(int signo,siginfo_t * info, void * context){
    if (signo == SIGUSR1)
    {
        printf("i've recived the signal, my pid is %d and my father is %d",getpid(),getppid());
        int number = rand() * 100;
        write(children[myIndex].dataPipe[1],&number,sizeof(number));
    }else if (signo == SIGUSR2)
    {
        int pid = getpid();
        write(children[myIndex].dataPipe[1],&pid,sizeof(pid));
    }
}


int main(int argc, char const *argv[])
{
    int idle = 1;
    if (argc != 2)
    {
        printf("Usage: ./a.out r/i<index>\n");
        _exit(1);
    }
    int numchild = atoi(argv[1]);
    if (numchild <= 0 && numchild > MAXCHILD)
    {
        printf("Insert a valid number of child\n");
        _exit(2);
    }
    
    printf("Father pid: %d\n", getpid());
    //Create child
    printf("Creating %d process\n",numchild);
    for (int i = 0; i < numchild; i++)
    {
        //create pipe for teh child 
        if (pipe(children[i].dataPipe) == -1)
        {
            perror("Pipe");
            _exit(3);
        }
        
        int childPid = fork();
        if (childPid != 0)
        {
            children[i].pid = childPid;
            close(children[i].dataPipe[1]); // close write side
        }else{
            myIndex = i;
            struct sigaction sa;
            sa.sa_sigaction = handler;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = SA_SIGINFO;
            sigaction(SIGUSR1,&sa,NULL);
            sigaction(SIGUSR2,&sa,NULL);
            close(children[i].dataPipe[0]); // close read side
            while (1)
            {
                pause();
            }
            
        }
        
    }
    for (int i = 0; i < numchild; i++)
    {
        printf("child: %d pid %d\n",i,children[i].pid);
    }
    

    while (idle)
    {
        printf("Next Command:\n");
        char buffer[32];
        fgets(buffer,sizeof(buffer),stdin);
        char command;
        char number[32];
        int index;
        if (sscanf(buffer,"%c%d",&command,&index))
        {
            printf("i've recived command %c and index %d\n",command,index);
            index--; //0-base notation
            switch (command)
            {
            case 'r':
                printf("Child computing random. . .\n");
                kill(children[index].pid,SIGUSR1);
                read(children[index].dataPipe[0],number,sizeof(number));
                printf("Child told me: %s\n",number);
                break;
            case 'i':
                printf("Child sending his pid. . .\n");
                kill(children[index].pid,SIGUSR1);
                read(children[index].dataPipe[0],number,sizeof(number));
                printf("Child told me: %s\n",number);
                break;
            case 'q':
                printf("Terminate\n");
                for (int i = 0; i < numchild; i++)
                {
                    kill(children[i].pid,SIGKILL);
                }
                return 0;
            break;
            
            default:
                printf("Invalid Command");
                break;
            }
            
        }
        
    }
    
    
    return 0;
}
