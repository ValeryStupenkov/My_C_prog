#include<stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    int in=open("text.txt",O_RDONLY,0);
    int out=open("text.txt",O_WRONLY | O_TRUNC,0);
    if (in<0 || out<0){
        perror("Can't open file");
        return 1;
    }
    char buf[1024];
    int i=0,kol=0,a=0;
    read(in,buf,1024);
    printf("%d",buf[0]);
    while (buf[i]!=EOF){
        if (buf[i]=='\n'){
            if (kol==2){
                write(out,buf,i);
                a=a+i;
                i=0;
            }
            kol=0;        
        }
        else {
            if (isdigit(buf[i])){
                kol++;   
            } 
            i++;   
        }            
    }
    ftruncate(out,a);
    close(in); 
    close(out);
    return 0;                
}
