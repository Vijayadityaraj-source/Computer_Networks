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
#include <sys/uio.h>
#include<sys/un.h>

#define ADDRESS  "mysocket"
#define CLIENT_ADDRESS "client_socket1"

char buff[100];
void clienthandle_fxn(int newsfd){
    int fileFD = open("file1.txt", O_RDONLY);
    bzero(buff,sizeof(buff));

    int s=read(fileFD,buff,14);
    // perror("read");
    s=read(fileFD,buff,14);
    s=read(fileFD,buff,14);
    printf("%s",buff);
    write(newsfd,buff,sizeof(buff));
    // perror("write");
}

int main(){
    int ufd=socket(AF_UNIX,SOCK_STREAM,0);

    struct sockaddr_un u,su;
    u.sun_family=AF_UNIX;
    strcpy(u.sun_path,CLIENT_ADDRESS);
    u.sun_len=sizeof(u);

    bind(ufd,(struct sockaddr*)&u,sizeof(u));

    su.sun_family=AF_UNIX;
    strcpy(su.sun_path,ADDRESS);
    su.sun_len=sizeof(su);
    
    connect(ufd,(struct sockaddr*)&su,sizeof(su));
    // perror("connect");

    while(1){
        read(ufd,buff,sizeof(buff));
        printf("%s",buff);
        clienthandle_fxn(ufd);
    }
}