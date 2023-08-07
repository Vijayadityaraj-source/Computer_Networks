#include<pcap.h>
#include<stdio.h>
#include<stdlib.h> // for exit()
#include<string.h> //for memset
 
#include<sys/socket.h>
#include<arpa/inet.h> // for inet_ntoa()
#include<net/ethernet.h>
#include<netinet/ip_icmp.h>   //Provides declarations for icmp header
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include <pthread.h>
#include <unistd.h>
#define SA struct sockaddr_in

pcap_t *handle; //Handle of the device that shall be sniffed

void process_packet(u_char *, const struct pcap_pkthdr *, const u_char *);
void print_tcp_packet(const u_char *  , int );
void print_ip_header(const u_char * Buffer, int Size);
void* sniff_fxn(void* args);
int sniffing_for_ports();
void* raw_fxn(void* args);

char* ip_addr_to_sniff = "127.0.0.1";

struct sockaddr_in source,dest;
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j; 

//ports are stored in port_storage
int port_storage[100];
int k=0;
int portupadtionflag=0;

#define SPOOF_IP "127.0.0.1"    
#define DEST_IP "127.0.0.1"
int newsfd1;
int main(){
    sniffing_for_ports();

    pthread_t tid_raw;
    pthread_create(&tid_raw,NULL,raw_fxn,NULL);
    pthread_join(tid_raw,NULL);

    int sfd1=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in saddr,caddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);
    saddr.sin_port=htons(9100);
    bind(sfd1, (SA*) &saddr,sizeof(saddr));
    listen(sfd1,5);

    int len=sizeof(caddr);
    newsfd1=accept(sfd1,(SA*) &caddr,(socklen_t *) &len);


    return 0;
}

void* raw_fxn(void* args){
    struct in_addr src_ip,dst_ip;
    if (inet_aton(SPOOF_IP, &src_ip) == 0) {
        fprintf(stderr, "Invalid source IP address\n");
        exit(1);
    }
    if (inet_aton(DEST_IP, &dst_ip) == 0) {
        fprintf(stderr, "Invalid destination IP address\n");
        exit(1);
    }

    int rsfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(rsfd < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    // struct sockaddr_in myaddr = {
    //  .sin_family = AF_INET,
    //  .sin_addr.s_addr = inet_addr("127.0.0.1")
    // };
    
    // bind(rsfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
    // perror("bind");
    
    const int one = 1;
    if (setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    

    while(1){
        unsigned char data[2*(k+1)];

        char packet[1500];
        struct ip *ipheader = (struct ip*)packet;
        // char data[100] = "Raw Socket OP";
        
        ipheader->ip_v = 4;
        ipheader->ip_hl = 5;
        ipheader->ip_tos = 0;
        ipheader->ip_len = sizeof(struct ip) + strlen(data);
        ipheader->ip_id = htons(54321);
        ipheader->ip_off = 0; // No fragmentation
        ipheader->ip_ttl = 255;
        ipheader->ip_p = IPPROTO_RAW;
        ipheader->ip_sum = 0;
        ipheader->ip_src = src_ip;
        ipheader->ip_dst = dst_ip;

        if(portupadtionflag){
            portupadtionflag=0;

            int port=port_storage[k-1];
            sprintf(data,"%d",port);

            strcpy(packet+sizeof(struct ip), data);
            
            struct sockaddr_in dest_addr = {
                .sin_family = AF_INET,
                .sin_addr.s_addr = inet_addr(DEST_IP)
            };
            
            sendto(rsfd, packet, ipheader->ip_len, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
            perror("sendto");
            
            char s_ip[128];
            inet_ntop(AF_INET, &ipheader->ip_src, s_ip, 128);
            
            char d_ip[128];
            inet_ntop(AF_INET, &ipheader->ip_dst, d_ip, 128);
            
            printf("Updated Data sent to %s from IP address - %s and also to ISS\n",d_ip, s_ip);

            char buff[100];
            bzero(buff,sizeof(buff));
            sprintf(buff,"%d",port);
            write(newsfd1,buff,sizeof(buff));
        }
    }
}

int sniffing_for_ports(){
    char errbuf[100] , *devname , devs[100][100];
    devname = "lo";

    bpf_u_int32 mask;
    bpf_u_int32 net;
    struct bpf_program fp;
    char ipaddr[20];
    sprintf(ipaddr,"host %s",ip_addr_to_sniff);
    char* filter_exp = ipaddr; // change to desired IP address
    struct pcap_pkthdr header;
    const u_char *packet;

    // Get network address and mask
    if (pcap_lookupnet(devname, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Can't get netmask for device %s\n", devname);
        net = 0;
        mask = 0;
    }
     
    //Open the device for sniffing
    printf("Opening device %s for sniffing ... " , devname);
    handle = pcap_open_live(devname , 65535 , 1 , 0 , errbuf);
     
    if (handle == NULL) 
    {
        fprintf(stderr, "Couldn't open device %s : %s\n" , devname , errbuf);
        exit(1);
    }
    printf("Done\n");

    // Compile filter expression
    printf("compiling ... ");
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }
    printf("Done\n");

    // Apply the filter
    printf("filtering ... ");
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }
    printf("Done\n");
     
    pthread_t tid;
    pthread_create(&tid, NULL, sniff_fxn, NULL);
    // pthread_join(tid, NULL);
}

void* sniff_fxn(void* args){
    //Put the device in sniff loop
    while(1) pcap_loop(handle , 1 , process_packet , NULL);
}

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer){
	unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)(buffer  + sizeof(struct ethhdr) );
    iphdrlen = iph->ihl*4;
     
    struct tcphdr *tcph=(struct tcphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
     
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    char* src_addr=inet_ntoa(source.sin_addr);
    int port=ntohs(tcph->source);

    //Checking if the ip is 127.0.0.1 and filtering the ephermal ports and standardized ports.
    if(strcmp(src_addr,ip_addr_to_sniff)==0 && port>8000 && port<8999){
        int flag=1;
        for(int i=0;i<k;i++){
            if(port_storage[i]==port){
                flag=0;
                break;
            }
        }
        if(flag){
            port_storage[k++]=port;
            printf("New port : %d\n",port);
            portupadtionflag=1;
        }
    }
}