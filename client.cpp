//client
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
#include<arpa/inet.h>
const int MAXLINE=4096;
using namespace std;
int main(int argc,char** argv)
{
    int sockfd;
    struct sockaddr_in serveraddr;
	char address[]="127.0.0.1";
    char recvbuff[MAXLINE],sendbuff[MAXLINE];
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        cout<<"socket create failed"<<endl;
    }
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(40000);
    if(inet_pton(AF_INET,address,&serveraddr.sin_addr)==0)
    {
        cout<<"Inet_pton wrong"<<endl;
        return 0;
    }
    if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))<0)
    {
        cout<<"connect error"<<endl;
        return 0;
    }

    cout<<"send msg to server:"<<endl;
    //fgets(sendbuff,MAXLINE,stdin);
	while(1)
	{
    cin>>sendbuff;
	if(send(sockfd,sendbuff,strlen(sendbuff),0)<0)
    {
        cout<<"send msg error"<<endl;
        return 0;
   	}
	read(sockfd,recvbuff,MAXLINE);
	cout<<recvbuff<<endl;
	}
    close(sockfd);
    return 0;
}

