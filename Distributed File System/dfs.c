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
#define ipaddr "192.168.87.82"

#define SA struct sockaddr
#define n 2 //number of files

char files[n][10],ips[n][5][30],port[n][5][10];
int size[n],num_of_local[n];
char file[100];

void clienthandle_fxn(int newsfd,int locals4fd,int locals5fd){
    char buff[1000];
    bzero(buff,sizeof(buff));

    read(newsfd,buff,sizeof(buff));
    printf("Client : %s",buff);
    sprintf(file,"%s",buff);
    int i=0;
    for(i=0;i<n;i++){
        if(strcmp(files[i],buff)==0) break;
    }
    bzero(buff,sizeof(buff));

    if(i>=n){
        strcpy(buff,"ERROR : No such file in the file system!");  
        write(newsfd,buff,sizeof(buff)); 
    }
    else{
        char buff1[100];
        sprintf(buff1,"%d%d",size[i],num_of_local[i]);
        write(newsfd,buff1,sizeof(buff1));
        strcat(buff,buff1);
        int k=num_of_local[i];
        for(int j=0;j<size[i],k>0;j++){
            if(strcmp(ips[i][j],"locals4")==0){
                k--;
                write(locals4fd,file,sizeof(file));
                bzero(buff,sizeof(buff));
                int s=read(locals4fd,buff,sizeof(buff));
                // perror("read");
                printf("%s",buff);
                write(newsfd,buff,sizeof(buff));
            }
            if(strcmp(ips[i][j],"locals5")==0){
                k--;
                write(locals5fd,file,sizeof(file));
                bzero(buff,sizeof(buff));
                int s=read(locals5fd,buff,sizeof(buff));
                // perror("read");
                printf("%s",buff);
                write(newsfd,buff,sizeof(buff));
            }
        }
    }
    // else {
        // char buff1[100];
        // sprintf(buff1,"%d%d",size[i],num_of_local[i]); 
        // strcat(buff,buff1);
        // for(int j=0;j<size[i];j++){
        //     if(j==0){
        //         bzero(buff1,sizeof(buff1)); 
        //         sprintf(buff1,"%s",ips[i][j]); 
        //         strcat(buff,buff1);
        //     }
        //     else{
        //         bzero(buff1,sizeof(buff1)); 
        //         sprintf(buff1,"|%s",ips[i][j]); 
        //         strcat(buff,buff1);
        //     }
        // }
        // write(newsfd,buff1,sizeof(buff1));   

        // for(int j=0;j<size[i];j++){
        //     bzero(buff,sizeof(buff));
        //     sprintf(buff,"%s",ips[i][j]);
        //     printf("%s",buff);
        //     write(newsfd,buff,sizeof(buff));    
        //     bzero(buff,sizeof(buff));
        //     sprintf(buff,"%s",port[i][j]);
        //     write(newsfd,buff,sizeof(buff));    
        // }
        // }
}

int main(){
    setbuf(stdin,NULL);
    setbuf(stdout,NULL);

    int ufd=socket(AF_UNIX,SOCK_STREAM,0);

    struct sockaddr_un u;
    u.sun_family=AF_UNIX;
    strcpy(u.sun_path,ADDRESS);
    u.sun_len=sizeof(u);

    bind(ufd,(struct sockaddr*)&u,sizeof(u));
    listen(ufd,10);

    struct sockaddr_un client_addr;
    int len=sizeof(client_addr);
    int nufd1=accept(ufd,(struct sockaddr*)&client_addr,&len);
    int nufd2=accept(ufd,(struct sockaddr*)&client_addr,&len);

    strcpy(files[0],"file1\n");
    size[0]=3;
    num_of_local[0]=1;
    strcpy(ips[0][0],ipaddr);//s1
    strcpy(port[0][0],"8080");//s1
    strcpy(ips[0][1],ipaddr);//s2
    strcpy(port[0][1],"8085");//s2
    strcpy(ips[0][2],"locals4");//locals4
    strcpy(port[0][2],"locals4");//locals4
    
    strcpy(files[1],"file2\n");
    size[1]=2;
    num_of_local[1]=1;
    strcpy(ips[1][0],ipaddr);//s3
    strcpy(port[1][0],"9090");//s3
    strcpy(ips[1][1],"locals5");//locals5
    strcpy(port[1][1],"locals5");//locals5

    int sfd=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in saddr,caddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);
    saddr.sin_port=htons(5555);
    bind(sfd, (SA*) &saddr,sizeof(saddr));
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(saddr.sin_addr), ip_str, INET_ADDRSTRLEN);
    printf("Server address- %s:%d\n",ip_str,ntohs(saddr.sin_port));

    listen(sfd,5);

    while(1){
        int len=sizeof(caddr);
        int newsfd=accept(sfd,(SA*) &caddr,(socklen_t *) &len);
        char ip_str1[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(caddr.sin_addr), ip_str1, INET_ADDRSTRLEN);
        printf("Client address- %s:%d\n",ip_str1,ntohs(caddr.sin_port));

        int x=fork();
        if(x>0){close(newsfd);}
        if(x==0){
            close(sfd);
            clienthandle_fxn(newsfd,nufd1,nufd2);
            close(newsfd);
            exit(0);
        }
    }
}