#include "../inc/ft_ping.h"
#include <errno.h>
extern int errno ;

//getaddrinfo()
// struct addrinfo {
//                int              ai_flags;
//                int              ai_family;
//                int              ai_socktype;
//                int              ai_protocol;
//                socklen_t        ai_addrlen;
//                struct sockaddr *ai_addr;
//                char            *ai_canonname;
//                struct addrinfo *ai_next;
//            };

// void handler(int signum){
//     printf("\ntimew = %d\n", timec);
//     exit(1);
// }
// void time(int signum){
//     timec++;
//     a = 1;
//     // exit(1);
// }

// int main(int ac, char **av){
//     signal(SIGINT, handler);
//     while(count){
//         if (a){
//             a = 0;
//              
//         }
//     }
//     return(0);
// }
t_ping ping;

struct echo_request
    {
        char type; // Type
        char code; // Code
        short checksum; // Checksum
        short id; // Identification
        short seq; // Sequence
        int time; // Time
        char data[16]; // Data
    };

// Calculating the Check Sum
unsigned short checksum(void *b, int len)
{    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;
 
    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
// 

void init_ping(){
    ping.sent_count = 0;
    ping.rcev_count = 0;
    ping.ttl = 64;
    ping.rcvTimeo = 10;
    ping.addrInfo = &ping.addrInfoStruct;

}

int main(int ac, char **av){
    int sockfd;
    int pingPong = 1;
    int i;
    int argn;

    char buff[INET_ADDRSTRLEN];
    ping.ipStr = buff;
    struct sockaddr from;
    socklen_t *addrlen;

    
    init_ping();
    argn = 1;
    // get server info, including the ip address
    if (getaddrinfo(av[1], NULL, NULL, &ping.addrInfo) != 0){
        printf("getaddrinfo error \n");
        exit(1);
    }
    else {
        // extract and print the ip address;
        inet_ntop(ping.addrInfo->ai_family, &((struct sockaddr_in *) ping.addrInfo->ai_addr)->sin_addr, ping.ipStr, sizeof(buff));
    }
    // open a raw socker with icmp prot
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 1)
    {
        printf("socket failed\n");
    }
    // set Time to live (ttl) to limit hops of the packert
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ping.ttl, sizeof(ping.ttl)) != 0){
        printf("setsockopt TTL failed");
    }
    // set the timeout of the recev function
    if (setsockopt(sockfd, IPPROTO_IP, SO_RCVTIMEO, &ping.rcvTimeo, sizeof(ping.rcvTimeo)) != 0){
        printf("setsockopt RCVTIMEO failed");
    }
    printf("PING %s (%s) %d(%d) bytes of data.\n", av[argn], ping.ipStr, sizeof(ping.s_pkt.msg), sizeof(ping.s_pkt));
    // ping loop
    while (pingPong){
        // clean icmp packet
        ft_bzero(&ping.s_pkt, sizeof(ping.s_pkt));
        ft_bzero(&ping.r_pkt, sizeof(ping.r_pkt));
        // fillup icmp packet
        // set packet type to ICMP_ECHO
        ping.s_pkt.hdr.type = ICMP_ECHO;
        // set id to pid of process
        ping.s_pkt.hdr.un.echo.id = getpid();
        // fill msg (random)
        i = -1;
        while(++i < sizeof(ping.s_pkt.msg)){
            ping.s_pkt.msg[i] = 'A'+(i%16);
        }
        ping.s_pkt.msg[i] = '\0';
        // calculate checksum
        ping.s_pkt.hdr.checksum = checksum(&ping.s_pkt, sizeof(ping.s_pkt));
        // send the packet
        // send = 0;
        // send = sendto(sockfd, &ping.s_pkt, sizeof(ping.s_pkt), 0, ping.addrInfo->ai_addr, sizeof(*ping.addrInfo->ai_addr));
        int snt = sendto(sockfd, &ping.s_pkt, sizeof(ping.s_pkt), 0, ping.addrInfo->ai_addr, sizeof(*ping.addrInfo->ai_addr));
        // printf("snt value ========= %d\n", snt);
        if (snt == -1){
            printf("sendto error\n");
            printf("Value of errno: %d\n", errno);
            perror("sendto");
        }
        // printf("Value of snt: %d\n", errno);
        // perror("sendto");
        ping.sent_count++;
        printf("s_pkt type = %d ",ping.s_pkt.hdr.type);
        printf("s_pkt code = %d // ",ping.s_pkt.hdr.code);
        int rcv = recvfrom(sockfd, &ping.r_pkt, sizeof(ping.r_pkt), 0, 0, 0);
        // printf("rcv value ========= %d\n", rcv);
        if (rcv == -1){
            printf("recvfrom error\n");
        }
        // printf("Value of rcv: %d\n", errno);
        // perror("recvfrom");
        // }
        printf("s_pkt type = %d ",ping.r_pkt.hdr.type);
        printf("s_pkt code = %d\n",ping.r_pkt.hdr.code);


    }




    return(0);
}