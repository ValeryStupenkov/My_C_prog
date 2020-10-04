#include <stdlib.h>
#include <stdio.h>

#define MAX 1024

char *str1;
char *str2;

int main()
{
     int i=0,rav=1;

     FILE* f1=fopen("file1.txt","r");
     FILE* f2=fopen("file2.txt","r");
     while (!feof(f1)&&!feof(f2)){
         str1=malloc(MAX);
         str2=malloc(MAX);
         if ((fgets(str1,MAX,f1)!=NULL)&&(fgets(str2,MAX,f2)!=NULL)){
         while ((str1[i]==str2[i])&&(str1[i]!='\n')&&(str1[i]!='\0')){
             i++;
         }
         if (str1[i]!=str2[i]){
             puts(str1);
             rav=0;
             break;
         }
         else{
             i=0;
             free(str1);
             free(str2);
         }
         }
     }
     if (rav==1){
         printf("Все строки равны\n");
         free(str1);
         free(str2);
     }
     fclose(f1);
     fclose(f2);
     return 0;
}
         


