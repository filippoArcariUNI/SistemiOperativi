#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#define MAXLEVEL  10
#define SIGADDCHILD SIGUSR1
#define SIGREMOVECHILD SIGUSR2
int elected[MAXLEVEL];
int idle = 1;
int myLevel = -1;

void handler(int signo,siginfo_t *  info, void * context){
  if (signo == SIGADDCHILD)
  {
    printf("Creating child. . .\n");
    pid_t child = fork();
        if (child == 0) {
            // Child process: ignore signals and join parent's group
            struct sigaction sa;
            sa.sa_handler = SIG_IGN;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(SIGADDCHILD, &sa, NULL);
            sigaction(SIGREMOVECHILD, &sa, NULL);
            
            printf("Child created in group %d (PID: %d)\n", getpgid(0), getpid());
            pause(); // Wait for termination
            _exit(0);
        } 
  }else{
    printf("Deleting child. . .");
     // Terminate all children in group
     kill(-getpgrp(), SIGTERM);
     _exit(0); // Elected process exits after cleanup
  }
  
  
} 


void create_elected(int level) {
  pid_t pid = fork();
  if (pid == 0) {
      // Elected process: become group leader and set up handlers
      setpgid(0, 0);
      struct sigaction sa;
      sa.sa_sigaction = handler;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = SA_SIGINFO;
      sigaction(SIGADDCHILD, &sa, NULL);
      sigaction(SIGREMOVECHILD, &sa, NULL);

      printf("Elected process for level %d (PGID: %d)\n", level, getpgrp());
      while (1) pause(); // Wait for signals
  } else {
      elected[level] = pid; // Store group ID (same as PID for group leader)
  }
}
void printer(){
  for (int i = 0; i < MAXLEVEL; i++)
  {
    
  }
  
}



int main(int argc, char const *argv[])
{
  
  //Create N Child and select those like a representat of the level 
    for (int i = 0; i < MAXLEVEL; i++)
    {
      create_elected(i);
    }
    
    
    //  wait some user command
    while (idle)
    {
      char command;
      int level;
      char input[32];
      if (!fgets(input, sizeof(input), stdin)) {
        break;  // fine input (EOF)
      }

      if (sscanf(input, "%c%d", &command, &level) != 2 && sscanf(input,"%c",&command)!= 1) {
          printf("Invalid command format. Use: c<level>\n");
          continue;
      }
      switch (command)
      {
        case 'c':
        kill(-elected[level],SIGADDCHILD);
        break;
        case 'k':
        kill(-elected[level],SIGREMOVECHILD);
        break;
        case 'p':
        printer();
        break;
        case 'q':
        
        break;
        default:
          printf("Invalid command");
        break;
        
      } 
      

      
    }
    return 0;
}