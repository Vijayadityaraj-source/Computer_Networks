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

#define ipaddr "192.168.87.82"

char buff[100];
void clienthandle_fxn(int newsfd){
    sprintf(buff,"%s.txt",buff);
    int fileFD = open(buff, O_RDONLY);
    bzero(buff,sizeof(buff));

    int s=read(fileFD,buff,14);
    // perror("read");
    printf("%s",buff);
    write(newsfd,buff,sizeof(buff));
    // perror("write");
}

int main(){
    int sfd=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in saddr,caddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_addr.s_addr=inet_addr(ipaddr);
    saddr.sin_port=htons(8080);
    bind(sfd, (SA*) &saddr,sizeof(saddr));
    // perror("bind");
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(saddr.sin_addr), ip_str, INET_ADDRSTRLEN);
    printf("Server address- %s:%d\n\n",ip_str,ntohs(saddr.sin_port));

    listen(sfd,5);
    // perror("listen");

    while(1){
        int len=sizeof(caddr);
        int newsfd=accept(sfd,(SA*) &caddr,(socklen_t *) &len);
        // perror("accept");
        char ip_str1[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(caddr.sin_addr), ip_str1, INET_ADDRSTRLEN);
        printf("Client address- %s:%d\n",ip_str1,ntohs(caddr.sin_port));

        int x=fork();
        if(x>0){close(newsfd);}
        if(x==0){
            close(sfd);
            read(newsfd,buff,sizeof(buff));
            clienthandle_fxn(newsfd);
            close(newsfd);
            exit(0);
        }
    }
}