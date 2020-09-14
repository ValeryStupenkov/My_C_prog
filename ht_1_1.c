#include <stdio.h>
#include <math.h>
#define N 1024
#define YES 1
#define NO 0
/*Функция для обмена значениями элементов*/
void swap(int *px,int *py)
{ 
   int t=*px;
   *px=*py;
   *py=t;
}

int main(void)
{
    int pos[N];
    int c,n,i,j,sign;
    for(i=0;i<=N;i++){
       pos[i]=0;
    }
    i=0;
    sign=NO;
    printf("Введите последовательность чисел(между числами пробелы,после ввода нажмите enter) ->");
    while ((c=getchar())!='\n'&&i<=N){       /*Цикл ввода чисел*/
        if (c>='0'&&c<='9'){                 
           if (sign==NO)
              pos[i]=pos[i]*10+c-'0';        /*положительное число*/
           else 
              pos[i]=pos[i]*10-c+'0';        /*отрицательное число*/
        }      
        else if (c=='-')                     /*отслеживание знака -*/
           sign=YES;
        else if (c==' '){                    /*переход к слудющему числу*/
           i++;
           sign=NO;
        }
    }
    n=i+1;
    for(i=0;i<n-1;i++){      /*Сортировка элементов по возрастанию*/
       for(j=0;j<n-i-1;j++){
          if (pos[j]>pos[j+1])
            swap(&pos[j],&pos[j+1]);
       }
    }
    printf("Последовательность, упорядоченная по возрастанию ->");
    for(i=0;i<n;i++){        /*Печать упорядоченной последовательности*/
        printf("%d ",pos[i]); 
    }
    return 0;
}
