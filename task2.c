#include<stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    int in=open(argv[1],O_RONLY,0);
    int out=open(argv[1],O_WRONLY | O_TRUNC,0);
    if (in<0 || out<0){
        perror("Can't open file");
        return 1;
    }
    char *buf;
    int i=0;
    read(in,buf,1024);
    while (buf[i]!=EOF){
        if (buf[i]=='\n'){
            
        }
        else {
            if ()    
        }            
    }
    
                  
}
