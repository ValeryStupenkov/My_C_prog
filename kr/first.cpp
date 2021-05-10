#include <iostream>
#include <cstdio>
#include <cctype>

using namespace std;

enum state {H,B,C,S,ER};

class Parser{
protected:
    int ch;
public:
    state CS;
    Parser() : CS(H) {}
    virtual void get_char() = 0;
    virtual ~Parser() {}
    bool is_allowed() const {
        cout<<(ch==EOF)<<endl;
        cout<<(CS==S)<<endl;
        cout<<(ch==EOF&&CS==S)<<endl;
        return ((ch==EOF)&&(CS==S));
    }
    int GetCh() const {return ch;}
    void feed(char c){
        switch (CS){
            case H:
                if (c=='b')
                    CS=C;
                else if (c=='a')
                    CS=B;
                else
                    CS=ER;
                break;
            case B:
                if (c=='a')
                    CS=S;
                else if (c=='c')
                    CS=B;
                else 
                    CS=ER;
                break;
            case C:
                if (c=='a')
                    CS=B;
                else if (c=='b')
                    CS=C;
                else 
                    CS=ER;
                break;
            case S:
                if (c=='b')
                    CS=B;
                else if (c!=EOF)
                    CS=ER;
                break;
        }              
    }
};

class StringParser : public Parser{
    string input;
    size_t index;
public:
    explicit StringParser(const char* str) :Parser(), input(str), index(0){}
    void get_char() override{
        if (index<input.size())
            ch=(int)input[index++];
        else 
            ch=EOF;
    }
};

class StdinParser : public Parser{
public:
    StdinParser() : Parser() {}
    void get_char() override{
        ch=getchar();
    }
};

int main(int argc, char **argv)
{
    Parser *pParser;
    if (argc==1)
        pParser=new StdinParser();
    else
        pParser=new StringParser(argv[1]); 
        do {
            pParser->get_char();
            pParser->feed((char)pParser->GetCh());
        }
        while (pParser->GetCh()!=EOF && pParser->CS!=ER);
    if (pParser->is_allowed())
        printf("YES\n");
    else 
        printf("NO\n");
    delete pParser;
    return 0;
}
