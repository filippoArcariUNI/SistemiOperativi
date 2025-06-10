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
#define MAXBUFFER 255
typedef struct UserInfo{
    char username[MAXBUFFER];
    char passwrord[MAXBUFFER];
    long recivedPid;
} UserInfo;
typedef struct Message
{
    long type;
    char mPayload[MAXBUFFER];
} Message;
int lastPid;

void sigStopHandler(int signo){
    exit(1);
}

void * loginThread(void* arg){
    UserInfo* user = (UserInfo*) arg;
    int fdLogin = open("/tmp/login.fifo", O_RDONLY);
    // Creo la coda
    key_t keyQueue = ftok("/tmp/login.fifo",51);
    int id = msgget(keyQueue, IPC_CREAT);
    while (1)
    {
        char buff[MAXBUFFER];
        int dim = read(fdLogin,buff,MAXBUFFER);
        if (dim > 0)
        {
            buff[dim] = '\0';
            // Mando il messaggio sulla coda richiesta
            Message m;
            m.type = lastPid;
            strcpy(m.mPayload,buff);
            msgsnd(id,(void*)&m,strlen(m.mPayload),0);
            if (strcmp(buff,user->passwrord) == 0)
            {
                printf("NO\n");
            }else{
                printf("OK\n");
                fprintf(stderr,"%s logged in\n",user->username);
            }
            
        }
        
    
    }
    
    
}

int main(){
    printf("My pid is: %d\n",getpid());
    // apro gli streams
    FILE * credential = fopen("/tmp/credential.txt","r");
    if (credential == NULL)
    {
        perror("Error opening credential\n");
        return 1;
    }

    FILE * secret = fopen("/tmp/secret.txt","w+");
    if (secret == NULL)
    {
        perror("Error opening secret\n");
        return 1;
    }
    // copio contenuto di un file nell'altro
    UserInfo user;
    char buffer[MAXBUFFER];
    int i = 0;
    while (fscanf(credential,"%s",buffer) != EOF)
    {
        fprintf(secret,"%s\n",buffer);
        if (i== 0)
        {
            strcpy(user.username,buffer);
        }else{
            strcpy(user.passwrord,buffer);
        }
        i++;
    }
    // Rimuovo il file
    // remove("/tmp/credentials.txt");
    // termino il programma alla pressione del segnale sigstop
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigStopHandler;
    sigaction(SIGTSTP,&sa,NULL);
    // creo la FIFO authenticator 
    int authFD = open("/tmp/authenticator.fifo", O_WRONLY);
    if (authFD == -1 )
    {
        int res = mkfifo("/tmp/authenticator.fifo", 0666);
        if (res == -1)
        {
            perror("Error creating FIFO: ");
            return 1;
        }
        authFD = open("/tmp/authenticator.fifo", O_WRONLY);
        if (authFD == -1)
        {
            perror("Error creating authFD:");
        }
    }
    char pidString[6];
    sprintf(pidString,"%d",getpid());
    write(authFD,pidString,strlen(pidString));
    // Creo un nuovo thread
    pthread_t idThread;
    int res = pthread_create(&idThread,NULL,loginThread,(void*)&user);  
    if (res != 0)
    {
        perror("Error creating thread:");
    }
    // Mi metto in attesa su clients.fifo
    int fdClient = open("/tmp/clients.fifo", O_RDONLY);
    
    if (fdClient == -1)
    {
        perror("Error open fdClient:");
    }
    
    
    
    // Mi metto in ascolto sulla FIFO client
    while (1)
    {
        char buff[MAXBUFFER];
        ssize_t dim = read(fdClient,buff,MAXBUFFER);
        if (dim > 0)
        {
            buffer[dim] = '\0';
            int pidToSend;
            sscanf(buff,"%d",&pidToSend);
            user.recivedPid = pidToSend;
            lastPid = pidToSend;
            if (pidToSend != -1)
            {
                kill(pidToSend,SIGUSR1);
            }
            
        }
        
    }
    
    
    fclose(secret);
    fclose(credential);
    pthread_cancel(idThread);
    
    return 0;
}