/*
  *
  *   --- THIS FILE GENERATED BY S-FUNCTION BUILDER: 3.0 ---
  *
  *   This file is a wrapper S-function produced by the S-Function
  *   Builder which only recognizes certain fields.  Changes made
  *   outside these fields will be lost the next time the block is
  *   used to load, edit, and resave this file. This file will be overwritten
  *   by the S-function Builder block. If you want to edit this file by hand, 
  *   you must change it only in the area defined as:  
  *
  *        %%%-SFUNWIZ_wrapper_XXXXX_Changes_BEGIN 
  *            Your Changes go here
  *        %%%-SFUNWIZ_wrapper_XXXXXX_Changes_END
  *
  *   For better compatibility with the Simulink Coder, the
  *   "wrapper" S-function technique is used.  This is discussed
  *   in the Simulink Coder User's Manual in the Chapter titled,
  *   "Wrapper S-functions".
  *
  *   Created: Fri Nov  9 10:41:20 2018
  */


/*
 * Include Files
 *
 */
#if defined(MATLAB_MEX_FILE)
#include "tmwtypes.h"
#include "simstruc_types.h"
#else
#include "rtwtypes.h"
#endif

#include <math.h>
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
#include <string.h>

static int g_err[SFD_MAX_NUM];

void ks_net_init_Start_wrapper(const real_T *u0,
	const real_T *u1,
	real_T *y0,
	real_T *y1, 
	const real_T  *ctl_id,
	const int_T  p_width0, 
	const real_T  *l_addr,
	const int_T  p_width1, 
	const real_T  *l_port,
	const int_T  p_width2, 
	const real_T  *r_para0,	//udp or tcp client
	const int_T  p_width3, 
	const real_T  *r_para1,// dst ip  when tcp client
	const int_T  p_width4, //dst port when tcp client
	const real_T  *r_para2,
	const int_T  p_width5, 
	const real_T  *r_para3,//当非阻塞模式时tcp尝试connect次数
	const int_T  p_width6, 
	const real_T  *r_para4,//是否阻塞io
	const int_T p_width7,
	const int_T y_width,
	const int_T u_width)
{
	int				id = (int)*ctl_id;
	char			proto = (char)*r_para0;
	unsigned short	port = (unsigned short)*l_port;
	int				connet_tryNum = (int)*r_para3;
	int				connet_cnt = 0;
	int				flag,i,len;
	char			RemoteAddr[16], LocalAddr[16], buf[5];
	
	WeLog_2txt("kl ethernet init socket start , id = %d , sd = %d , protocol = %d local port = %d", id, g_sSockinf[id].ks_simulink_sd, proto, port);

	g_err[id] = EOK;	
	g_sSockinf[id].ks_simulink_sd = 0;
	g_sSockinf[id].isNonBlock = (char)*r_para4;
	if (proto == UDP_PROTOCOL)
	{
		g_sSockinf[id].kind = UDP_PROTOCOL;
		g_sSockinf[id].socket_proto = IPPROTO_UDP;
		g_sSockinf[id].socket_type = SOCK_DGRAM;
	}
	if (proto == TCP_PROTOCOL)
	{
		g_sSockinf[id].kind = TCP_PROTOCOL;
		g_sSockinf[id].socket_proto = IPPROTO_IP;
		g_sSockinf[id].socket_type = SOCK_STREAM;
	}
	
	//sock init
	g_sSockinf[id].ks_simulink_sd = socket(AF_INET, g_sSockinf[id].socket_type, g_sSockinf[id].socket_proto);
	if (g_sSockinf[id].ks_simulink_sd < 0)
	{
		g_err[id] = g_sSockinf[id].ks_simulink_sd;
		WeLog_2txt("kl ethernet init socket failed , id = %d , errno = %d\n", id, g_err[id]);
		return;
	}
	
	//blocking or not
	if (g_sSockinf[id].isNonBlock == 1)
	{
		flag = fcntl(g_sSockinf[id].ks_simulink_sd, F_GETFL, 0);
		if (flag < 0)
		{
			WeLog_2txt("kl ethernet init F_GETFL failed\n");
			
			return;
		}
		flag |= O_NONBLOCK;
		if (fcntl(g_sSockinf[id].ks_simulink_sd, F_SETFL, flag) < 0)
		{
			WeLog_2txt("kl ethernet init F_SETFL failed\n");
			
			return;
		}
	}
	
	// Set the structure for the local port and address
	memset(&g_sSockinf[id].recv_ad, 0, sizeof(struct sockaddr_in));
	g_sSockinf[id].recv_ad.sin_family = AF_INET;
	if (p_width1 == 4)
	{
		len = 0;
		for (i = 0; i < 4; i++)
		{
			sprintf(buf, "%d.", (int)l_addr[i]);
			strncpy(&LocalAddr[len], buf, strlen(buf));
			len += strlen(buf);
		}
		LocalAddr[len - 1] = '\0';
		g_sSockinf[id].recv_ad.sin_addr.s_addr = inet_addr(LocalAddr);
		
		WeLog_2txt("kl ethernet local addr %s\n", LocalAddr);
		
	}
	else
	{
		WeLog_2txt("kl ethernet init Set local ip ilegal, id = %d", id);
		
		return;
	}
	
	if (port == 0)
	{
		port = randPort();
	}
	g_sSockinf[id].recv_ad.sin_port = htons(port);
	
	//bind local inf
	g_err[id] = bind(g_sSockinf[id].ks_simulink_sd, (struct sockaddr *)&g_sSockinf[id].recv_ad, sizeof(struct sockaddr_in));
	if (g_err[id] == -1)
	{
		WeLog_2txt("kl ethernet bind socket failed , id = %d , errno = %d\n", id, g_err[id]);
		
		return;
	}
	
	//tcp connect
	if(g_sSockinf[id].kind == TCP_PROTOCOL)
	{
		// Connect to server to start data transmission
		if(p_width4 != 4)
		{
			WeLog_2txt("kl ethernet Connect socket ip ilegal , id = %d\n",id);
			
			return;
		}
		len = 0;
		for(i = 0 ; i < 4 ; i++) //ip 目的地址 如 [192 168 1 200]
		{
			sprintf(buf, "%d.", (int)r_para1[i]);
			strncpy(&RemoteAddr[len], buf, strlen(buf));
			len += strlen(buf);
		}
		RemoteAddr[len - 1] = '\0';   //Remote_addr[len-1] is a "."
			WeLog_2txt("kl ethernet Connect socket to ip %s", RemoteAddr);
			
		g_sSockinf[id].send_ad.sin_family = AF_INET;
		g_sSockinf[id].send_ad.sin_addr.s_addr = inet_addr(RemoteAddr);
		port = (unsigned short)*r_para2;
		g_sSockinf[id].send_ad.sin_port = htons(port);
		
		if (g_sSockinf[id].isNonBlock == 1)
		{
			connet_cnt = 0;
			do
			{
				g_err[id] = connect(g_sSockinf[id].ks_simulink_sd, (struct sockaddr *)&g_sSockinf[id].send_ad, sizeof(g_sSockinf[id].send_ad));
				if (g_err[id] < 0) 
				{ 
					WeLog_2txt("kl ethernet connect socket failed with nonbolocking ,try %d times, id = %d\n", connet_cnt+1,id);
					
				}
				else
				{
					WeLog_2txt("kl ethernet connect socket suc with nonbolocking , id = %d , cnt = %d\n", id, connet_cnt);
					
					break;
				}		
				usleep(10);
			} while (++connet_cnt < connet_tryNum)
				;
		}
		if (g_sSockinf[id].isNonBlock == 2)
		{
			g_err[id] = connect(g_sSockinf[id].ks_simulink_sd, (struct sockaddr *)&g_sSockinf[id].send_ad, sizeof(g_sSockinf[id].send_ad));
			if (g_err[id] < 0) 
			{ 
				WeLog_2txt("kl ethernet connect socket failed with bolocking , id = %d\n", id);
				
				return;
			}
			else
			{
				WeLog_2txt("kl ethernet connect socket suc with bolocking, id = %d\n", id);
				
			}					
		}
				 
	}

	WeLog_2txt("kl ethernet init socket end , id = %d , sd = %d\n", id, g_sSockinf[id].ks_simulink_sd);
	
}

void ks_net_init_Outputs_wrapper(const real_T *u0,
                          const real_T *u1,
                          real_T *y0,
                          real_T *y1  , 
                          const real_T  *ctl_id, const int_T  p_width0, 
                          const real_T  *l_addr, const int_T  p_width1, 
                          const real_T  *l_port, const int_T  p_width2, 
                          const real_T  *r_para0, const int_T  p_width3, 
                          const real_T  *r_para1, const int_T  p_width4, 
                          const real_T  *r_para2, const int_T  p_width5, 
                          const real_T  *r_para3, const int_T  p_width6, 
                          const real_T  *r_para4,  const int_T p_width7,
			     const int_T y_width, const int_T u_width)
{
	int	id = (int)*ctl_id;
	*y1 = g_err[id];
}

void ks_net_init_Terminate_wrapper(const real_T *u0,
	const real_T *u1,
	real_T *y0,
	real_T *y1, 
	const real_T  *ctl_id,
	const int_T  p_width0, 
	const real_T  *l_addr,
	const int_T  p_width1, 
	const real_T  *l_port,
	const int_T  p_width2, 
	const real_T  *r_para0,
	const int_T  p_width3, 
	const real_T  *r_para1,
	const int_T  p_width4, 
	const real_T  *r_para2,
	const int_T  p_width5, 
	const real_T  *r_para3,
	const int_T  p_width6, 
	const real_T  *r_para4,
	const int_T p_width7,
	const int_T y_width,
	const int_T u_width)
{
	int	id = (int)*ctl_id;

	if (g_sSockinf[id].ks_simulink_sd > 0)
	{
		close(g_sSockinf[id].ks_simulink_sd);
		g_sSockinf[id].ks_simulink_sd = 0;
	}
	
	WeLog_Deinit();
}
