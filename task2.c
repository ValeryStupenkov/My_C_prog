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
    int fd=open(argv[1], O_RDWR,0777);
    if (fd<0){
        perror("No file");
        exit(1);   
    }   
    char c;
    char *p;
    int i=0,kol=0,a=0,str=1,n=0,end=0;
    while ((n=read(fd,&c,1))!=0){
        if (c=='\n'){
            str++;
            if (kol==2){
                i=i-str;
                lseek(fd, end, SEEK_SET);
			    while(read(fd, &c, 1) && a!=i){
				    write(fd, &c, 1);
				    a++;
			    }
            a=0;
			lseek(fd, i+str, SEEK_SET);    
            }
            kol=0;
            str=0;
            end=1;
        }
        if (c!='\n'){
            if (isdigit(c)){
                kol++;
            }
            i++;    
        }
    }
    ftruncate(fd,i);
    close(fd);
    return 0;                
}
