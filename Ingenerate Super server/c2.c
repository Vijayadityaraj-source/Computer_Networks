#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include<pthread.h>
#define SA struct sockaddr_in

int port=8081;
void* raw_fxn(void* args){
	int rsfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	perror("socket");

	struct sockaddr_in dest_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = inet_addr("127.0.0.1")
	};

	int k=0;
	char buffer[8192];
	recv(rsfd, buffer, sizeof(buffer), 0);
	struct ip *ipheader = (struct ip*)buffer;
	int service_ports[100];
	char temp[2];
	sprintf(temp,"%s",buffer + ipheader->ip_hl * 4);

	//port of s1
	port=port+(k);
	k++;
	printf("Data received is: %d\n", port);
}

void fxn(int sfd){
    char buff[100];
    while(1){
        bzero(buff,sizeof(buff));
        printf("To Server : ");
        fgets(buff,100,stdin);
        write(sfd,buff,sizeof(buff));
        bzero(buff,sizeof(buff));
        read(sfd,buff,sizeof(buff));
        printf("From  Server: %s",buff);
        if(strncmp("exit",buff,4)==0){
            printf("Client Exits!!");
            break;
        }
    }
}

int main(){
	pthread_t tid;
	pthread_create(&tid,NULL,raw_fxn,NULL);
	// pthread_join(tid,NULL);

	int sfd=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in caddr;
    bzero(&caddr, sizeof(caddr));
    caddr.sin_family=AF_INET;
    caddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    caddr.sin_port=htons(port);

    connect(sfd,(SA*)&caddr,sizeof(caddr));
    printf("connected successfully to server!\n");
    fxn(sfd);
}