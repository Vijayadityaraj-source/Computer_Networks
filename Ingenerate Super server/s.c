#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<netdb.h>
#define SA struct sockaddr

void clienthandle_fxn(int newsfd){
    char buff[100];
    bzero(buff,sizeof(buff));
    read(newsfd,buff,sizeof(buff));
    printf("From client : %s",buff);
    bzero(buff,sizeof(buff));
    printf("To client : ");
    fgets(buff,sizeof(buff),stdin);
    write(newsfd,buff,sizeof(buff));
}

int main(){
    int sfd=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in saddr,caddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);
    saddr.sin_port=htons(8080);
    bind(sfd, (SA*) &saddr,sizeof(saddr));
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(saddr.sin_addr), ip_str, INET_ADDRSTRLEN);
    printf("Server address- %s:%d\n",ip_str,ntohs(saddr.sin_port));

    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    listen(sfd,5);

    int len=sizeof(caddr);
    int newsfd=accept(sfd,(SA*) &caddr,(socklen_t *) &len);
    char ip_str1[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(caddr.sin_addr), ip_str1, INET_ADDRSTRLEN);
    printf("Client address- %s:%d\n",ip_str1,ntohs(caddr.sin_port));
    clienthandle_fxn(newsfd);
    close(newsfd);
    close(sfd);
}