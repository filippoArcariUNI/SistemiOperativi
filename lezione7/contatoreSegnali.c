#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>


#define MAX_MITTENTI 10
struct Counter{
    int pid;
    int usr1;
    int usr2;
};
int idle = 1;
struct Counter matrix[MAX_MITTENTI];
static int signalCounter = 0;


void handler(int signo, siginfo_t * info, void * context){
    
    if (signo == SIGTERM)
    {
        printf("switching idle...\n");
        idle = 0;
    }else{
        printf("Recived Signal from PID:%d. Numbero of signal is:%d\n", info->si_pid,signo);
        struct Counter temp;
        temp.pid = info->si_pid;
        temp.usr1 = signo==SIGUSR1 ? 0:1;
        temp.usr2 = signo==SIGUSR2 ? 0:1;
        matrix[signalCounter++] = temp;
    }
    
}
void printSummary(){
    int usr1,usr2;
    usr1 = usr2 = 0;
    for (int i = 0; i < signalCounter; i++)
    {
        printf("[%d] Recived %d from %d\n", i,matrix[i].usr1,matrix[i].pid );
        usr1 += matrix[i].usr1;
        usr2 += matrix[i].usr2;
    }
    printf("Total USR1: %d | Total USR2:%d\n", usr1, usr2);
}

int main(int argc, char const *argv[])
{
    struct sigaction sa1;
    
    sa1.sa_sigaction = handler;
    sa1.sa_flags = SA_SIGINFO;
    sigemptyset(&sa1.sa_mask);

    if(sigaction(SIGTERM,&sa1,NULL) == -1){
        perror("Error in sigaction SIGTERM");
    }
    if (sigaction(SIGUSR1, &sa1, NULL) == -1)
    {
        perror("Error in sigacrion USR1");
    }
    if (sigaction(SIGUSR2, &sa1, NULL) == -1)
    {
        perror("Error in sigacrion USR2");
    }

    
    printf("My pid is: %d\n", getpid());
    while (idle) pause();
    printf("Summary:\n");
    printSummary();

    
    return 0;
}
