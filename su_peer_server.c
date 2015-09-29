#include "sarudp_peer.h"
#include "domain_parse.h"
#include "wrapfunc.h"

#include <sys/signal.h>

// test client ordinary
void sar_cli_send(FILE *fp, su_peer_t *psar);
// test client reliable
void sar_cli_send_recv(FILE *fp, su_peer_t *psar);

// handler reliable data come in
void udpin_reliable(su_peer_t *psar, char *buff, int len);
// handle ordinary data come in
void udpin_ordinary(su_peer_t *psar, char *buff, int len);

// handle SIGINT
void sigint(int no);

/* *
 * This program working promiscuous mode
 * Using macro
 * #define promiscuous_mode
 * compiling SARUDP Library
 * */
int main(int argc, char **argv)
{
    su_peer_t sar;
    char ip[256], errinfo[256];

    signal(SIGINT, sigint);

	if (argc != 1 && argc != 2)
		err_quit("usage: %s [Port Default 10000]", argv[0]);

#if 0
    /* The address is not used as a client, can be arbitrarily set  */
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(65535);
	Inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
#else
	struct sockaddr_in6 servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(65535);
	//Inet_pton(AF_INET6, "::ffff:127.0.0.1", &servaddr.sin6_addr); // for test localhost IPv6
	Inet_pton(AF_INET6, "::1", &servaddr.sin6_addr); // for test localhost IPv6
#endif

#if 0
    if (su_peer_create(&sar, (SA*)&servaddr, sizeof(servaddr)) < 0)
        err_quit("su_peer_create error");
#else
    if (su_peer_create_bind(&sar, argc == 1 ? 10000 : atoi(argv[1]), 
                (SA*)&servaddr, sizeof(servaddr)) < 0)
        err_sys("su_peer_create_bind error");
    log_msg("listen port %s successful", argc == 1 ? "10000" : argv[1]);
#endif

    su_peer_reliable_request_handle_install(&sar, udpin_reliable);
    su_peer_ordinary_request_handle_install(&sar, udpin_ordinary);

    while (1) 
        pause();

    exit(0);
}

void udpin_reliable(su_peer_t *psar, char *buff, int len)
{
    static long long c=0;
    struct sockaddr_in s4;
    socklen_t slen;

    su_peer_getsrcaddr(psar, (struct sockaddr*)&s4, &slen);
    printf("reliable recv from %s:%d datagram len %d\n", inet_ntoa(s4.sin_addr), ntohs(s4.sin_port), len);

    //c+=10; // sarudp header len;

    printf("reliable recv len %d datagrams " ColorGre "%s" ColorEnd 
            " count = %llu\n"ColorEnd, len, buff, c+=len);
    // reply successful and response data
    su_peer_reply(psar, buff, len);
    // reply successful but Do not carry data
    su_peer_reply(psar, 0, 0);
}
void udpin_ordinary(su_peer_t *psar, char *buff, int len)
{
    static long long c=0;
    struct sockaddr_in s4;
    socklen_t slen;

    su_peer_getsrcaddr(psar, (struct sockaddr*)&s4, &slen);
    printf("ordinary recv from %s:%d datagrams len %d\n", inet_ntoa(s4.sin_addr), ntohs(s4.sin_port), len);

    //c+=10; // sarudp header len;

    printf("ordinary recv len %d datagrams " ColorYel "%s" ColorEnd 
            " count = %llu\n"ColorEnd, len, buff, c+=len);
}

void sigint(int no)
{
    log_msg(ColorRed "\nSIGINT\n" ColorEnd);
    exit(0);
}
