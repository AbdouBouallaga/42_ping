#include "../inc/ft_ping.h"
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
//             signal(SIGALRM, time);
//             alarm(1);
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
    ping.count = 0;
    ping.ttl = 128;
    ping.rcvTimeo = 10;
    ping.aInfo = &ping.aInfoStruct;

}

int main(int ac, char **av){
    int sockfd;
    int i = 0;
    int PingPong = 1;

    char buff[INET_ADDRSTRLEN];
    ping.ipStr = buff;
    
    init_ping();

    // get server info, including the ip address
    if (getaddrinfo(av[1], NULL, NULL, &ping.aInfo) != 0){
        printf("getaddrinfo error \n");
        exit(1);
    }
    else {
        // extract and print the ip address;
        if (inet_ntop(ping.aInfo->ai_family, &((struct sockaddr_in *) ping.aInfo->ai_addr)->sin_addr, ping.ipStr, sizeof(buff)) != NULL)
        printf("inet addr: %s\n", ping.ipStr);
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
    // ping loop
    while (PingPong){
        // clean icmp packet
        ft_bzero(&ping.pkt, sizeof(ping.pkt));
        // fillup icmp packet
        // set packet type to ICMP_ECHO
        ping.pkt.hdr.type = ICMP_ECHO;
        // set id to pid of process
        ping.pkt.hdr.un.echo.id = getpid();
        // fill msg (random)
        i = -1;
        while(++i < sizeof(ping.pkt.msg)){
            ping.pkt.msg[i] = 'A'+(i%16);
        }
        ping.pkt.msg[i] = '\0';
        // calculate checksum
        ping.pkt.hdr.checksum = checksum(&ping.pkt, sizeof(ping.pkt));
        // send the packet
        // if (sendto(sockfd, &ping.pkt, sizeof(ping.pkt, 0, )) == -1)
        pingPong = 0;
    }




    return(0);
}