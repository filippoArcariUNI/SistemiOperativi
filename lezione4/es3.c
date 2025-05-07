#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#define TMP "temp.txt"

void childWriterFunction(int tempFile, char bin[]){
    char * childArg[] = {bin,NULL};
    dup2(tempFile,STDOUT_FILENO);
    execvp(bin,childArg);
}
void childReaderFuncion(int tempFile, char bin[]){
    char * childArg[]  = {bin, TMP, NULL};
    execvp(bin,childArg);
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./a.out <bin1> <bin2>\n");
        return 1;
    }
    
    int tempFile = open(TMP, O_CREAT | O_RDWR);

    int writerPid = fork();
    if (writerPid == -1)
    {
        printf("Erorre nella creazione del figlio writer");
    }else if(writerPid == 0){
        //passo al figlio che deve scrivere il file dove scrivere ed il binario che deve eseguire
        childWriterFunction(tempFile,(char*) argv[1]);
        
    }else{
        //il padre deve aspettare che il processo figlio finisca
        if (wait(&writerPid))
        {
          
            //ho finito di fare quello che il primo processo doveva fare, inizo con il secondo processo
            int readerChild = fork();
            if (readerChild == -1)
            {
                perror("Erorre nella creazione del figlio che legge\n");
            }else if(readerChild == 0){
                // sono nel figlio
                //resetto la testina 
                lseek(tempFile,0,SEEK_CUR);
                childReaderFuncion(tempFile,(char*)argv[2]);
            }else{

            }
            


        }
        

    }
    
    
    return 0;
}
