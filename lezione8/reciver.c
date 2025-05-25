#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define MAX_MESSAGE 50

struct Message
{
    long mtype;
    char mPayload[MAX_MESSAGE];

}message ;

int main(int argc, char const *argv[])
{

    
    key_t keyId  = ftok("/tmp/keyFile.txt",1);
    if (keyId == -1)
    {
        printf("Error in the generation of the key in reciver\n");
    }
    int idQueue = msgget(keyId,0666);
   
    // get the message with highest priority, we assume that the max priority is 10
    for (int i = 10; i >= 0; i--)
    {
        ssize_t recivedBytes = msgrcv(idQueue,&message, sizeof(message),i,IPC_NOWAIT);
        

        if (recivedBytes > 0)
        {
            printf("Recived Message:\n \t Priority: %d | \t Message: %s\n",(int)message.mtype, (char*) message.mPayload);
        }else{
            printf("No message with priority  %d\n",i);
        }
        
    }
    printf("All message Processed\n");
    return 0;
}
