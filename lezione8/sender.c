#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE 50

struct Message
{
    long mtype;
    char mPayload[MAX_MESSAGE];

}Message;


int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./sender.out <priority> <text>\n");
        _exit(2);
    }
    int priority = atoi(argv[1]);
    char message[MAX_MESSAGE];
    strcpy(message,argv[2]);
    
    int file  = creat("/tmp/keyFile.txt", O_CREAT);
    key_t keyId  = ftok("/tmp/keyFile.txt",1);
    if (keyId == -1)
    {
        printf("Error in the generation of the key in sender\n");
        _exit(1);
    }
    int idQueue = msgget(keyId,0666 | IPC_CREAT);
    // strcturize the message
    struct Message m;
    strcpy(m.mPayload, message);
    m.mtype = priority;
    //Send the message
    int esito  = msgsnd(idQueue,&m,sizeof(m),0);
    if (esito == -1)
    {
        printf("Error in sending message\n");
        _exit(3);
    }else{
        printf("Message invied successfully\n");
    }

    return 0;
}
