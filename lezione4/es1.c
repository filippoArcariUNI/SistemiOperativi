#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
void childCode(const char binario[]){
    printf("Sono il figlio: il mio Pid è: %d. Quello di mio padre è: %d\n", getpid(),getppid());
    char * args[] = {(char*) binario,NULL}; // inizializzo la lista di argomenti pasati al binario, in questo caso solo se stesso perché lo lancio senza argomenti 
    execv(binario, args); //esegue il binario e metto come secondo parametro la lista di arogmenti 
}

int main(int argc, char const *argv[])
{
    for (int i = 1; i < argc; i++) // parto da 1 perché il primo paramentro è il nome del programma 
    {
        __pid_t pid = fork();
        if (pid == -1)
        {
            printf("Errore nella fork()\n");
            return 1;
        }else if (pid == 0) // controllo che non sia il padre
        {
            childCode(argv[i]);
        }
    }
    while (wait(NULL) > 0 );
    printf("Sono il Padre, tutti i miei figli hanno terminato. Per evitare incomprensioni il mio PId è: %d\n",getpid());
    
    
    return 0;
}