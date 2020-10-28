#include <stdio.h>

void inout(FILE *f1,FILE *f2,int bom)
{
    unsigned short ch,a,b;
    char c;
    int i;
    while ((i=fread(&ch,sizeof(unsigned short),1,f1))==1){
        if (bom!=1){
            a=ch&0x00ff;
            a=a<<8;
            ch=ch>>8;
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
    
}

int main(int argc, char *argv[])
{
    FILE *f1=stdin;
    FILE *f2=stdout;
    int bom=0,i;
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
            bom=0; /*0-обратный порядок,1-прямой*/
        else{
            bom=0;
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
