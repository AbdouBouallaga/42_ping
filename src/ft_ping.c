#include "../inc/ft_ping.h"


t_ping ping;

void    hex_dump(u_char ch)
{
    int a = ch/16;
    int i = -1;
    while (++i < 2){
        if (a < 10)
            a = '0' + a;
        else
            a = 'a' + a - 10;
        ft_putchar(a);
        a = ch%16;
    }
}

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
    ping.msg_size = 56;
    ping.count[0] = 0;
    ping.count[1] = 0;
    ping.sent_count = 0;
    ping.rcev_count = 0;
    ping.ttl = 64;
    ping.stats[0] = 100; // min
    ping.stats[1] = 0; // max
    ping.stats[2] = 0; // total
    ping.rcvTimeval.tv_sec = 10;  /* 30 Secs Timeout */
    ping.addrInfo = &ping.addrInfoStruct;
    ping.sizeof_pkt = (sizeof(ping.s_pkt)-sizeof(ping.s_pkt.msg)) + ping.msg_size;
}

void    halt(){ // print stats and exit.
    printf("\n--- %s ping statistics ---\n",ping.ipStr);
    printf("%d packets transmitted, %d packets received, %d%% packet loss.\n",\
    ping.sent_count, ping.rcev_count, (int)(ping.sent_count - ping.rcev_count) / ping.sent_count * 100);
    if (ping.stats[0] < 100){
        printf("round-trip min/avg/max = %.3f / %.3f / %.3f ms\n",\
        ping.stats[0],\
        ping.stats[2]/ping.rcev_count,\
        ping.stats[1]\
        );
    }
    freeaddrinfo(ping.addrInfo);
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
    int rcv;

    NetIpHdr *r_ipHdr;
    uint8_t *r_ip; // uint32_t / 4, x.x.x.x

    // r message storage init
    char msgbuff[ping.sizeof_pkt+20];
    struct iovec iov[1];
    ft_bzero(&ping.r_msg, sizeof(ping.r_msg));
    // calculate checksum
    ping.s_pkt.hdr.checksum = checksum(&ping.s_pkt, ping.sizeof_pkt);

    // get send time
    gettimeofday(&ping.timeC[0].Timeval, NULL);
    // send the packet
    int snt = sendto(ping.sockfd, &ping.s_pkt, (size_t)ping.sizeof_pkt, 0, ping.addrInfo->ai_addr, sizeof(*ping.addrInfo->ai_addr));
    if (snt == -1){
        printf("sendto : %s\n", strerror(errno));
        ping.sent_count--;
    }
    ping.s_pkt.hdr.un.echo.sequence += (u_int16_t)1;


    ping.r_msg.msg_iov = iov;
    ping.r_msg.msg_iovlen = 1;
    ping.r_msg.msg_iov->iov_base = msgbuff;
    ping.r_msg.msg_iov->iov_len = sizeof(msgbuff);
    i = 1;
    while (i){
        rcv = recvmsg(ping.sockfd, &ping.r_msg, 0);
        if (rcv == -1){
            if (ping.verbose)
                printf("recvmsg : %s\n", strerror(errno));
            printf("Request timeout for icmp_seq %d\n", ping.s_pkt.hdr.un.echo.sequence);
            ping.rcev_count--;
            i = 0;
        }
        else {
            r_ipHdr = msgbuff;
            ping.r_pkt = &msgbuff[sizeof(NetIpHdr)];
            r_ip = (uint8_t *)&r_ipHdr->src_addr;
            printf("response type: %d, code: %d, checksum: %x, id: %d, seq: %d\n",\
            ping.r_pkt->hdr.type,\
            ping.r_pkt->hdr.code,\
            ping.r_pkt->hdr.checksum,\
            ping.r_pkt->hdr.un.echo.id,\
            ping.r_pkt->hdr.un.echo.sequence);
            // if (ping.r_pkt->hdr.un.echo.id != ping.s_pkt.hdr.un.echo.id){
            //     printf("id not match\n");
            //     goto end;
            // }
            // if ((unsigned long)r_ipHdr->src_addr != ((struct sockaddr_in *)ping.addrInfo->ai_addr)->sin_addr.s_addr){
            //     printf("src_addr: %x\n",(unsigned long)r_ipHdr->src_addr);
            //     printf("addr in_add : %x\n", ((struct sockaddr_in *) ping.addrInfo->ai_addr)->sin_addr);
            //     printf("ip not match\n");
            //     goto end;
            // }
            gettimeofday(&ping.timeC[1].Timeval, NULL);
            double time = (ping.timeC[1].Timeval.tv_usec - ping.timeC[0].Timeval.tv_usec)/1000.0;
            statsSave(time);
            if (ping.r_pkt->hdr.type == ICMP_ECHOREPLY){
                printf("%d bytes from %d.%d.%d.%d: icmp_seq=%d ttl=%d time=%.3f ms\n",\
                rcv,\
                r_ip[0],r_ip[1],r_ip[2],r_ip[3],\
                ping.r_pkt->hdr.un.echo.sequence,\
                r_ipHdr->timetolive,\
                time\
                );
                i = 0;
            }
            else if (ping.r_pkt->hdr.type == ICMP_UNREACH){
                printf("Destination unreachable\n");
                i = 0;
            }
            else if (ping.r_pkt->hdr.type == ICMP_TIMXCEED){
                printf("Time to Live exceeded in Transit\n");
                i = 0;
            }
        end:
        continue;
        }
    }
    // int j = -1;
    // while(++j < ping.sizeof_pkt+20){
    //     // printf("%x\n", msgbuff[j]);
    //     hex_dump((char)msgbuff[j]);
    //     ft_putchar('\n');
    // }

    // printf("ip : %d.%d.%d.%d\n", r_ip[0], r_ip[1], r_ip[2], r_ip[3]);
    // printf("src_addr: %lu\n",(unsigned long)r_ipHdr->src_addr);
    // printf("addr in_add : %lu\n", ((struct sockaddr_in *) ping.addrInfo->ai_addr)->sin_addr);
    // printf("ttl: %d\n",(int)r_ipHdr->timetolive);
    // perror("recvfrom")

    // printf("response type: %d, code: %d, checksum: %x, id: %d, seq: %d\n",\
    // ping.r_pkt->hdr.type,\
    // ping.r_pkt->hdr.code,\
    // ping.r_pkt->hdr.checksum,\
    // ping.r_pkt->hdr.un.echo.id,\
    // ping.r_pkt->hdr.un.echo.sequence);

    // else if (ping.r_pkt->hdr.type == ICMP_ECHOREPLY || ping.r_pkt->hdr.code == 0){
    //         printf("%d bytes from %d.%d.%d.%d: icmp_seq=%d ttl=%d time=%.3f ms\n",\
    //         rcv, r_ip[0], r_ip[1], r_ip[2], r_ip[3], ping.r_pkt.hdr.un.echo.sequence, ping.ttl, time);
    // }
    // else if (ping.r_pkt->hdr.type == ICMP_UNREACH){
    //     printf("Destination unreachable\n");
    // }
    // else if (ping.r_pkt->hdr.type == ICMP_TIMXCEED){
    //     printf("Time to Live exceeded in Transit\n");
    // }
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
    int dns;

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
        // printf("av[%d] = %s\n", i, av[i]);
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
        else if (av[i][1] == 's' && ft_itsdigit(av[i+1])){
            ping.msg_size = ft_atoi(av[i+1]);
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
        // printf("i = %d\n", i);
    }
    if (av[i] == NULL){
        usage(av[0]);
    }
    // set ping package size
    ping.sizeof_pkt = sizeof(ping.s_pkt)-sizeof(ping.s_pkt.msg) + ping.msg_size;
    // get address info
    dns = getaddrinfo(av[i], NULL, NULL, &ping.addrInfo);
    if (dns != 0){
        printf("<destination> error : %s\n", gai_strerror(dns));
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
        printf("setsockopt IP_TTL %s\n", strerror(errno));
    }
    // set the timeout of the recev function
    if (setsockopt(ping.sockfd, SOL_SOCKET, SO_RCVTIMEO, &ping.rcvTimeval, sizeof(ping.rcvTimeval)) != 0){
        printf("setsockopt SO_RCVTIMEO %s\n", strerror(errno));
    }
    // clean icmp packet
    ft_bzero(&ping.s_pkt, ping.sizeof_pkt);
    //// fillup icmp packet ////
    // set packet type to ICMP_ECHO
    ping.s_pkt.hdr.type = ICMP_ECHO;
    // set id to pid of process
    ping.s_pkt.hdr.un.echo.id = getpid();
    // fill msg (random)
    i = -1;
    if (ping.msg_size){
        while(++i < ping.msg_size){
            ping.s_pkt.msg[i] = 'Z';
        }
        ping.s_pkt.msg[i] = '\0';
    }
    printf("PING %s (%s) %d(%d) bytes of data.\n", av[i], ping.ipStr, ping.msg_size, (int)ping.sizeof_pkt);
    signal(SIGINT, halt);
    while (1){
        if (ping.pong){
            signal(SIGALRM,pingPong);
            alarm(1);
            ping.pong = 0;
            if (ping.count[0]){
                if (ping.count[1] == 0){
                    halt();
                }
                ping.count[1]--;
            }
        }
    }
    return(0);
}