#include <stdio.h>
#include <stdlib.h>

char *tstr = NULL;
char *mstr = NULL;

int main() 
{
int size=0,tlen=0,maxl=0,i;
char c;
printf("Введите текст->\n");   
/*Считывание строк и запись в mstr самой длинной из них*/  
while ((c=getchar())!=EOF){            
       if (c=='\n'){
           if (maxl<tlen){       /*Записывание в mstr самой длинной строки*/   
               mstr=malloc(0);
               mstr=malloc(tlen);
               maxl=tlen;
               for (i=0; i<maxl; i++){
                    mstr[i]=tstr[i];
               } 
           }
       tlen=0;
       tstr=malloc(0);
       size=0;
       }
       else{
           if (tlen==size){             /*Расширение массива tstr*/
               size=2*size+1;
               tstr=realloc(tstr, size);
               if (tstr==NULL){
                   fprintf(stderr, "Ошибка:переполнение памяти");
                   exit(0);
               }
           }     
           tstr[tlen]=c;
           tlen++;
        }
}
if (maxl==0){
    printf("Введен пустой текст\n");
}
else{
    printf("Самая длинная строка->");
    if (tlen>maxl){
        for (i=0; i<tlen; i++){
             putchar(tstr[i]);
        }
    }
    else{
         for (i=0; i<maxl; i++){
              putchar(mstr[i]);
         }
    }
}
free(mstr);
free(tstr);
return 0;
}
