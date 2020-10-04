#include <stdlib.h>
#include <stdio.h>

#define MAX 1024
#define YES 1
#define NO 0

char *str1;
char *str2;

int main(int argc, char *argv[])
{
     int i=0,rav=YES;
     if (argc!=3){
        printf("После имени программы должны быть введены названия двух файлов\n");
        exit(1);
     }

     FILE *f1=fopen(argv[1],"r");
     FILE *f2=fopen(argv[2],"r");
     while (!feof(f1)&&!feof(f2)){
         str1=malloc(MAX);
         str2=malloc(MAX);
         if ((fgets(str1,MAX,f1)!=NULL)&&(fgets(str2,MAX,f2)!=NULL)){
         while ((str1[i]==str2[i])&&(str1[i]!='\n')&&(str1[i]!='\0')){
             i++;
         }
         if (str1[i]!=str2[i]){
             puts(str1);
             rav=NO;
             break;
         }
         else{
             i=0;
             free(str1);
             free(str2);
         }
         }
     }
     if (rav){
         printf("Все строки равны\n");
         free(str1);
         free(str2);
     }
     fclose(f1);
     fclose(f2);
     return 0;
}
         


