#include "kl_def.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>

//点分 ip 转整
//IN 192.168.1.100 OUT {192 168 1 100}
void IPtoInt(struct	sockaddr_in recv_ad, unsigned char ip[4],unsigned short *port)
{
	struct in_addr addr;
	unsigned int int_ip = 0;
	char src_ip[32];
	
	*port = ntohs(recv_ad.sin_port);
	memcpy(src_ip, (char *)inet_ntoa(recv_ad.sin_addr), strlen((char *)inet_ntoa(recv_ad.sin_addr)));
	if (inet_aton(src_ip, &addr))
	{
		int_ip = ntohl(addr.s_addr);
	}
	ip[3] = int_ip &0xff;
	ip[2] = int_ip >> 8 & 0xff;
	ip[1] = int_ip >> 16 & 0xff;
	ip[0] = int_ip >> 24 & 0xff;
	
	return;
}

//获取随机可用端口
//知名端口 0-1023 动态端口 1024-65535
// netstat -an | grep :xx 查询端口是否可用
unsigned short randPort(void)
{
	int		i_random_port;
	time_t	t;
	char	psz_port_cmd[256];
	
	srand((unsigned)time(&t));
	do
	{
		i_random_port = rand() % (65535 - 1025 + 1) + 1025;
		sprintf(psz_port_cmd, "netstat -an | grep :%d > /dev/null", i_random_port);
	}while(!system(psz_port_cmd));
	
	return (i_random_port);
}

#if 0
//ethernet
int InitSocket(Kl_EthernetParam_Ctrl IconCtrlStruct)
{
	struct ip_mreq mreq;       // Multicast group structure
	int socket_type;
	int socket_proto;
	unsigned char TTL = 1;
	unsigned char LOOP = 0;
	int rc;
	unsigned char proto = IconCtrlStruct.protocol;
	
	// Communication using UDP/IP protocol ----------------------------------------------------
	if(proto == UDP_PROTOCOL)
	{ 
		socket_proto = IPPROTO_UDP;
		socket_type = SOCK_DGRAM;
		OpalPrint("%s: Protocol       : UDP/IP\n", PROGNAME);
	}

	// Communication using TCP/IP protocol ----------------------------------------------------
	else if(proto == TCP_PROTOCOL)
	{
		socket_proto = IPPROTO_IP;
		socket_type = SOCK_STREAM;
		OpalPrint("%s: Protocol       : TCP/IP\n", PROGNAME);
	}

	// Protocol is not recognized -------------------------------------------------------------
	else
	{
		OpalPrint("%s: ERROR: Protocol (%d) not supported!\n", PROGNAME, proto);
		return (EINVAL);
	}
  
	OpalPrint("%s: Remote Address : %s\n", PROGNAME, IconCtrlStruct.StringParam[0]);
	OpalPrint("%s: Remote Port    : %d\n", PROGNAME, (int)IconCtrlStruct.FloatParam[1]);

	// Initialize the socket
	if((sd = socket(AF_INET, socket_type, socket_proto)) < 0)
	{
		OpalPrint("%s: ERROR: Could not open socket\n", PROGNAME);
		return (EIO);
	}
  
	// Set the structure for the remote port and address
	memset(&send_ad, 0, sizeof(send_ad));
	send_ad.sin_family = AF_INET;
	send_ad.sin_addr.s_addr = inet_addr(IconCtrlStruct.StringParam[0]);
	send_ad.sin_port = htons((u_short)IconCtrlStruct.FloatParam[1]);
  
	// Set the structure for the local port and address
	memset(&recv_ad, 0, sizeof(recv_ad));
	recv_ad.sin_family = AF_INET;
	recv_ad.sin_addr.s_addr = INADDR_ANY;
	recv_ad.sin_port = htons((u_short)IconCtrlStruct.FloatParam[2]);
  
	// Bind local port and address to socket.
	if(bind(sd, (struct sockaddr *)&recv_ad, sizeof(struct sockaddr_in)) == -1)
	{
		OpalPrint("%s: ERROR: Could not bind local port to socket\n", PROGNAME);
		return (EIO);
	}
	else
	  OpalPrint("%s: Local Port     : %d\n", PROGNAME, (int)IconCtrlStruct.FloatParam[2]);
 
	// Communication using UDP/IP protocol ----------------------------------------------------
	if(proto == UDP_PROTOCOL)
	{
		// If sending to a multicast address
		if((inet_addr(IconCtrlStruct.StringParam[0]) & inet_addr("240.0.0.0")) == inet_addr("224.0.0.0"))
		{
			if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&TTL, sizeof(TTL)) == -1)
			{
				OpalPrint("%s: ERROR: Could not set TTL for multicast send (%d)\n", PROGNAME, errno);
				return (EIO);
			}
			if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&LOOP, sizeof(LOOP)) == -1)
			{
				OpalPrint("%s: ERROR: Could not set loopback for multicast send (%d)\n", PROGNAME, errno);
				return (EIO);
			}

			OpalPrint("%s: Configured socket for sending to multicast address\n", PROGNAME);
		}

		// If receiving from a multicast group, register for it.
		if(inet_addr(IconCtrlStruct.StringParam[1]) > 0)
		{
			if ((inet_addr(IconCtrlStruct.StringParam[1]) & inet_addr("240.0.0.0")) == inet_addr("224.0.0.0"))
			{
				mreq.imr_multiaddr.s_addr = inet_addr(IconCtrlStruct.StringParam[1]);
				mreq.imr_interface.s_addr = INADDR_ANY;
	      
				// Have the multicast socket join the multicast group
				if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) == -1)
				{
					OpalPrint("%s: ERROR: Could not join multicast group (%d)\n", PROGNAME, errno);
					return (EIO);
				}

				OpalPrint("%s: Added process to multicast group (%s)\n", PROGNAME, IconCtrlStruct.StringParam[1]);
			}
			else
			{
				OpalPrint("%s: WARNING: IP address for multicast group is not in multicast range. Ignored\n", PROGNAME);
			}
		}
	}

	// Communication using TCP/IP protocol ----------------------------------------------------
	else if(proto == TCP_PROTOCOL)
	{
		OpalPrint("%s: Calling connect()\n", PROGNAME);
		// Connect to server to start data transmission
		rc = connect(sd, (struct sockaddr *)&send_ad, sizeof(send_ad));
		if (rc < 0) 
		{ 
			OpalPrint("%s: ERROR: Call to connect() failed\n", PROGNAME);
			return (EIO);
		} 
		OpalPrint("%s: Called connect()\n", PROGNAME);
	}

	return (EOK);
}

int SendPacket(char* DataSend, int datalength)
{
	int	err;
  
	if (sd < 0) return (-1);

	// Send the packet
	if(proto == TCP_PROTOCOL)
	  err = send(sd, DataSend, datalength, 0);
	else
	  err = sendto(sd, DataSend, datalength, 0, (struct sockaddr *)&send_ad, sizeof(send_ad));
  
	return (err);
}
#endifint RecvPacket(int id,char* DataRecv, int datalength, unsigned long timeout)
{
	int	 len = 0;
	socklen_t    client_ad_size = sizeof(struct sockaddr_in);
	fd_set sd_set;
	struct timeval tv, time_elapse, time_elapse1;
	int sd = g_sSockinf[id].ks_simulink_sd;
	char proto = g_sSockinf[id].kind;
  
	if (sd < 0) return (-1);
	if (timeout > 0)
	{
		// Set the descriptor set for the select() call
	   //
	   FD_ZERO(&sd_set);
		FD_SET(sd, &sd_set);
    
		// Set the tv structure to the correct timeout value
		tv.tv_sec = (unsigned long)(timeout / 1000000);
		tv.tv_usec = (unsigned long)(timeout % 1000000);

		// Wait for a packet. We use select() to have a timeout. This is
		// necessary when reseting the model so we don't wait indefinitely
		// and prevent the process from exiting and freeing the port for
		// a future instance (model load).
//		memset(&time_elapse, 0, sizeof(struct timeval));
//		gettimeofday(&time_elapse, NULL);
		switch(select(sd + 1, &sd_set, (fd_set*)0, (fd_set*)0, &tv))
		{
		case -1:
			// Error
			return (-1);
		case  0:
			// We hit the timeout
//			gettimeofday(&time_elapse1, NULL);
			
			return (0);
		default:
			if (!(FD_ISSET(sd, &sd_set)))
			{
				// We received something, but it's not on "sd". Since sd is the only
				// descriptor in the set...
				//OpalPrint("%s: RecvPacket: God, is that You trying to reach me?\n", PROGNAME);
				return (-2);
			}
		}  
	}
	// Clear the DataRecv array (in case we receive an incomplete packet)
	memset(DataRecv, 0, datalength);
	// Perform the reception
	if(proto == TCP_PROTOCOL)
	  len = recv(sd, DataRecv, datalength, 0);
	else
	  len = recvfrom(sd, DataRecv, datalength, 0, (struct sockaddr *)&g_sSockinf[id].recv_ad, &client_ad_size);
	
	if (len < 0)
	{
	}
    
	return (len);
}//////ring buf//////当in==out时，表明缓冲区为空的，当(in-out)==size 时，说明缓冲区已满
 /*

my_fifo_init

 */
struct my_fifo *my_fifo_init(unsigned char *buffer, unsigned int size)
{
	struct my_fifo *fifo;

	fifo = malloc(sizeof(struct my_fifo));
	if (!fifo)
		return NULL;

	fifo->buffer = buffer;
	fifo->size = size;
	fifo->in = fifo->out = 0;

	return fifo;
}

/*
 my_fifo_alloc
*/
struct my_fifo *my_fifo_alloc(unsigned int size)
{
	unsigned char *buffer;
	struct my_fifo *ret;
	/*
	* round up to the next power of 2, since our 'let the indices
	* wrap' tachnique works only in this case.
	*/

	buffer = malloc(size);
	if (!buffer)
		return NULL;

	ret = my_fifo_init(buffer, size);
	if (ret == NULL)
		free(buffer);

	return ret;
}
/*
* my_fifo_free
*/
void my_fifo_free(struct my_fifo *fifo)
{
	free(fifo->buffer) ;
	free(fifo) ;
}

/*
 my_fifo_put()
 */
unsigned int my_fifo_put(struct my_fifo *fifo, unsigned char *buffer, unsigned int len)
{
	unsigned int l ;

	len = min(len, fifo->size - fifo->in + fifo->out) ; /*可能是缓冲区的空闲长度或者要写长度*/

	/* first put the data starting from fifo->in to buffer end*/
	l = min(len, fifo->size - (fifo->in & (fifo->size - 1))) ;
	memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l) ;

	/* then put the rest (if any) at the beginning of the buffer*/
	memcpy(fifo->buffer, buffer + l, len - l) ;

	fifo->in += len ;

	return len ;
}

 
 /*
 my_fifo_get
 */
unsigned int my_fifo_get(struct my_fifo *fifo, unsigned char *buffer, unsigned int len)
{
	unsigned int l ;

	len = min(len, fifo->in - fifo->out) ; /*可读数据*/

	/* first get the data from fifo->out until the end of the buffer*/
	l = min(len, fifo->size - (fifo->out & (fifo->size - 1))) ;
	memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l) ;

	/* then get the rest (if any) from the beginning of the buffer*/
	memcpy(buffer + l, fifo->buffer, len - l) ;

	fifo->out += len ;

	return len ;
}

void my_fifo_reset(struct my_fifo *fifo)
{
	fifo->in = fifo->out = 0 ;
}
 
unsigned int my_fifo_len(struct my_fifo *fifo)
{
	return fifo->in - fifo->out ;
}

