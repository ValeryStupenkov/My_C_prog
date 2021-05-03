#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctime>
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>
#include <cstdio>
#include <sys/stat.h>
#include <vector>
#include <signal.h>
#include <sys/wait.h>

using namespace std;

#define BUF 2048
#define DEFPORT 1234

class Error{       // класс для обработки ошибок
    string str;
public:
    Error(string s) : str(s){}
    string GetErr(){ return str;}
};

class SocketAddress {    
protected:
    struct sockaddr_in _sockaddr;
public:
    SocketAddress(){}
    SocketAddress(const char* IP_addr, short port){
        memset(&_sockaddr,0,sizeof(_sockaddr));
        _sockaddr.sin_family=AF_INET;
        _sockaddr.sin_port=htons(port);
        _sockaddr.sin_addr.s_addr=inet_addr(IP_addr);      
    }
    struct sockaddr* GetAddr() const { return (sockaddr*)&_sockaddr;}
    socklen_t GetAddrlen() const { return sizeof(_sockaddr);}
    int GetPort() const {return _sockaddr.sin_port;}
    char* GetIP() const {return inet_ntoa(_sockaddr.sin_addr);}
};

SocketAddress claddr;  // адрес клиента

class Socket{
protected:
    int sd;
    explicit Socket(int _sd) : sd(_sd) {}
public:
    Socket(){ 
        if ((sd=socket(AF_INET,SOCK_STREAM,0)) < 0)
            throw Error("Couldn't create socket");
    }
    ~Socket(){ close(sd);}
    int Getsd() const{ return sd;}
};

class ConnectedSocket : public Socket{
public:
    ConnectedSocket() : Socket() {}
    explicit ConnectedSocket(int _sd) : Socket(_sd) {}
    void Read (string &s){
        char msg[BUF];
        memset(msg,0,BUF);
        int tmp=recv(Getsd(), msg, BUF, 0);
        if (tmp<0)
            cerr<<"No client"<<endl;
        else{
            s=msg; 
            memset(msg,0,BUF);
        }                
    }
    void Write(const string& str){
        vector<char> tmp(str.begin(),str.end());
        int status=send(Getsd(), tmp.data(), tmp.size(),0);
        if (status<0)
            throw Error("Send");
    }
    void Write(char *c, int len){
        int tmp=send(Getsd(),c,len,0);
        if (tmp<0)
            throw Error("Send");
    }
    int Shutdown(){
        int status=shutdown(Getsd(),2);
        if (status<0){
            cerr<<strerror(errno)<<endl;
            throw Error("In shutdown");
        }
        return status;
    }
};

class ServerSocket : public Socket{
public:
    ServerSocket() : Socket() {}
    explicit ServerSocket(int _sd) : Socket(_sd) {}
    void Listen(int n){
        int tmp=Getsd();
        int status=listen(tmp,n);
        if (status<0)
            throw Error("Listen");
    }  
    void Bind(const SocketAddress &ipaddr){
        int status=bind(Getsd(),(struct sockaddr *)ipaddr.GetAddr(), ipaddr.GetAddrlen());  
        if (status==-1){
            throw Error("Bind problem"); 
        }  
    }
    int Accept(SocketAddress &Claddr){
        int len=Claddr.GetAddrlen();
        int *point;
        point=&len;
        int csd=accept(Getsd(),Claddr.GetAddr(),(socklen_t*)point);
        if (csd<0) 
            throw Error("In accept");
        return csd;
    }  
};

class ClientSocket : public ConnectedSocket{
public:
    ClientSocket() : ConnectedSocket() {}
    void Connect(const SocketAddress& serveraddr){
        int status=connect(Getsd(),(struct sockaddr *) serveraddr.GetAddr(), serveraddr.GetAddrlen());
        if (status<0)
            throw Error("Connect error");
    }
};

class HttpHeader{   // класс для http заголовка
    string name;
    string value;
public:
    HttpHeader(const string& n, const string& v) : name(n), value(v) {}
    explicit HttpHeader(const string&line){
        string tmp=line;
        int pos=tmp.find(":");
        name=tmp.substr(0,pos);
        tmp.erase(0,pos+1);
        value=tmp;    
    }
    const string GetHeader() const{
        string tmp=name;
        tmp+=": ";
        tmp+=value;
        return tmp;
    }
};

class HttpRequest{  //обработка запроса http
    string full;
    string method;
    string uri_way;
    string parameters;
    string version;
    string body;
    int syntaxerr;
    int index;
    int iscgi;
    int size_of_r;
public:
    HttpRequest(const string& request) : full(request), method(), uri_way(), parameters(), version(), body(), syntaxerr(0), index(0), iscgi(0), size_of_r(0){
        string tmp=request;
        size_of_r=request.size();
        int pos=tmp.find(" ");
        if (pos<0)
            syntaxerr=1;
        else{
            method=tmp.substr(0,pos);
            tmp.erase(0,pos+1);
        }
        pos=tmp.find(" ");
        if (pos>=0){
            string uri=tmp.substr(1,pos);
            if (pos==1){
                uri="index.html";    
                uri_way=uri.substr(0,uri.size());
            }
            else{
            int pos2=tmp.find("?");
            if (pos2!=-1){
                uri_way=uri.substr(0,pos2-1);
                uri.erase(0,pos2);
                parameters=uri;
                iscgi=1;
            }   
            else{
                uri_way=uri.substr(0,uri.size()-1);
                parameters=" ";   
            }
            tmp.erase(0,pos+1);
            }
        }
        pos=tmp.find("/");
        if (pos<0)
            syntaxerr=1;
        else 
            tmp.erase(0,pos);
        pos=tmp.find("\n");
        if (pos>=0){
            version=tmp.substr(0,pos);
            tmp.erase(0,pos);
        }
        else {
            version=tmp.substr(0);
            tmp.erase(0);
        }
        body=tmp;
    } 
    friend class HttpResponse;
    friend char** CreateEnvironment(HttpRequest &request);
};

char** CreateEnvironment(HttpRequest &request){   // создание набора переменных окружения
    char **env=new char*[14];
    env[0]=new char[22];
    env[0]=(char*)"SERVER_ADDR=127.0.0.1";
    env[1]=new char[40];
    env[1]=(char*)"SERVER_NAME=Model Http Server Stupenkov";
    env[2]=new char[17];
    env[2]=(char*)"SERVER_PORT=1234";
    env[3]=new char[24];
    env[3]=(char*)"CONTENT_TYPE=text/plain";
    env[4]=new char[25];
    env[4]=(char*)"SERVER_PROTOCOL=HTTP/1.0";
    env[5]=new char[13+request.uri_way.size()];
    strcpy(env[5],"SCRIPT_NAME=");
    strcat(env[5],request.uri_way.c_str());
    env[6]=new char[14+request.parameters.size()];
    strcpy(env[6],"QUERY_STRING=");
    strcat(env[6],request.parameters.c_str());
    env[7]=new char[26];
    env[7]=(char*)"GATEWAY_INTERFACE=CGI/1.1";
    env[8]=new char[40];
    env[8]=(char*)"SERVER_SOFTWARE=Linux Mint/4.6.6 (Unix)";
    env[9]=new char[64+request.uri_way.size()];
    strcpy(env[9],"SCRIPT_FILENAME=/home/valerystupenkov/Prac/My_C_Prog2/My_C_prog");
    strcat(env[9],request.uri_way.c_str());
    env[10]=new char[62];
    env[10]=(char*)"DOCUMENT_ROOT=/home/valerystupenkov/Prac/My_C_Prog2/My_C_prog";
    int port=claddr.GetPort();
    const char *c=to_string(port).c_str();
    env[11]=new char[13+to_string(port).size()];
    strcpy(env[11],"REMOTE_PORT=");
    strcat(env[11],c);
    env[12]=new char[28];
    strcpy(env[12],"REMOTE_ADDR=");
    strcat(env[12],claddr.GetIP());
    env[13]=NULL;
    return env;
};


class HttpResponse{   //ответ сервера 
    string header;
    string code;
    string answer;
    string body;
public:
    HttpResponse(HttpRequest& request, ConnectedSocket &csd) : header(), code(), answer(), body(){
        int fd;
        if (request.iscgi==1){
            int pid;
            if ((pid=fork())>0){
                int status;
                wait(&status);
                if (WIFEXITED(status) && (WEXITSTATUS(status)==0)){
                    fd=open("temporary.txt",O_RDONLY);
                    code="200 OK";
                }
                else {
                    code="404 NOT FOUND";
                }
            }
            else if (pid==0){
                fd=open("temporary.txt",O_WRONLY|O_TRUNC|O_CREAT,0666);
                if (fd<0){
                    throw Error("Error in open");
                }
                dup2(fd,1);
                close(fd);
                char *argv[]={(char*)request.uri_way.c_str(),NULL};
                char **env=CreateEnvironment(request);
                execvpe(request.uri_way.c_str(), argv, env);
                perror("execvpe");
                exit(2);
            }
            else if (pid<0){
                perror("Fork error!!!");
                exit(1);
            }
        }
        else {
            fd=open(request.uri_way.c_str(),O_RDONLY);
            if (fd==-1){
                if (errno == EACCES)
                    code="403 Forbidden";
                else if (request.syntaxerr==1)
                    code="400 Bad Request";
                else 
                    code="404 Not Found";
            }
            else{
                code="200 OK"; 
            }
        }
        if (request.method!="GET"&&request.method!="HEAD"){
            code="501 Not Implemented";
            HttpHeader allow("Allow","GET,HEAD");
            header+=allow.GetHeader()+"\n"; 
        }
        answer="HTTP/1.0 ";
        answer+=code+"\n";
        HttpHeader server("Server", "Model Http Server Stupenkov/0.1");
        header+=server.GetHeader()+"\n";
        time_t t=time(0);
        HttpHeader date("Date",asctime(localtime(&t)));
        header+=date.GetHeader();
        
        if (fd>=0){
            char c;
            int length=0;
            while(read(fd,&c,1))
                length++;
            lseek(fd,0,0);
            HttpHeader content_len("Content-length",to_string(length));
            header+=content_len.GetHeader()+"\n";
        }
        int indx=request.uri_way.find('.');
        string extension;
        if (indx>=0){
            extension=request.uri_way.substr(indx+1);
            if (extension=="html"){
                HttpHeader content_type("Content-type", "text/html");
                header+=content_type.GetHeader()+"\n";
            }
            else if(extension=="jpg" || extension=="jpeg"){
                HttpHeader content_type("Content-type", "image/jpeg");
                header+=content_type.GetHeader()+"\n";
            }
            else {
                HttpHeader content_type("Content-type", "text/plain");
                header+=content_type.GetHeader()+"\n";
            }
        }
        if (fd>=0){
            struct stat result;
            if (stat(request.uri_way.c_str(),&result)==0){
                time_t t1=result.st_mtime;
                HttpHeader last_mod("Last-modified", asctime(localtime(&t1)));
                header+=last_mod.GetHeader();
            }  
        } 
        answer+=header+"\r\n";
        if (fd>=0 && request.method=="GET" && extension!="jpg"){
            char mas[BUF];
            int i;
            string s;
            while((i=read(fd,mas,BUF))>0){
                for (int j=0;j<i;j++)
                    s+=string(1,mas[j]);
                answer+=s;
                s.clear();
            }
            answer+="\r\n";
            csd.Write(answer);
        }
        else if (fd<0){
            answer+=code;
            answer+="\r\n";
            csd.Write(answer);
        }
        else if (extension=="jpg"){
            char mas[BUF];
            int i;
            csd.Write(answer);
            while((i=read(fd,mas,BUF))>0){
                csd.Write(mas,i);
            }
            cout<<"Image send"<<endl;
        }
        extension.clear();
        close(fd);
    }
    const string GetAnswer() const {return answer;}
};

class HttpServer{
    ServerSocket server;
    SocketAddress servaddr;
    int queue;
    int exit1,exit2;
public:
    HttpServer(int port, int n) : server(), servaddr("127.0.0.1",port), queue(n), exit1(0), exit2(0){}
    void ProcessConnection(int cd, const SocketAddress& claddr){     //обработка запроса
        ConnectedSocket cs(cd);
        string request;  
        cs.Read(request); 
        cout<<"Request: "<<request<<endl;
        int pos=request.find("\n");
        string req=request.substr(0,pos);
        if (req=="Disconnect"){
            cout<<"Client Disconnected"<<endl;
            exit1=1;
        }
        else if (req=="Close"){
            cout<<"Server closed"<<endl;
            exit2=1;
        }
        else if (!request.empty()){
            HttpRequest text(request);
            HttpResponse response(text,cs);
            cout<<response.GetAnswer();
        }
        request.clear();
        if (exit1 || exit2){
            exit1=0;
            exit(1);
        }
        cs.Shutdown();
        cout<<"Shutdown client"<<endl;
    }
    void ServerLoop(){   //установление соединения
        try{
            server.Bind(servaddr);
            server.Listen(queue);
        }
        catch(Error err){
            cerr<<"Server Error "<<err.GetErr()<<endl;
            exit(2);                
        } 
        for(;;){
            int cd;
            cout<<"Waiting for connection...\n";
            try{cd=server.Accept(claddr);}
            catch(Error err){
                cerr<<"Error: "<<err.GetErr()<<endl;
                exit(1);
            }
            if (cd==-1)
                break;
            cout<<"Client Accepted!"<<endl;
            ProcessConnection(cd,claddr);
            if (exit2) 
                break;
        }
    }
}; 


int main()
{
    try{
        HttpServer server(DEFPORT,5);
        server.ServerLoop();
    }
    catch(Error err){
        cerr<<"Error: "<<err.GetErr()<<endl;
        return 1;
    };
    return 0;
}
