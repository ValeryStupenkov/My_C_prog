#include <stdio.h>

void inout(FILE *f1,FILE *f2,int bom)
{
    unsigned short ch,a,b;
    unsigned char c,mid[2];
    int i;
    while ((i=fread(&mid,sizeof(char),2,f1))==2){
        if (bom==1){
            ch=mid[0];
            a=mid[1];
            a=a<<8;
            ch=ch|a;    
        }
        else{
            ch=mid[0];
            ch=ch<<8;
            a=mid[1];
            ch=ch|a;
        }
        if (ch<=127){
            c=ch;
            fwrite(&c,sizeof(char),1,f2);
        }
        else if (ch<=2047){
            b=ch;
            b=b>>6;
            c=b;
            c=c|0xc0;
            fwrite(&c,sizeof(char),1,f2);
            a=ch&0x003f;
            c=a;
            c=c|0x80;
            fwrite(&c,sizeof(char),1,f2);  
        }
        else if (ch>=2048){
            a=ch;
            a=a>>12;
            c=a;
            c=c|0xe0;
            fwrite(&c,sizeof(char),1,f2);
            a=ch&0x0fc0;
            a=a>>6;
            c=a;
            c=c|0x80;
            fwrite(&c,sizeof(char),1,f2);
            a=ch&0x003f;
            c=a;
            c=c|0x80;
            fwrite(&c,sizeof(char),1,f2);
        }
    }
    if (i==1)
        fprintf(stderr,"В файле нечётное количество байтов!\n");
}

int main(int argc, char *argv[])
{
    FILE *f1=stdin;
    FILE *f2=stdout;
    int bom=1,i;
    if (argc==3){
        f1=fopen(argv[1],"r");
        f2=fopen(argv[2],"w");
        if (f1==NULL||f2==NULL){
            fprintf(stderr,"Error\n");
            return 1;
        }
    }
    else if (argc==2){
        f1=fopen(argv[1],"r");
        if (f1==NULL){
            fprintf(stderr,"Error\n");
            return 1;
        }
    }
    unsigned short utf16ch;
    if ((i=fread(&utf16ch,sizeof (unsigned short),1,f1))==1){
        if (utf16ch==0xfeff)
            bom=1;
        else if (utf16ch==0xfffe)   
            bom=0; /*1-обратный порядок,0-прямой*/
        else{
            bom=1;
            fseek(f1,0,SEEK_SET);
        }
    }
    else 
        fseek(f1,0,SEEK_SET);
    inout(f1,f2,bom);
    fclose(f1);
    fclose(f2);
    return 0;
}
