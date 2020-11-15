#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

void directory(void)
{
    char buf[PATH_MAX];
    if (getcwd(buf,PATH_MAX)!=NULL)
        printf("%s$ ",buf);
    else 
        perror("Ошибка в getcwd");
}

int processing(char **words,int j)
{
    char *home=NULL;
    int i;
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
                perror("No such directory"); 
        }
        else{
            /*Создание нoвого процесса, вызов программы*/
            if (fork()==0){
                execvp(words[0],words);
                perror("Execvp error");
                return 1;
            }
            else 
                wait(0); 
        }            
    }
    return 0;
}

int main()
{
    char **words=NULL;
    char *w=NULL;
    char *home=NULL;
    char c;
    int prob=0,kav=0,i=0,j=0,stat,size=1;

    directory();   
    while ((c=getchar())!=EOF){
        if (c=='\n'){
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
            words=realloc(words,sizeof(char**)*(j+1));
            words[j]=NULL;  
            j++;
            stat=processing(words,j);
            for (i=0;i<j;i++){
                free(words[i]);
                words[i]=0;
            }
            i=0;
            free(words);
            words=NULL;
            j=0;
            if (stat==1)
                return 1;
            else if (stat==2)
                return 0;
            directory();
            prob=0;
        }
        else if (isspace(c) && kav==0){
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
            prob=1;
        }
        else{
            if (c=='"'){
                if (kav==0)
                    kav=1;
                else kav=0;
            }
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
