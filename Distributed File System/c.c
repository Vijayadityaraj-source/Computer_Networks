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
#include<pthread.h>
#define SA struct sockaddr

#define ipaddr "192.168.87.82"
char ans[1000];

void fxn(void* arg){
    // printf(".");
    char buff[100];
    int newsfd=*((int *)arg);
    read(newsfd,buff,sizeof(buff));
    // perror("read");
    strcat(ans,buff);
}

void dfs_fxn(int sfd){

    char buff[1000];
    bzero(buff,sizeof(buff));

    char file[1000];
    printf("Enter the name of the file needed : ");
    fgets(buff,1000,stdin);
    sprintf(file,"%s",buff);
    write(sfd,buff,sizeof(buff));

    bzero(buff,sizeof(buff));
    read(sfd,buff,sizeof(buff));

    if(strcmp(&buff[0],"E")==0){
        close(sfd);
        exit(0);
    }
    // printf("%s",buff);
    int size=atoi(&buff[0])/10;
    int num_of_locals=atoi(&buff[1]);
    // printf("%d %d\n",size,num_of_locals);
    char ip[size-num_of_locals][20];
    char port[size-num_of_locals][20];

    // for(int i=0;i<size;i++){
    //     bzero(buff,sizeof(buff));
    //     read(sfd,buff,sizeof(buff));
    //     sprintf(ip[i],"%s",buff);
    //     bzero(buff,sizeof(buff));
    //     read(sfd,buff,sizeof(buff));
    //     sprintf(port[i],"%s",buff);
    // }
    // for(int i=0;i<size;i++){
    //     printf("%s:%s\n",ip[i],port[i]);
    // }

    int newsfd[size-num_of_locals];
    pthread_t s[size-num_of_locals];
    int k=0;
    if(strcmp(file,"file1\n")==0){
        while(k<size-num_of_locals){
            newsfd[k]=socket(AF_INET,SOCK_STREAM,0);    
            char x[100];
            struct sockaddr_in saddr;
            saddr.sin_family=AF_INET;
            saddr.sin_addr.s_addr=inet_addr(ipaddr);
            saddr.sin_port=htons(8080+(5*k));
            bind(newsfd[k],(SA*) &saddr,sizeof(saddr));
            connect(newsfd[k],(SA*)&saddr,sizeof(saddr));
            // perror("connect");
            sprintf(x,"file1");
            write(newsfd[k],x,sizeof(x));                                                               
            pthread_create(&s[k], NULL, fxn, &newsfd[k]);
            k++;
        }
    }
    else if(strcmp(file,"file2\n")==0){
        newsfd[k]=socket(AF_INET,SOCK_STREAM,0);    
        char x[100];
        struct sockaddr_in saddr;
        saddr.sin_family=AF_INET;
        saddr.sin_addr.s_addr=inet_addr(ipaddr);
        saddr.sin_port=htons(8090);
        bind(newsfd[k],(SA*) &saddr,sizeof(saddr));
        connect(newsfd[k],(SA*)&saddr,sizeof(saddr));
        // perror("connect");
        sprintf(x,"file2");
        write(newsfd[k],x,sizeof(x));
        pthread_create(&s[k], NULL, fxn, &newsfd[k]);
        k++;
    }
    
    bzero(buff,sizeof(buff));
    read(sfd,buff,sizeof(buff));
    strcat(ans,buff);
    
    for(int i=0;i<size;i++) pthread_join(s[i],NULL);
    printf("%s",ans);
}

int main(){
    setbuf(stdin,NULL);
    setbuf(stdout,NULL);

    int sfd=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in caddr;
    bzero(&caddr, sizeof(caddr));
    caddr.sin_family=AF_INET;
    caddr.sin_addr.s_addr=inet_addr(ipaddr);
    caddr.sin_port=htons(5555);

    connect(sfd,(SA*)&caddr,sizeof(caddr));
    printf("connected successfully!\n");
    dfs_fxn(sfd);
    close(sfd);
}  