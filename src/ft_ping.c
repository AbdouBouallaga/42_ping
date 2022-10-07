#include "ft_ping.h"

struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
};

struct ping_pkt
{
	struct icmphdr hdr;
	char msg[PING_PKT_S-sizeof(struct icmphdr)];
};

int main(int ac, char **av){
    if (ac < 2)
    {
        printf("USAGE\n");
        return(0);
    }
    int sockfd;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd<0)
    {
        printf("\nSocket file descriptor not received!!\n");
        return 0;
    }
    inet_pton
    return(0);
}