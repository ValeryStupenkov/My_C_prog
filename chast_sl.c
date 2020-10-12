#include <stdlib.h>
#include <stdio.h> 
#include <ctype.h>
#include <string.h>

#define YES 1
#define NO 0 

struct tnode{
    char *word;
    int count;
    struct tnode *left;
    struct tnode *right;
};
int kol;

struct tnode *addtree(struct tnode *p,char *w)
{
    int cond;
    if (p==NULL){
        p=malloc(sizeof(struct tnode));
        p->word=strdup(w);
        p->count=1;
        p->left=p->right=NULL;
    }
    else if ((cond=strcmp(w,p->word))==0)
        p->count++;
    else if (cond<0)
        p->left=addtree(p->left,w);
    else 
        p->right=addtree(p->right,w);
    return p;
}

struct tnode *addnewtree(struct tnode *newp,char *w,int count)
{
    int cond;
    if (newp==NULL){
            newp=malloc(sizeof(struct tnode));
            newp->count=count;
            newp->word=strdup(w);
            free(w);
            newp->left=newp->right=NULL;
        }
     else if (count<newp->count)
            newp->left=addnewtree(newp->left,w,count);
     else if (count>newp->count)
            newp->right=addnewtree(newp->right,w,count);
     else{
            if ((cond=strcmp(w,newp->word))<0)
                newp->right=addnewtree(newp->right,w,count);
            else 
                newp->left=addnewtree(newp->left,w,count);
     } 
     return newp;       
}

struct tnode *vvod(struct tnode *tree,FILE *f)
{
    int i=0,size=1;
    char *word=NULL;
    char c;
    while ((c=fgetc(f))!=EOF){
        if (isspace(c)){
            if (i!=0){
                kol++;
                word[i]='\0';
                tree=addtree(tree,word);
                free(word);
                word=NULL;
                size=1;
                i=0;
            }
        }
        else{
            if (ispunct(c)){
                if (i!=0){
                    kol++;
                    word[i]='\0';
                    tree=addtree(tree,word);
                    free(word);
                    word=NULL;
                    size=1;
                    i=0;
                }
                kol++;
                word=realloc(word,size+1);
                word[i]=c;
                word[i+1]='\0';
                tree=addtree(tree,word);
                free(word);
                word=NULL;
                i=0;
            }  
            else{
                if ((i+1)==size){
                    size=2*size+1;
                    word=realloc(word,size);
                    if (word==NULL){
                        fprintf(stderr,"Error\n");
                        exit(1);
                    }
                }
                word[i]=c;
                i++;
            }
        }
    }
    if (word!=NULL){
        kol++;
        word[i]='\0';
        tree=addtree(tree,word);
        free(word);
    }
    return tree;
}

struct tnode *buildnewtree(struct tnode *p, struct tnode *newp)
{
    struct tnode *stack[100];
    struct tnode *med;
    int top=0,i=0;
    if (p==NULL)
        return NULL;
    stack[0]=NULL;
    while (1){
        while (p!=NULL){
            stack[++top]=p;
            p=p->left;
        }
        p=stack[top--];
        if (p!=NULL){
            newp=addnewtree(newp,p->word,p->count);
            med=p->right;
            free(p);
            p=med;
        }
        else 
            break;
    }
    return newp;
}

void printnewtree(struct tnode *newp,FILE *fl)
{
    double chast;
    if (newp!=NULL){
        printnewtree(newp->right,fl);
        chast=(double)newp->count/kol;
        fprintf(fl,"%s %4d %f\n",newp->word,newp->count,chast);
        printnewtree(newp->left,fl);
    }
}

void eliminatenew(struct tnode *p)
{
    if (p!=NULL){
        eliminatenew(p->left);
        eliminatenew(p->right);
        free(p->word);
        free(p);
    }
}

int main(int argc,char *argv[])
{   
    struct tnode *tree=NULL;
    struct tnode *new=NULL;
    int j,isout=NO,isin=NO; 
    kol=0;
    FILE *f1;
    FILE *f2;
    if (argc==3&&strcmp(argv[1],"-i")==0){
        isin=YES;
        f1=fopen(argv[2],"r");
    }
    else if (argc==3&&strcmp(argv[1],"-o")==0){
        f2=fopen(argv[2],"w");
        isout=YES;
        printf("Введите текст:\n");
    }
    else if (argc==5){
        if (strcmp(argv[3],"-o")==0){        
            f2=fopen(argv[4],"w");
            isout=YES;
        }
        if (strcmp(argv[1],"-i")==0){
            f1=fopen(argv[2],"r");
            isin=YES;
        }
    }
    else if (argc==1){
        printf("Введите текст:\n");
    }
    if (isin)
        tree=vvod(tree,f1);
    else 
        tree=vvod(tree,stdin);
    new=buildnewtree(tree,new);
    if (isout){
        fprintf(f2,"Частотный словарь:\n");
        printnewtree(new,f2);
    }
    else {
        printf("Частотный словарь:\n");
        printnewtree(new,stdout);
    }
    if (isin)
        fclose(f1);
    if (isout)
        fclose(f2);
    eliminatenew(new);
    return 0;
}
