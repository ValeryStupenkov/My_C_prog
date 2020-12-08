#include<stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    int pid,status;
    int fd[2];
    pipe(fd);
    for (int i=1;i<=2;i++){
        if ((pid=fork())==0){
            dup2(fd[1],1);
            close(fd[0]);
            close(fd[1]);
            execlp(argv[i],argv[i],NULL);
            perror("Execlp error");
            return 1;
        }
        else {
            waitpid(pid,&status,0);    
        }
    }
    if ((pid=fork())==0){
        dup2(fd[0],0);
        close(fd[0]);
        close(fd[1]);
        execlp(argv[3],argv[3],NULL);
        perror("Execlp error");
        return 1;    
    } 
    else {
        waitpid(pid,&status,0);    
    }
    if (WIFEXITED(status)){
        if ((pid=fork())==0){
            dup2(fd[0],0);
            close(fd[0]);
            close(fd[1]);
            execlp(argv[3],argv[3],NULL);
            perror("Execlp error");
            return 1;    
        } 
        else {
            waitpid(pid,&status,0);    
        }    
    }
    return 0;                
}
