//web server
#include<iostream>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<sys/stat.h>
#include<pthread.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string>
#include<cctype>
#include<sstream>
#include<fstream>
#include<signal.h>
#include<errno.h>

using namespace std;
void process(int sockfd);
void NotFound(int sockfd);
void NotImplement(int sockfd);
void SendFile(int,string);
const int MAXLINE=4096;
char buff[MAXLINE];

void wait_child(int signo)
{
	while(waitpid(0,NULL,WNOHANG)>0);
	return;
}

int main()
{
    sockaddr_in clientaddr,serveraddr;
    int listenfd,connfd;
    size_t n;
    pid_t childpid;
    socklen_t clilen;  
	char client_ip[4096];
    listenfd=socket(AF_INET,SOCK_STREAM,0);

    if(listenfd==0)
    {
        cout<<"create socket failed!"<<listenfd<<endl;
        return 0;
    }

	int opt=1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));//加入端口复用
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(40000);
    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);

    //binding the socket
    if(bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0)
    {
        cout<<"bind failed"<<endl;
        return 0;
    }
	//listen 函数的作用是指定同时有多少个客户端可以与我建立连接
    if(listen(listenfd,5)<0)
    {
        cout<<"listen failed"<<endl;
        return 0;
    }
    cout<<"listening in port:40000"<<endl;
    while(1)
    {
        clilen=sizeof(clientaddr);
		//此处clientaddr返回的是客户端的地址信息，是一个传出参数
        connfd=accept(listenfd,(struct sockaddr*)&clientaddr,&clilen);
		cout<<"client IP----------->"<<inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,client_ip,sizeof(client_ip))<<":"<<ntohs(clientaddr.sin_port)<<endl;
        childpid=fork();
		if(childpid<0)
		{
			cout<<"fork error";
			exit(1);
		}
		else if(childpid==0)
		{
			close(listenfd);
			process(connfd);
			exit(0);
		}
		else
		{
			close(connfd);
			signal(SIGCHLD,wait_child);
		}
    }

}
//process the message from client and send response
void process(int sockfd)
{
	//process request
	size_t n;
	n=recv(sockfd,buff,MAXLINE,0);
	stringstream recvMessage;
	string method;
	string url;
	string version;
	recvMessage<<buff;
	recvMessage>>method;
	recvMessage>>url;
	recvMessage>>version;
	cout<<"method"<<method<<endl;
	cout<<"url"<<url<<endl;
	cout<<"version"<<version<<endl;
	string me("GET");
	string empty("/");
	if(0==method.compare(me))
	{
		string u("/index.html");
		if(0==url.compare(u)||0==url.compare("/"))
		{
			cout<<"yeah"<<endl;
			SendFile(sockfd,url);
		}
		else
		{
			NotFound(sockfd);
		}
	}
	else
	{
		NotImplement(sockfd);
	}
}
void NotFound(int sockfd)
{

	string contentType("text/html");
	string content("<html><head></head><body>404 NOT FOUND</body></html>");
	string contentSize("to_string(content.size())");
	string head("HTTP/1.1 404 NOT FOUND");
	string contType("\r\nContent-Type:");
	string serverHead("\r\nServer:localhost");
	string ContentLength("\r\nContent-Length:");
	string date("\r\nDate");
	string Newline("\r\n");
	time_t rawtime;
	time(&rawtime);
	string message;
	message=head+contType+contentType+serverHead+ContentLength+contentSize+date+(string)ctime(&rawtime)+Newline;

	int messageLength=message.size();
	send(sockfd,message.c_str(),messageLength,0);
	send(sockfd,content.c_str(),content.size(),0);
}
void NotImplement(int sockfd)
{
	string ret;
	string head("HTTP/1.1");
	string statusCode("501 METHOD NOT IMPLEMENTED\r\n");
	string content("<html><head><title>501<title></head><body>501 METHOD NOT IMPLEMENTED!<body></html>");
	string contType("text/html");
	string contentType("\r\nContent-Type:");
	string serverHead("\r\nServer:localhost");
	ret=head+statusCode+contType+contentType+serverHead;
	send(sockfd,ret.c_str(),ret.size(),0);
	send(sockfd,content.c_str(),content.size(),0);
}

void SendFile(int sockfd,string url)
{
	if(0==url.compare("/"))
	{
		url="/index.html";
	}
	url.erase(url.begin());
	ifstream myfile(url.c_str());
	string line,content;
	if(myfile.is_open())
	{
		while(getline(myfile,line))
		{
			content+=line;
		}
	}
	//cout<<"recv msg from client:"<<endl<<buff<<endl;
	string statusCode("200 OK");
	string contentType("text/html");
//	string content("<html><head></head><body>hello cilent!</body></html>");
	string contentSize("to_string(content.size())");
	string head("\r\nHTTP/1.1");
	string contType("\r\nContent-Type:");
	string serverHead("\r\nServer:localhost");
	string ContentLength("\r\nContent-Length:");
	string date("\r\nDate");
	string Newline("\r\n");
	time_t rawtime;
	time(&rawtime);
	string message;
	message=head+statusCode+contType+contentType+serverHead+ContentLength+contentSize+date+(string)ctime(&rawtime)+Newline;

	int messageLength=message.size();
	send(sockfd,message.c_str(),messageLength,0);
	send(sockfd,content.c_str(),content.size(),0);
}

