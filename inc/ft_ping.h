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

#include <sys/time.h>

#define PING_PKT_S 64



//from ip_icmp.h

#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_ECHO                8        /* Echo Request     */
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/

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

/////////////////////////////////////////////////////////

struct ping_pkt {
    struct icmphdr hdr;
    char msg[PING_PKT_S-sizeof(struct icmphdr)];
};

// struct timeval {
//    long  tv_sec;
//    long  tv_usec;
// };

typedef struct          s_ping{
    int                 pong; // ping.pong comtroled by signal
    int                 sockfd;
    int                 verbose;
    int                 sent_count;
    int                 rcev_count;
    int                 ttl;
    char                ipStr[INET_ADDRSTRLEN];
    struct addrinfo     addrInfoStruct;
    struct addrinfo     *addrInfo;
    struct ping_pkt     s_pkt;
    struct ping_pkt     r_pkt;
    struct timeval      rcvTimeval;
    struct timespec     time_sent;
    struct timespec     time_recv;
}                       t_ping;

#endif