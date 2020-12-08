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
    int out=open("text.txt",O_RDWR,0);
    if (in<0 || out<0){
        perror("Can't open file");
        return 1;
    }
    char buf[1024];
    char *p;
    int i=0,kol=0,a=0,n,nw;
    while ((n=read(in,buf,1024))!=0){
        p=buf;
        while (n){
            nw=write(out,p,n);
            p=p+nw;
            n=n-nw;
        }
    }
    ftruncate(out,a);
    close(in); 
    close(out);
    return 0;                
}
