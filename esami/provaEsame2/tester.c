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
int main(){
    printf("Pid: %d\n" ,getpid());
    while (1)
    {
        pause();
        key_t key = ftok("file.txt",0);
        int id = msgget(key,0);
        
        
    }
    
}