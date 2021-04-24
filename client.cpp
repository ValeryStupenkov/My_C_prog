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
#include <vector>
#include <fstream>

using namespace std;

#define BUF 2048
#define DEFPORT 1234

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
    SocketAddress(){
        short port;
        const char *ip="127.0.0.1";
        memset(&_sockaddr,0,sizeof(_sockaddr));
        _sockaddr.sin_family=AF_INET;
        _sockaddr.sin_port=htons(port);
        _sockaddr.sin_addr.s_addr=inet_addr(ip);   
    }
    SocketAddress(const char* IP_addr, short port){
        memset(&_sockaddr,0,sizeof(_sockaddr));
        _sockaddr.sin_family=AF_INET;
        _sockaddr.sin_port=htons(port);
        _sockaddr.sin_addr.s_addr=inet_addr(IP_addr);      
    }
    struct sockaddr* GetAddr() const { return (sockaddr*)&_sockaddr;}
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
        int status=bind(Getsd(),(struct sockaddr *) ipaddr.GetAddr(), ipaddr.GetAddrlen());  
        if (status<0)
            throw Error("Bind problem");   
    }
    int Accept(SocketAddress &Claddr){
        socklen_t len=Claddr.GetAddrlen();
        int csd=accept(Getsd(), (struct sockaddr*) Claddr.GetAddr(), &len);
        if (csd==-1) 
            throw Error("In accept");
        return csd;
    }  
};

class ClientSocket : public ConnectedSocket{
public:
    ClientSocket() : ConnectedSocket() {}
    void Connect(const SocketAddress& serveraddr){
        int status=connect(Getsd(),(struct sockaddr*)serveraddr.GetAddr(), serveraddr.GetAddrlen());
        if (status!=0){
            cout<<strerror(errno)<<endl;
            throw Error("Connect error");
        }
        else 
            cout<<"Connected Succesfully"<<endl;
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

class HttpClient{
    ClientSocket client;
    SocketAddress servaddr;;
    int _exit;
public:
    HttpClient(int port) : client(), servaddr("127.0.0.1",port),_exit(0) {}
    void ClientConnection(){
        try {client.Connect(servaddr);}
        catch (Error err){
            cerr<<"Client Error: "<<err.GetErr()<<endl;
            exit(1);
        };
        cout<<"Client connected to server\n";
        string request;
        string response;
        int counter=0;
        ifstream file("in.txt");
            if ((counter==0) && file){
                getline(file,request);
                counter++;
            }
            else{
                getline(cin,request);
            }
            string req=request;
            char hostname[30];
            gethostname(hostname,30);
            string hn=hostname;
            hn+=":1234";
            HttpHeader host("Host",hn);
            request+="\n"+host.GetHeader()+"\n";
            //HttpHeader Referer();
            HttpHeader user_agent("User-agent","client.cpp");
            request+=user_agent.GetHeader()+"\n";
            client.Write(request);
            client.Read(response);
            cout<<"Response:"<<response<<endl;
            if (req=="Disconnect"){
                sleep(1);
                cout<<"Client Disconnected"<<endl;
                _exit=1;
            }
            else if (req=="Close"){
                sleep(1);
                cout<<"Server closed; Client Disconnected"<<endl;
                _exit=1;
            }
            request.clear();
            req.clear();
            cout<<"Client disconnected"<<endl;
    }
};

int main()
{
    try{
    HttpClient client(DEFPORT);
    client.ClientConnection();
    }
    catch (Error err){
        cerr<<"Error: "<<err.GetErr()<<endl;
        return 1;
    }
    return 0;
}
