#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <wait.h>
#include <signal.h>

int fon=0;

void directory(void)
{
    char buf[PATH_MAX];
    if (getcwd(buf,PATH_MAX)!=NULL)
        printf("\33[33m%s$\33[0m ",buf);
    else 
        perror("Ошибка в getcwd\n");
}

void ChangeInOut(char **words, int j)
{ /*перенаправляет ввод, вывод*/
    int i,flag=0;
    
    for (i=0;i<j-1;i++){
        if (flag==0){
            if (strcmp(words[i],">")==0){
                if (words[i+1]==NULL){
                    fprintf(stderr,"Ошибка в перенаправлении\n");
                    break;
                }
                int fd=open(words[i+1],O_WRONLY | O_CREAT | O_TRUNC,0666);
                if (fd<0){
                    fprintf(stderr,"Ошибка при открытии/создании файла\n");
                    break;
                } 
                dup2(fd,STDOUT_FILENO);
                close(fd);  
                flag=1;
            }
            else if (strcmp(words[i],"<")==0){
                if (words[i+1]==NULL){
                    fprintf(stderr,"Ошибка в перенаправлении\n");
                    break;
                }
                int fd=open(words[i+1], O_RDONLY,0);
                if (fd<0){
                    fprintf(stderr,"Ошибка при открытии/создании файла\n");
                    break;
                }
                dup2(fd,STDIN_FILENO);
                close(fd); 
                flag=1;        
            } 
            else if (strcmp(words[i],">>")==0){
                if (words[i+1]==NULL){
                    fprintf(stderr,"Ошибка в перенаправлении\n");
                    break;
                }
                int fd=open(words[i+1], O_WRONLY | O_CREAT | O_APPEND,0666);  
                if (fd<0){
                    fprintf(stderr,"Ошибка при открытии/создании файла\n");
                    break;
                }
                dup2(fd,STDOUT_FILENO);
                close(fd);
                flag=1;          
            } 
        }
        if (flag==1){
            free(words[i+1]);
            words[i+1]=NULL;
            free(words[i]);
            words[i]=NULL;
            flag++;
        }
        else if (flag==2)
            flag=0;
    } 
    return;   
}

void fonproc()
{
    int status,pid;
    pid=waitpid(-1,&status,WNOHANG);
    if (pid>0 && WIFEXITED(status)!=0){
        if (WIFSIGNALED(status)){
            printf("Фоновый процесс %d завершился по сигналу %d\n",pid,WTERMSIG(status));
        }
        else 
            printf("Фоновый процесс %d завершился с кодом %d\n",pid,WEXITSTATUS(status));
    }
}
int processing(char **words,int j)
{
    char *home=NULL;
    int i,a,pid,status;
    if (words!=NULL){
        if (strcmp(words[0],"exit")==0)
            return 2;
        else if (strcmp(words[0],"cd")==0){
            /*обработка команды cd*/
            if (words[1]==NULL){
                home=strdup(getenv("HOME"));
                chdir(home);
                free(home);
            }  
            else if (chdir(words[1])==-1)
                perror("No such directory\n"); 
        }
        else{
            /*Создание нoвого процесса, вызов программы*/
            if ((pid=fork())==0){
                /*перед execvp если есть > or >> or < то перенаправление вывода, ввода */
                if (!fon){
                    signal(SIGINT,SIG_DFL);
                }
                ChangeInOut(words,j);
                execvp(words[0],words);
                perror("Execvp error ");
                return 1;
            }
            else if (pid<0){
                perror("Fork's mistake ");
                return 3;
            }
            else{ 
                if (fon==0)
                    waitpid(pid,&status,0);
                else{
                    printf("Запущен фоновый процесс %d\n",pid); 
                    fonproc();
                    fon=0;
                } 
            }
        }            
    }
    return 0;
}

int PipeN(char **words,int j)
{
    int fd[2],i=0,a=0,b=0,pid,status;
    char **mas=NULL;
    
    ChangeInOut(words,j);
    while (words[i]!=NULL){
        if (strcmp(words[i],"|")==0){
            mas=realloc(mas,sizeof(char**)*(a+1));
            mas[a]=NULL;
            if (pipe(fd)<0){
                perror("Pipe's error\n");
                exit(1);
            }      
            if ((pid=fork())==0){
                if ((i+1)!=j)
                    dup2(fd[1],1);
                close(fd[0]);
                close(fd[1]);
                execvp(mas[0],mas);
                perror("Execvp error\n");
                for (b=0;b<a;b++){
                    free(mas[b]);
                    mas[b]=NULL;
                } 
                free(mas);
                return 4;    
            }
            else if (pid<0){
                perror("Fork error\n");
                return 5;
            }  
            dup2(fd[0],0);
            close(fd[0]);
            close(fd[1]);
            for (b=0;b<a;b++){
                free(mas[b]);
                mas[b]=NULL;
            } 
            free(mas);
            mas=NULL;
            a=0; 
            i++;  
        }
        else{
            mas=realloc(mas,sizeof(char**)*(a+1));
            mas[a]=strdup(words[i]);
            a++;  
            i++;  
        }     
    }
    if (mas==NULL){
        fprintf(stderr,"Error!!!\n");
    }
    else{ 
        if ((pid=fork())==0){
            mas=realloc(mas,sizeof(char**)*(a+1));
            mas[a]=NULL;
            execvp(mas[0],mas);
            perror("Execvp error");
            for (b=0;b<a;b++){
                free(mas[b]);
                mas[b]=NULL;
            } 
            free(mas);
            return 4;    
        }
        else if (pid<0){
            perror("Fork error\n");
            return 5;
        } 
        else 
            waitpid(pid,&status,0); 
    }
    while (wait(NULL)!=-1);
    for (b=0;b<a;b++){
        free(mas[b]);
        mas[b]=NULL;
    } 
    free(mas);
    return 0;        
}

int main()
{
    char **words=NULL;
    char *w=NULL;
    char *home=NULL;
    char c;
    int prob=0,kav=0,i=0,j=0,stat,size=1,vvod=0,a,conv=0,pid,status;
    
    signal(SIGINT,SIG_IGN);
    directory();   
    while ((c=getchar())!=EOF){
        if (c=='\n' && kav==0){ /*запись последнего слова в массив, вызов обработки строки*/
            if (w!=NULL){
                w[i]='\0';
                words=realloc(words,sizeof(char**)*(j+1));
                if (words==NULL){
                    fprintf(stderr,"Error\n");
                    exit(1);
                }
                words[j]=strdup(w);
                j++;
                free(w);
                w=NULL;
                i=0;
                size=1;
            }
            if (words!=NULL){
                words=realloc(words,sizeof(char**)*(j+1));
                words[j]=NULL;  
                j++;            
                for (a=0;a<j-1;a++){
                    if ((strcmp(words[a],"&")==0)&&(a!=j-2)){
                        fprintf(stderr,"Ошибка! & стоит не в конце строки!");
                        fon=0;
                        break;
                    } 
                    else if (strcmp(words[a],"&")==0){
                        fon=1;
                        free(words[a]);
                        words[a]=NULL;
                        j--;
                    }       
                }
                for (a=0;a<j-1;a++){
                    if (strcmp(words[a],"|")==0)
                        conv=1;    
                }
                if (conv==1){
                    if ((pid=fork())==0){
                        if (!fon){
                            signal(SIGINT,SIG_DFL);
                        }
                        PipeN(words,j);
                        for (i=0;i<j;i++){
                            free(words[i]);
                            words[i]=NULL;
                        }
                        free(words);
                        words=NULL;
                        i=0;
                        return 0;
                    }
                    else if (pid>0){
                        if  (fon==0) 
                            waitpid(pid,&status,0);
                        else{
                            printf("Запущен фоновый процесс %d\n",pid);
                            fonproc();
                            fon=0;
                        }
                    }
                    else {
                        perror("Fork's error ");
                        return 1;
                    }
                    conv=0;
                    stat=0;
                }
                else
                    stat=processing(words,j);
                for (i=0;i<j;i++){
                    free(words[i]);
                    words[i]=NULL;
                }
                i=0;
                free(words);
                words=NULL;
                j=0;
                if (stat==1)
                    return 1;
                else if (stat==2)
                    return 0;
                else if (stat==3)
                    return 2;
            }
            /*здесь проверка, завершился ли фоновый процесс, если да то обработка*/
            fonproc();
            directory();
            prob=0;
            vvod=0;
        }
        else if ((isspace(c)||(c=='>')||(c=='<')||(c=='|')||(c=='&')) && kav==0){
            if (prob==0){
                /*добавление слова в массив*/
                if (w!=NULL){
                    words=realloc(words,sizeof(char**)*(j+1));
                    if (words==NULL){
                        fprintf(stderr,"Error\n");
                        exit(1);
                    }
                    w[i]='\0';
                    words[j]=strdup(w);
                    j++;
                    free(w);
                    w=NULL;
                    i=0;
                    size=1;
                }   
            }
            if ((c=='<')||(c=='|')||(c=='&')){
            /*обработка и добавление < или |*/
                if ((i+1)==size){
                    size=size*2+1;
                    w=realloc(w,size);
                    if (w==NULL){
                        fprintf(stderr,"Error\n");
                        exit(1);
                    }
                }
                w[i]=c;
                i++;
                words=realloc(words,sizeof(char**)*(j+1));
                if (words==NULL){
                    fprintf(stderr,"Error\n");
                    exit(1);
                }
                w[i]='\0';
                i++;
                words[j]=strdup(w);
                j++;
                free(w);
                w=NULL;
                i=0;
                size=1;    
            }
            else if (c=='>'){ 
                    /*Обработка и добавление >*/
                    if (vvod==0){
                        if ((i+1)==size){
                            size=size*2+1;
                            w=realloc(w,size);
                            if (w==NULL){
                                fprintf(stderr,"Error\n");
                                exit(1);
                            }
                        }
                        w[i]=c;
                        i++;
                        vvod=1;
                    }
                    else{
                        if ((i+1)==size){
                            size=size*2+1;
                            w=realloc(w,size);
                            if (w==NULL){
                                fprintf(stderr,"Error\n");
                                exit(1);
                            }
                        }
                        w[i]=c;
                        i++;
                        vvod=0;
                        words=realloc(words,sizeof(char**)*(j+1));
                        if (words==NULL){
                            fprintf(stderr,"Error\n");
                            exit(1);
                        }
                        w[i]='\0';
                        i++;
                        words[j]=strdup(w);
                        j++;
                        free(w);
                        w=NULL;
                        i=0;
                        size=1;    
                    }
            }
            prob=1;
        }
        else{
            if (c=='"'){
                if (kav==0)
                    kav=1;
                else kav=0;
            }
            if (vvod==1){
                words=realloc(words,sizeof(char**)*(j+1));
                if (words==NULL){
                    fprintf(stderr,"Error\n");
                    exit(1);
                }
                if ((i+1)==size){
                    size=size*2+1;
                    w=realloc(w,size);
                    if (w==NULL){
                        fprintf(stderr,"Error\n");
                        exit(1);
                    }
                }
                w[i]='\0';
                i++;
                words[j]=strdup(w);
                j++;
                free(w);
                w=NULL;
                i=0;
                size=1; 
                vvod=0;  
            }
            /*добавление символов в слово*/    
            if ((i+1)==size){
                size=size*2+1;
                w=realloc(w,size);
                if (w==NULL){
                    fprintf(stderr,"Error\n");
                    exit(1);
                }
            }
            w[i]=c;
            i++;
            prob=0;     
        }
    }
    /*Если конец файла, то надо дозаписать в массив слов и выполнить последнюю команду*/
    if (w!=NULL){
        w[i]='\0';
        words=realloc(words,sizeof(char**)*(j+1));
        if (words==NULL){
            fprintf(stderr,"Error\n");
            exit(1);
        }
        words[j]=strdup(w);
        j++;
        free(w);
        w=NULL;
        i=0;
        size=1;
    }
    if (words!=NULL){
        words=realloc(words,sizeof(char**)*(j+1));
        words[j]=NULL;  
        j++;
        stat=processing(words,j);
        for (i=0;i<j;i++){
            free(words[i]);
            words[i]=NULL;
        }
        free(words);
        words=NULL;
        j=0;
        if (stat==1)
            return 1;
        else if (stat==2)
            return 0;
    }
    return 0;
}
