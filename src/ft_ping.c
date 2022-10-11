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
    ping.pong = 1;
    ping.sent_count = 0;
    ping.rcev_count = 0;
    ping.ttl = 64;
    ping.rcvTimeval.tv_sec = 30;  /* 30 Secs Timeout */
    ping.addrInfo = &ping.addrInfoStruct;
}

void    halt(){
    printf("\n--- %s ping statistics ---\n",ping.ipStr);
    printf("%d packets transmitted, %d packets received, %.2f%% packet loss\n",\
    ping.sent_count, ping.rcev_count, (double)(ping.sent_count - ping.rcev_count) / ping.sent_count * 100);
    printf("round-trip min/avg/max/stddev = 1.515/2.050/2.926/0.408 ms\n");
    exit(1);
}

void    pingPong(){
    u_int16_t save_seq = ping.s_pkt.hdr.un.echo.sequence;
    int i;
    // ping loop
    // clean icmp packet
    ft_bzero(&ping.s_pkt, sizeof(ping.s_pkt));
    ping.s_pkt.hdr.un.echo.sequence = save_seq;
    ft_bzero(&ping.r_pkt, sizeof(ping.r_pkt));
    //// fillup icmp packet
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
    // get send time
    clock_gettime(CLOCK_MONOTONIC, &ping.time_sent);
    // send the packet
    int snt = sendto(ping.sockfd, &ping.s_pkt, sizeof(ping.s_pkt), 0, ping.addrInfo->ai_addr, sizeof(*ping.addrInfo->ai_addr));
    if (snt == -1){
        perror("sendto");
        ping.sent_count--;
    }
    ping.s_pkt.hdr.un.echo.sequence++;
    int rcv = recvfrom(ping.sockfd, &ping.r_pkt, sizeof(ping.r_pkt), 0, 0, 0);
    clock_gettime(CLOCK_MONOTONIC, &ping.time_recv);
    if (rcv == -1){
        perror("recvfrom");
        ping.rcev_count--;
    }
    else if (ping.r_pkt.hdr.type == ICMP_ECHOREPLY || ping.r_pkt.hdr.code == 0){
            printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",\
            rcv, ping.ipStr, ping.s_pkt.hdr.un.echo.sequence, ping.ttl, (double)(ping.time_recv.tv_nsec - ping.time_sent.tv_nsec)/1000000.0);
    }
    else if (ping.r_pkt.hdr.type == ICMP_DEST_UNREACH){
        printf("Destination unreachable\n");
    }
    else if (ping.r_pkt.hdr.type == ICMP_TIME_EXCEEDED){
        printf("Time to Live exceeded in Transit\n");
    }
    else {
        printf("Request timeout for icmp_seq %d\n", ping.s_pkt.hdr.un.echo.sequence);
    }
    // printf("rcv code: %d, type: %d\n", ping.r_pkt.hdr.code, ping.r_pkt.hdr.type);
    // printf("recieved data type = %d code = %d checksum = %d\n", ping.r_pkt.hdr.type, ping.r_pkt.hdr.code, ping.s_pkt.hdr.checksum);
    // printf("%d bytes from %s: icmp_seq=%d ttl=64 time=1.246 ms\n", (int)sizeof(ping.r_pkt), ping.ipStr, ping.s_pkt.hdr.un.echo.sequence);
    ping.sent_count++;
    ping.rcev_count++;
    ping.pong = 1;
}

int main(int ac, char **av){
    int i;
    int hostAV;

    // printf("uid %d\n", getuid());
    // exit(1);
    if (getuid() != 0){
        printf("Please run as root\n");
        exit(1);
    }
    if(ac < 2)
	{
		printf("usage: %s [-Flags] <Hostname>\n", av[0]);
		exit(0);
	}
    i = 1;
    while(av[i][0] == '-'){
        
    }
    init_ping();
    // get server info, including the ip address
    if (getaddrinfo(av[1], NULL, NULL, &ping.addrInfo) != 0){
        printf("getaddrinfo error \n");
        exit(1);
    }
    else {
        // extract and print the ip address;
        inet_ntop(ping.addrInfo->ai_family, &((struct sockaddr_in *) ping.addrInfo->ai_addr)->sin_addr, ping.ipStr, INET_ADDRSTRLEN);
    }
    // open a raw socker with icmp prot
    ping.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping.sockfd < 1)
    {
        printf("socket failed\n");
    }
    // set Time to live (ttl) to limit hops of the packert
    if (setsockopt(ping.sockfd, IPPROTO_IP, IP_TTL, &ping.ttl, sizeof(ping.ttl)) != 0){
        perror("setsockopt IP_TTL");
    }
    // set the timeout of the recev function
    if (setsockopt(ping.sockfd, SOL_SOCKET, SO_RCVTIMEO, &ping.rcvTimeval, sizeof(ping.rcvTimeval)) != 0){
        perror("setsockopt SO_RCVTIMEO");
    }
    ft_bzero(&ping.s_pkt, sizeof(ping.s_pkt));
    // init the packet sequence number
    ping.s_pkt.hdr.un.echo.sequence = (u_int16_t)-1;
    signal(SIGINT, halt);
    printf("PING %s (%s) %d(%d) bytes of data.\n", av[hostAV], ping.ipStr, (int)sizeof(ping.s_pkt.msg), (int)sizeof(ping.s_pkt));
    while (1){
        if (ping.pong){
            signal(SIGALRM,pingPong);
            alarm(1);
            ping.pong = 0;
        }
    }
    return(0);
}