#ifndef PING_HEADER
#define PING_HEADER

#include "../libft/libft.h"

#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h>

#define PING_PKT_S 64

//icmp header from ip_icmp.h
struct icmphdr
{
  u_int8_t type;                /* message type */
  u_int8_t code;                /* type sub-code */
  u_int16_t checksum;
  union
  {
    struct
    {
      u_int16_t        id;
      u_int16_t        sequence;
    } echo;                        /* echo datagram */
    u_int32_t        gateway;        /* gateway address */
    struct
    {
      u_int16_t        unused;
      u_int16_t        mtu;
    } frag;                        /* path mtu discovery */
  } un;
};
#define ICMP_ECHO                8        /* Echo Request     */


struct ping_pkt {
    struct icmphdr hdr;
    char msg[PING_PKT_S-sizeof(struct icmphdr)];
};


typedef struct          s_ping{
    struct addrinfo     addrInfoStruct;
    // struct addrinfo     *aInfo;
    struct addrinfo     *addrInfo;
    struct ping_pkt     s_pkt;
    struct ping_pkt     r_pkt;
    char                *ipStr;
    int                 sent_count;
    int                 rcev_count;
    int                 ttl;
    int                 rcvTimeo;
}                       t_ping;

#endif