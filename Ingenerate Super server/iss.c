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
#include <sys/un.h>
#define SA struct sockaddr_in

#define ADDRESS  "mysocket"

void fxn(int sfd){
    char buff[100];
    int sfd2[10];
    int nsfd[10];
    int i=0,j=0;
    while(1){
        bzero(buff,sizeof(buff));
        read(sfd,buff,sizeof(buff));
        printf("T: %s",buff);
        int port=8080;

        sfd2[i++]=socket(AF_INET,SOCK_STREAM,0);

        struct sockaddr_in saddr,caddr;
        bzero(&saddr, sizeof(saddr));
        saddr.sin_family=AF_INET;
        saddr.sin_addr.s_addr=htonl(INADDR_ANY);
        saddr.sin_port=htons(8090);
        bind(sfd2[i-1], (SA*) &saddr,sizeof(saddr));

        listen(sfd2[i-1],5);
        int len=sizeof(caddr);
        nsfd[j++]=accept(sfd,(SA*) &caddr,(socklen_t *) &len);

        // const int one = 1;
        // if (setsockopt(nsfd[i-1], IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        //     perror("setsockopt");
        //     exit(EXIT_FAILURE);
        // }

        //pass newsfd to s2
        int ufd=socket(AF_UNIX,SOCK_STREAM,0);

        struct sockaddr_un u;
        u.sun_family=AF_UNIX;
        strcpy(u.sun_path,ADDRESS);

        bind(ufd,(struct sockaddr*)&u,sizeof(u));
        listen(ufd,10);

        struct sockaddr_un client_addr;
        len=sizeof(client_addr);
        int nufd=accept(ufd,(struct sockaddr*)&client_addr,&len);
        
        struct iovec iov[1];
        char ptr[2] = {0, 0};
        iov[0].iov_base = ptr;
        iov[0].iov_len = 2;
        struct cmsghdr *cmptr=malloc(CMSG_LEN(sizeof(int)));
        cmptr->cmsg_len=CMSG_LEN(sizeof(int));
        cmptr->cmsg_level=SOL_SOCKET;
        cmptr->cmsg_type = SCM_RIGHTS;

        struct msghdr m;
        m.msg_name=NULL;
        m.msg_namelen = 0;
        m.msg_iov=iov;
        m.msg_iovlen = 1;
        m.msg_control=cmptr;
        m.msg_controllen=CMSG_LEN(sizeof(int));
        *(int *)CMSG_DATA(cmptr) = nsfd[i-1];

        sendmsg(nufd,&m,0);
    }
}

int main(){
    int sfd1=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in caddr;
    bzero(&caddr, sizeof(caddr));
    caddr.sin_family=AF_INET;
    caddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    caddr.sin_port=htons(9100);

    connect(sfd1,(SA*)&caddr,sizeof(caddr));
    printf("connected successfully to T!\n");
    fxn(sfd1);
    close(sfd1);
}