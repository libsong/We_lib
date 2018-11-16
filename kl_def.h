#pragma once

#include "kl_types.h"
#include <netinet/in.h>

#define SFD_MAX_NUM		65

#define PROGNAME		"kl_simulink"

#define DATA_TYPE		char
#define DATA_RECV_MAX	2048
#define DATA_SEND_MAX	2048

char  g_szMsg[512];

//
extern int WeLog_2txt(const char *msg, ...);
extern void WeLog_Deinit();
extern void IPtoInt(struct	sockaddr_in recv_ad, unsigned char ip[4], unsigned short *port);
extern unsigned short randPort(void);
extern int RecvPacket(int id, char* DataRecv, int datalength, unsigned long timeout);

extern struct my_fifo *my_fifo_alloc(unsigned int size);
extern unsigned int my_fifo_put(struct my_fifo *fifo, unsigned char *buffer, unsigned int len);
extern unsigned int my_fifo_get(struct my_fifo *fifo, unsigned char *buffer, unsigned int len);
extern unsigned int my_fifo_len(struct my_fifo *fifo);
extern void my_fifo_free(struct my_fifo *fifo);



//
struct sUdpGotInf
{
	char *data;
	int src_port;
	char src_ip[32];
};

//
#define	GENASYNC_NB_FLOAT_PARAM			12
#define	GENASYNC_NB_STRING_PARAM		12
#define	GENASYNC_MAX_STRING_LENGTH		1000

#define UDP_PROTOCOL	1
#define TCP_PROTOCOL	2
#define	EOK		0

// Align bytes
#if defined(__GNUC__)
#	undef	GNUPACK
#	define	GNUPACK(x)		__attribute__ ((aligned(x),packed))
#else
#	undef	GNUPACK
#	define	GNUPACK(x)
#	if defined(__sgi)
#		pragma pack(1)
#	else
#		pragma pack (push, 1)
#	endif
#endif

typedef struct sKs_Thread_Para
{
	char id;
	int period;
	int want_len;
}Ks_Thread_Para;

typedef struct sKs_Simulink_SockInf
{
	int socket_proto;
	int socket_type;
	int ks_simulink_sd;
	int ks_simulink_err;
	char kind;
	char isNonBlock;	//×èÈû·ñ
	struct	sockaddr_in recv_ad;    // recv address
	struct	sockaddr_in send_ad;     // send address	
	
	Ks_Thread_Para para;
}Ks_Simulink_SockInf;
Ks_Simulink_SockInf g_sSockinf[SFD_MAX_NUM];

struct my_fifo {
	unsigned char *buffer; /* the buffer holding the data*/
	unsigned int size; /* the size of the allocated buffer*/
	unsigned int in; /* data is added at offset (in % size)*/
	unsigned int out; /* data is extracted from off. (out % size)*/
};

#define min(a,b) ((a) < (b) ? (a):(b))
#define FIFOBUFSIZE 4096
struct my_fifo *g_ptrKsRecvFifo[SFD_MAX_NUM];


