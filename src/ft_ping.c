#include "../inc/ft_ping.h"
#include <errno.h>
extern int errno ;

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


unsigned short checksum(void *b, int len){ // Calculating icmp CheckSum
    unsigned short *buf = b;
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

void init_ping(){ // init ping struct
    ping.pong = 1;
    ping.verbose = 0;
    ping.count[0] = 0;
    ping.count[1] = 0;
    ping.sent_count = 0;
    ping.rcev_count = 0;
    ping.ttl = 64;
    ping.stats[0] = 100; // min
    ping.stats[1] = 0; // max
    ping.stats[2] = 0; // total
    ping.rcvTimeval.tv_sec = 30;  /* 30 Secs Timeout */
    ping.addrInfo = &ping.addrInfoStruct;
}

void    halt(){ // print stats and exit.
    printf("\n--- %s ping statistics ---\n",ping.ipStr);
    printf("%d packets transmitted, %d packets received, %f%% packet loss.\n",\
    ping.sent_count, ping.rcev_count, (double)(ping.sent_count - ping.rcev_count) / ping.sent_count * 100);
    if (ping.stats[0] < 100){
        printf("round-trip min/avg/max = %.3f / %.3f / %.3f ms\n",\
        ping.stats[0],\
        ping.stats[2]/ping.rcev_count,\
        ping.stats[1]\
        );
    }
    exit(1);
}

void    statsSave(double time){
    if (ping.stats[0] > time) // min
        ping.stats[0] = time;
    if (ping.stats[1] < time) // max
        ping.stats[1] = time;
    ping.stats[2] += time; // total
}

void    pingPong(){
    int i;
    u_int16_t save_seq = ping.s_pkt.hdr.un.echo.sequence; // save old seq
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
    while(++i < (int)sizeof(ping.s_pkt.msg)){
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
    double time = (ping.time_recv.tv_nsec - ping.time_sent.tv_nsec)/1000000.0;
    statsSave(time);
    if (rcv == -1){
        perror("recvfrom");
        ping.rcev_count--;
    }
    else if (ping.r_pkt.hdr.type == ICMP_ECHOREPLY || ping.r_pkt.hdr.code == 0){
            printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",\
            rcv, ping.ipStr, ping.s_pkt.hdr.un.echo.sequence, ping.ttl, time);
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
    ping.sent_count++;
    ping.rcev_count++;
    ping.pong = 1;
}

void    usage(char *execName){
    printf("Usage:\n\t %s [options] <destination>\n", execName);
    printf("Options:\n");
    printf("\t-v                 verbose output\n");
    printf("\t-t <ttl>           define time to live\n");
    printf("\t-c <count>         stop after <count> replies\n");
    exit(0);
}

int ft_itsdigit(char *str){
    int i = -1;
    while(str[++i]){
        if (!ft_isdigit(str[i]))
            return (0);
    }
    return (1);
}

int main(int ac, char **av){
    int i;

    // printf("uid %d\n", getuid());
    // exit(1);
    if (getuid() != 0){
        printf("Please run as root\n");
        exit(1);
    }
    if(ac < 2)
	{
		usage(av[0]);
	}
    init_ping();
    i = 1;
    while(av[i][0] == '-' && i < ac){
        printf("av[%d] = %s\n", i, av[i]);
        if (av[i][1] == 'v'){
            ping.verbose = 1;
        }
        else if (av[i][1] == 't' && ft_itsdigit(av[i+1])){
            ping.ttl = ft_atoi(av[i+1]);
            i++;
        }
        else if (av[i][1] == 'c' && ft_itsdigit(av[i+1])){
            ping.count[0] = 1;
            ping.count[1] = ft_atoi(av[i+1]);
            i++;
        }
        else if (av[i][1] == 'h'){
            usage(av[0]);
        }
        else{
            printf("Invalid option: %s\n", av[i]);
            usage(av[0]);
        }
        i++;
        printf("i = %d\n", i);
    }
    if (av[i] == NULL){
        usage(av[0]);
    }
    // get address info
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
    printf("PING %s (%s) %d(%d) bytes of data.\n", av[i], ping.ipStr, (int)sizeof(ping.s_pkt.msg), (int)sizeof(ping.s_pkt));
    
    while (1){
        if (ping.pong){
            signal(SIGALRM,pingPong);
            alarm(1);
            ping.pong = 0;
            if (ping.count[1]){
                if (ping.count[1] == 0){
                    break;
                }
                ping.count[1]--;
            }
        }
    }
    return(0);
}