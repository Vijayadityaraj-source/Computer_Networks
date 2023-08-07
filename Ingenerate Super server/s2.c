//rm mysocket client_socket befor running
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define ADDRESS  "mysocket"
#define CLIENT_ADDRESS "client_socket"

void fxn(int sfd){
    char buff[100];
    bzero(buff,sizeof(buff));
    printf("To Server : ");
    fgets(buff,100,stdin);
    write(sfd,buff,sizeof(buff));
    bzero(buff,sizeof(buff));
    read(sfd,buff,sizeof(buff));
    printf("From  Server: %s",buff);
}

int main(){
    int ufd=socket(AF_UNIX,SOCK_STREAM,0);

    struct sockaddr_un u,su;
    u.sun_family=AF_UNIX;
    strcpy(u.sun_path,CLIENT_ADDRESS);

    bind(ufd,(struct sockaddr*)&u,sizeof(u));

    su.sun_family=AF_UNIX;
    strcpy(su.sun_path,ADDRESS);
    
    connect(ufd,(struct sockaddr*)&su,sizeof(su));

    struct iovec iov[1];
    char ptr[2];
    iov[0].iov_base = ptr;
    iov[0].iov_len = sizeof(ptr);
    struct cmsghdr *cmptr=malloc(CMSG_LEN(sizeof(int)));

    struct msghdr m;
    m.msg_name=NULL;
    m.msg_namelen = 0;
    m.msg_iov=iov;
    m.msg_iovlen = 1;
    m.msg_control=cmptr;
    m.msg_controllen=CMSG_LEN(sizeof(int));

    sleep(5);

    recvmsg(ufd,&m,0);
    
    int sfd = *(int *)CMSG_DATA(cmptr);

    fxn(sfd);
}