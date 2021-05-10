#include <iostream>        // номер 3 это номер 2 с дополнениями 
#include <cstdio>          // it works!
#include <string>
#include <cctype>

using namespace std;

class Parser{
protected:
    int ch;
public:
    virtual void get_char() = 0;
    virtual ~Parser(){}
    int S();
    int F();
    void A();
    int I();
    int D();
    bool Success(){ return ch==EOF;}
};

class StringParser : public Parser{
    string input;
    size_t index;
public:
    explicit StringParser(const char* str) : input(str), index(0){}
    void get_char() override{
        if (index<input.size())
            ch=(int)input[index++];
        else 
            ch=EOF;
    }
};

class StdinParser : public Parser{
public:
    StdinParser(){}
    void get_char() override{
        ch=getchar();
    }
};



int main(int argc, char **argv)
{
    Parser* pParser;
    if (argc==1)
        pParser=new StdinParser();
    else 
        pParser=new StringParser(argv[1]);
    try{
        pParser->get_char();
        int value=pParser->S();
        if (pParser->Success()){
            printf("Yes\n");
            printf("VALUE=%d\n",value);
        }
        else 
            throw "Extra char";
    }
    catch(const char *msg){
        fprintf(stderr,"%s",msg);
        printf("NO\n");
    }
    delete pParser;
    return 0;
}

int Parser::S(){
    int value=F();
    while (ch=='+' || ch=='-'){
        bool add= ch=='+';
        A();
        int value1=F();
        value= add ? value+value1 : value-value1;
    }
    return value;
}

int Parser::F(){
    int value;
    if (ch=='-'){
        get_char();
        value=F();
        return -value;
    }
    else if (ch=='('){
        get_char();
        value=S();   
        if (ch==')')
            get_char();
        return value;
    }
    else 
        return I();
}

void Parser::A(){
    if (ch=='+' || ch=='-')
        get_char();
    else 
        throw "add operator expected";
}

int Parser::I(){
    int value=D();
    while (isdigit(ch)){
        value=value*10+D();
    }
    printf("val=%d",value);
    return value;
}

int Parser::D(){
    int value=ch-'0';
    if (!isdigit(ch))
        throw "Digit expected";
    get_char();
    return value;
}


