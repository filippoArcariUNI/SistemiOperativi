#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define INPUTFILE "input.txt"
#define OUTFILE "out.txt"

void streamToDescriptorsChar(){
    FILE* ptrFileSteam = fopen(INPUTFILE,"r");
    if (ptrFileSteam == NULL)
    {   
        _exit(4); //check if the file is open 
    }
    int fileDescriptors  = open(OUTFILE, O_CREAT | O_RDWR );
    if (fileDescriptors == -1)
    {
        _exit(4);
    }
    
    
    char c = fgetc(ptrFileSteam);
    while (c != EOF)
    {
        write(fileDescriptors, &c,sizeof(c));
        c= fgetc(ptrFileSteam);
    }
    

    fclose(ptrFileSteam);
    close(fileDescriptors);
}
void descriptorsToSteamChar(){
    int inDesc = open(INPUTFILE, O_RDWR  );
    FILE* outStream = fopen(OUTFILE,"a");
    if (inDesc == -1 || outStream == NULL)
    {
        _exit(4);
    }
    
    char c;
    int byteRead = read(inDesc,&c,sizeof(c));
    
   
    while (byteRead > 0)
    {
        fprintf(outStream,"%c" , c);
        byteRead = read(inDesc,&c, sizeof(c));
    }
    
    close(inDesc);
    fclose(outStream);
}
void streamToDescriptorsString(){
    FILE *inStream = fopen(INPUTFILE,"r");
    int outDescriptor = open(OUTFILE, O_RDWR | O_APPEND);
    const int lenBuffer = 40;
    if (inStream == NULL || outDescriptor == -1)
    {
        _exit(4);   
    }
    char buffer[lenBuffer];
    while (feof(inStream))
    {
        fscanf(inStream,"%s",buffer);
        write(outDescriptor,buffer,sizeof(buffer));
    }
    close(outDescriptor);
    fclose(inStream);
    
}

void descriptorsToSteamString(){
    int inDescriptor = open(INPUTFILE, O_RDWR);
    FILE *outStream = fopen(OUTFILE, "a");
    if (outStream == NULL || inDescriptor == -1)
    {
        _exit(4);   
    }
    const int lenBuffer = 50;
    char buffer[lenBuffer];
    while (read(inDescriptor,buffer,lenBuffer-1) != 0)
    {
        fprintf(outStream,"%s" , buffer);
    }
    close(inDescriptor);
    fclose(outStream);
}


int main(int argc, char const *argv[])
{
    FILE * out = fopen(OUTFILE,"w");
    if (out == NULL)
    {
        return 4;
    }
    
    fprintf(out,"%s","streamToDescriptorsChar \n");
    fflush(out);
    streamToDescriptorsChar();
    fprintf(out,"%s","descriptorsToSteamChar\n");
    fflush(out);
    descriptorsToSteamChar();
    fprintf(out,"%s","streamToDescriptorsString\n");
    fflush(out);
    streamToDescriptorsString();
    fprintf(out,"%s","descriptorsToSteamString\n");
    fflush(out);
    descriptorsToSteamString();

    return 0;
}
