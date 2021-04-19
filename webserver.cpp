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

using namespace std;

#define BUF 2048
#define DEFPORT 1234
#define QUEUE 5

class Error{
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
    const struct sockaddr* GetAddr() const { return (sockaddr*)&_sockaddr;}
    const int GetAddrlen() const { return sizeof(_sockaddr);}
};

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
    string Read(int flag){
        char msg[BUF];
        int tmp=recv(Getsd(), msg, BUF, flag);
        if (tmp<0)
            throw Error("Recieve");
        else{
            string str(msg);
            return str;
        }
        return NULL;
    }
    void Read (string& str){
        char msg[BUF];
        int tmp=recv(Getsd(), msg, BUF, 0);
        if (tmp<0)
            throw Error("Recieve");
        else
            string str(msg);   
    }
    void Write(const string& str){
        const char *msg=(char*)malloc(str.size());
        msg=str.c_str();
        int tmp=send(Getsd(), msg, str.length(),0);
        if (tmp<0)
            throw Error("Send");
    }
    void Write(char *c, int len){
        int tmp=send(Getsd(),c,len,0);
        if (tmp<0)
            throw Error("Send");
    }
    int Shutdown(){
        int status=shutdown(Getsd(),2);
        if (status<0)
            throw Error("In shutdown");
        close(Getsd());
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
        socklen_t len=Claddr.GetAddrlen();
        int csd=accept(Getsd(), (struct sockaddr*) Claddr.GetAddr(), &len);
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

class HttpHeader{
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
        tmp+=":";
        tmp+=value;
        return tmp;
    }
};

class HttpRequest{
    string method;
    string uri_way;
    string parameters;
    string version;
    string body;
    int syntaxerr;
public:
    HttpRequest(const string& request){
        string tmp=request;
        int pos=tmp.find(" ");
        if (pos<0)
            syntaxerr=1;
        else{
            method=tmp.substr(0,pos);
            tmp.erase(0,pos+1);
        }
        pos=tmp.find(" ");
        if (pos<0)
            syntaxerr=1;
        else{
            string uri=tmp.substr(1,pos);
            int pos2=tmp.find("?");
            if (pos2!=-1){
                uri_way=uri.substr(0,pos2);
                uri.erase(0,pos2+1);
                parameters=uri;
            }   
            else{
                uri_way=uri.substr(0,uri.size()-1);
                parameters=" ";   
            }
            tmp.erase(0,pos+1);
        }
        pos=tmp.find("/");
        if (pos<0)
            syntaxerr=1;
        else 
            tmp.erase(0,pos);
        pos=tmp.find("\n");
        if (pos<0)
            syntaxerr=1;
        else{
            version=tmp.substr(0,pos);
            tmp.erase(0,pos);
        }
        body=tmp;
    } 
    friend ostream &operator<<(ostream &str, HttpRequest &r){
        str<<r.method<<" "<<"/"<<r.uri_way<<"?"<<r.parameters<<" "<<"HTTP/"<<r.version;
        return str;
    }  
    friend class HttpResponse;
};

class HttpResponse{
    string header;
    string code;
    string answer;
    string body;
public:
    HttpResponse(HttpRequest& request, ConnectedSocket &csd){
        int fd=open(request.uri_way.c_str(),O_RDONLY);
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
            struct stat result;
            if (stat(request.uri_way.c_str(),&result)==0){
                time_t t1=result.st_mtime;
                HttpHeader last_mod("Last-modified", asctime(localtime(&t1)));
                header+=last_mod.GetHeader()+"\n";
            }    
        }
        if (request.method!="GET"&&request.method!="HEAD"){
            code="501 Not Implemented";
            HttpHeader allow("Allow","GET,HEAD");
            header=allow.GetHeader()+"\n"; 
        }
        answer="HTTP/1.1";
        answer+="Code: " + code+"\n";
        time_t t=time(0);
        HttpHeader date("Date",asctime(localtime(&t)));
        header+=date.GetHeader()+"\n";
        HttpHeader server("Server", "Model Http Server Stu");
        header+=server.GetHeader()+"\n";
        char c;
        int length=0;
        if (fd>=0){
            while(read(fd,&c,0))
                length++;
            lseek(fd,0,0);
        }
        HttpHeader content_len("Content-length",to_string(length));
        header+=content_len.GetHeader()+"\n";
        int indx=request.uri_way.find('.');
        if (indx>=0){
            string extension=request.uri_way.substr(indx+1);
            HttpHeader content_type("Content-type", extension);
            header+=content_type.GetHeader()+"\n";
        }
        
        answer+=header+"\n";
        csd.Write(answer);
        
        if (fd>=0 && request.method=="GET"){
            char mas[BUF];
            int i;
            while((i=read(fd,mas,BUF))>0)
                csd.Write(mas,i);
        }
        close(fd);
    }
};

class HttpServer{
    ServerSocket server;
    SocketAddress servaddr;
    int queue;
    int exit1,exit2;
public:
    HttpServer(int port, int n) : server(), servaddr("127.0.0.1",port), queue(n), exit1(0), exit2(0){}
    void ProcessConnection(int cd, const SocketAddress& claddr){
        ConnectedSocket cs(cd);
        for(;;){
            string request;
            cs.Read(request);
            if (request=="Disconnect"){
                cerr<<"Client Disconnected"<<endl;
                exit1=1;
            }
            else if (request=="Close"){
                cerr<<"Server closed"<<endl;
                exit2=1;
            }
            else if (!request.empty()){
                HttpRequest text(request);
                cout<<text;
                HttpResponse response(text,cs);
            }
            request.clear();
            if (exit1||exit2){
                cs.Shutdown();
                break;
            }
        }
    }
    void ServerLoop(){
        try{
            server.Bind(servaddr);
            server.Listen(queue);
        }
        catch(Error err){
            cerr<<"Server Error "<<err.GetErr()<<endl;                
        } 
        for(;;){
            SocketAddress claddr;
            int cd=server.Accept(claddr);
            if (cd==-1)
                break;
            ProcessConnection(cd,claddr);
            if (exit2) 
                break;
        }
    }
}; 


int main()
{
    try{
        HttpServer server(DEFPORT,QUEUE);
        server.ServerLoop();
    }
    catch(Error err){
        cerr<<"Error: "<<err.GetErr()<<endl;
        return 1;
    };
    return 0;
}
