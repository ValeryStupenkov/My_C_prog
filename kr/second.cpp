#include <iostream>       //it works!!!!
#include <cstdio>
#include <string>
#include <cctype>

using namespace std;

class Parser{
protected:
    int ch;
public:
    virtual void get_char() = 0;
    virtual ~Parser(){}
    void S();
    void F();
    void A();
    void I();
    void D();
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
        pParser->S();
        if (pParser->Success())
            printf("Yes\n");
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

void Parser::S(){
    F();
    while (ch=='+' || ch=='-'){
        A();
        F();
    }
}

void Parser::F(){
    if (ch=='-'){
        get_char();
        F();
    }
    else if (ch=='('){
        get_char();
        S();   
        if (ch==')')
            get_char();
    }
    else 
        I();
}

void Parser::A(){
    if (ch=='+' || ch=='-')
        get_char();
    else 
        throw "add operator expected";
}

void Parser::I(){
    D();
    while (isdigit(ch))
        D();
}

void Parser::D(){
    if (!isdigit(ch))
        throw "Digit expected";
    get_char();
}


