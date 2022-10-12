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
    int                 count[2]; // 0 there is a count flag, 1 is the count value 
    char                ipStr[INET_ADDRSTRLEN];
    double              stats[3]; // 0 min, 1 max, 2 total to calculate avg
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
    struct ping_pkt     s_pkt;
    struct ping_pkt     r_pkt;
    struct time_s       timeC[2];
    struct timeval      rcvTimeval; // struct timeval {
                                    //    time_t  tv_sec;
                                    //    suseconds_t  tv_usec;
                                    // };
    struct timespec     time_sent;
    struct timespec     time_recv;
    struct timespec     time_allStart;
    struct timespec     time_allStop;
}                       t_ping;

#endif