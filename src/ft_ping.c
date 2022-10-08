#include "../inc/ft_ping.h"

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
t_ping *ping;

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

void init_ping(){
    ping->ticks = 0;
    ping->ttl = 128;
    ping->timeout = 10;

}

int main(int ac, char **av){
    int error;
    int error1;
    struct addrinfo *res;
    char buf[INET_ADDRSTRLEN];
    int sockfd;
    init_ping();
    // icmphdr icmp;
    error = getaddrinfo(av[1], NULL, NULL, &res);
    if (error != 0){
        printf("getaddrinfo error %d \n", error);
        exit(1);
    }
    else {
        if (inet_ntop(res->ai_family, &((struct sockaddr_in *) res->ai_addr)->sin_addr, buf, sizeof(buf)) != NULL)
        printf("inet addr: %s\n", buf);
    }
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 1)
    {
        printf("socket failed\n");
    }
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, 
    return(0);
}