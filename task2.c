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
    char c;
    char *p;
    int i=0,kol=0,a=0,str=0,n=0;
    while ((n=read(fd,&c,1))!=0){
        i++;
        str++;
        if (c=='\n'){
            if (kol==2){
                i=i-str;
			    while(read(fd, &c, 1)){
				    lseek(fd, i+a, SEEK_SET);
				    write(fd, &c, 1);
				    lseek(fd, i+str+a, SEEK_SET);
				    a++;
			    }
            a=0;
			lseek(fd, i+str, SEEK_SET);    
            }
            kol=0;
            str=0;
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
