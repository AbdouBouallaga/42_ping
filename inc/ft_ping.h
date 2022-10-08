#ifndef PING_HEADER
#define PING_HEADER

#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>


typedef struct  s_ping{
    struct addrinfo aInfo;
    char         *ipStr;
    int         ticks;
    int         ttl;
    int         timeout;
}               t_ping;

#endif