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

int main(int ac, char **av){
    int error;
    int error1;
    struct addrinfo *res;
    struct in_addr *inaddr;
    char buf[32];
    printf("len %d\n",INET_ADDRSTRLEN);
    error = getaddrinfo(av[1], NULL, NULL, &res);
    // if (inet_pton(AF_INET, av[1], &inaddr) == 1){
    //     if (inet_ntop(AF_INET, &inaddr, buf, sizeof(buf)) != NULL)
    //     printf("inet addr: %s\n", buf);
    // }
    // else {
    //     printf("inet_ntop error %d \n", error);
    //     exit(1);
    // }
    if (error != 0){
        printf("getaddrinfo error %d \n", error);
        exit(1);
    }
    else {
        printf("ai_flags %d \n", res->ai_flags);
        printf("ai_family %d \n", res->ai_family);
        printf("ai_socktype %d \n", res->ai_socktype);
        printf("ai_protocol %d \n", res->ai_protocol);
        printf("ai_canonname %x \n", res->ai_canonname);
        if (inet_ntop(res->ai_family, &((struct sockaddr_in *) res->ai_addr)->sin_addr, buf, sizeof(buf)) != NULL)
        printf("inet addr: %s\n", buf);
        // printf(" %d \n");
    }
    return(0);
}