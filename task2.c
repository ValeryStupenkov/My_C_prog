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
    int fd1=open(argv[1], O_RDONLY,0);
    int fd2=open(argv[1], O_WRONLY,0);
    if (fd1<0||fd2<0){
        perror("No file");
        exit(1);   
    }   
    char c;
    char *p;
    int i=0,kol=0,a=0,str=0,n=0,end=0,size=0,endfile;
    while ((n=read(fd1,&c,1))>0){ 
        if (c=='\n'){
            i++;
            if (kol==2){
                lseek(fd1, end, SEEK_SET);
                a=end;
			    while(read(fd1, &c, 1) && a!=i){
				    write(fd2, &c, 1);
				    a++;
                    size++;
			    }
            a=0;
			lseek(fd1, i, SEEK_SET);    
            }
            kol=0;
            str=0;
            end=i;
        }
        else if (c!='\n'){
            if (isdigit(c)){
                kol++;
            }
            i++;    
        }   
    }   
    close(fd1);
    ftruncate(fd2,size);
    close(fd2);
    return 0;                
}
