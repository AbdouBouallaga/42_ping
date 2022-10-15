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

#include <errno.h>

#include <netinet/ip_icmp.h>
#include <string.h>

#include <netinet/in.h>

#define PING_PKT_S 8192 // max ping size but irl max is 8184



// //from ip_icmp.h

// #define ICMP_ECHOREPLY		0	/* Echo Reply			*/
// #define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
// #define ICMP_ECHO                8        /* Echo Request     */
// #define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/

#define ICMP_ECHOREPLY          0               /* echo reply */
#define ICMP_UNREACH            3               /* dest unreachable, codes: */
#define ICMP_TIMXCEED           11              /* time exceeded, code: */


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

// struct msghdr {
//     void         *msg_name;       /* optional address */
//     socklen_t     msg_namelen;    /* size of address */
//     struct iovec *msg_iov;        /* scatter/gather array */
//     size_t        msg_iovlen;     /* # elements in msg_iov */
//     void         *msg_control;    /* ancillary data, see below */
//     size_t        msg_controllen; /* ancillary data buffer len */
//     int           msg_flags;      /* flags on received message */
// };

// struct sockaddr_in {
//     short            sin_family;   // e.g. AF_INET
//     unsigned short   sin_port;     // e.g. htons(3490)
//     struct in_addr   sin_addr;     // see struct in_addr, below
//     char             sin_zero[8];  // zero this if you want to
// };
/////////////////////////////////////////////////////////

struct ping_pkt {
    struct icmphdr hdr;
    char msg[PING_PKT_S-sizeof(struct icmphdr)];
};

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

struct          time_s{
  struct timeval      Timeval; // struct timeval {
                                  //    time_t      tv_sec;     /* seconds */
                                  //    suseconds_t tv_usec;    /* microseconds */
                                  // };
  struct timezone     Timezone; // struct timeval {
                                    //    int tz_minuteswest;     /* minutes west of Greenwich */
                                    //    int tz_dsttime;         /* type of DST correction */
                                    // };
};

typedef struct          s_ping{
    int                 pong; // ping.pong comtroled by signal
    int                 sockfd;
    int                 verbose;
    int                 sent_count;
    int                 rcev_count;
    int                 ttl;
    size_t              msg_size;
    size_t              sizeof_pkt;
    int                 count[2]; // 0 there is a count flag, 1 is the count value 
    char                ipStr[INET_ADDRSTRLEN];
    double              stats[3]; // 0 min, 1 max, 2 total to calculate avg
    struct ping_pkt     pkt;
    struct ping_pkt     *r_pkt;
    struct msghdr       r_msg;
    struct addrinfo     addrInfoStruct; // struct addrinfo {
                                        //    int              ai_flags;
                                        //    int              ai_family;
                                        //    int              ai_socktype;
                                        //    int              ai_protocol;
                                        //    socklen_t        ai_addrlen; 
                                        //    struct sockaddr *ai_addr;
                                        //    char            *ai_canonname;
                                        //    struct addrinfo *ai_next;
                                        // };
    struct addrinfo     *addrInfo; 
    struct time_s       timeC[2];
    struct timeval      rcvTimeval; // struct timeval {
                                    //    time_t  tv_sec;
                                    //    suseconds_t  tv_usec;
                                    // };
}                       t_ping;

#endif