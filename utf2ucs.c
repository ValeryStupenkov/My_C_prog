#include <stdio.h>

void inout(FILE *f1,FILE *f2,int bom)
{
    int i,prev=0,prev2=0;
    unsigned char c,j,m1=0x80,m2=0xE0,m3=0xC0;
    unsigned short ch=0,a;
    while ((i=fread(&c,sizeof(char),1,f1))==1){
        if ((j=c&0X80)==0){
            ch=c;
            if (bom==0){
                a=ch&0x00ff;
                a=a<<8;
                ch=ch>>8;
                ch=ch|a;
            }
            fwrite(&ch,sizeof(unsigned short),1,f2);
            ch=0;
        }
        else if (prev==110&&(j=c&0xc0)==0x80){
            c=c&0x3F;
            a=c;
            ch=ch|a;
            if (bom==0){
                a=ch&0x00ff;
                a=a<<8;
                ch=ch>>8;
                ch=ch|a;
            }
            fwrite(&ch,sizeof(unsigned short),1,f2);  
            ch=0;
            prev=0;
        }
        else if (prev==111&&prev2==10&&(j=c&0x80)==0x80){
            c=c&0x3F;
            a=c;
            ch=ch|a;
            if (bom==0){
                a=ch&0x00ff;
                a=a<<8;
                ch=ch>>8;
                ch=ch|a;
            }
            fwrite(&ch,sizeof(unsigned short),1,f2);
            ch=0;
            prev=0;
            prev2=0;
        }
        else{
            if ((j=c&0xe0)==0xc0){
                c=c&0x1F;
                ch=c;
                ch=ch<<6;
                prev=110;
            }  
            else if ((j=c&0xe0)==0xe0){
                c=c&0x0F;
                ch=c;
                ch=ch<<12;
                prev=111;
            }  
            else if ((j=c&0xc0)==0x80){
                c=c&0x3F;
                a=c;
                a=a<<6;
                ch=ch|a;
                prev2=10;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *f1=stdin;
    FILE *f2=stdout;
    int bom=0,i;
    unsigned short ch,a;
    char c[3];
    
    if (argc==3){
        f1=fopen(argv[1],"r");
        f2=fopen(argv[2],"w");
        if (f1==NULL||f2==NULL){
            fprintf(stderr,"Error");
            return 1;
        }
    }
    else if (argc==2){
        f1=fopen(argv[1],"r");
        if (f1==NULL){
            fprintf(stderr,"Error");
            return 1;
        }
    }
    if ((i=fread(&c,sizeof(char),3,f1))==3){
        c[0]=c[0]&0x0f;
        ch=c[0];
        ch=ch<<12;
        c[1]=c[1]&0x3f;
        a=c[1];
        a=a<<6;
        ch=ch|a;
        c[2]=c[2]&0x3f;
        a=c[2];
        ch=ch|a;
        if (ch==0xfeff){
            bom=1;
            fwrite(&ch,sizeof(unsigned short),1,f2);
        }
        else{
            ch=0xfffe;
            fwrite(&ch,sizeof(unsigned short),1,f2);
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

