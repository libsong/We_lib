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
  *   Created: Fri Nov  9 13:30:22 2018
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

void ks_net_send_Start_wrapper(const real_T *data,
                          const real_T *en,
                          const real_T *r1,
                          real_T *y0,
                          real_T *err,
                          real_T *r0  , 
                          const real_T  *ctl_id, const int_T  p_width0, 
                          const real_T  *protocol, const int_T  p_width1, 
                          const real_T  *isBcast, const int_T  p_width2, 
                          const real_T  *dst_addr, const int_T  p_width3, 
                          const real_T  *dst_port, const int_T  p_width4, 
                          const real_T  *r_para0, const int_T  p_width5, 
                          const real_T  *r_para1, const int_T  p_width6, 
                          const real_T  *r_para2, const int_T  p_width7, 
                          const real_T  *r_para3, const int_T  p_width8, 
                          const real_T  *r_para4,  const int_T p_width9,
			     const int_T y_width, const int_T u_width)
{
	int				id = (int)*ctl_id;
	int				i,len;
	char			RemoteAddr[16],buf[5];
	unsigned short	port = (unsigned short)*dst_port;
	int				nb = 0;
	const int		opt = 1;
	unsigned char	TTL = 1;
	unsigned char	LOOP = 0;
	
	WeLog_2txt("kl ethernet send start in, id = %d , sd = %d\n", id, g_sSockinf[id].ks_simulink_sd);
	
	
	if ((int)*protocol == 1)//udp 发送时指定目的地
	{
		if (p_width3 != 4)
		{
			WeLog_2txt("kl ethernet send dst ip ilegal , id = %d\n", id);
			
			return;
		}
		len = 0;
		for (i = 0; i < 4; i++) //ip 目的地址 如 [192 168 1 200]
		{
			sprintf(buf, "%d.", (int)dst_addr[i]);
			strncpy(&RemoteAddr[len], buf, strlen(buf));
			len += strlen(buf);
		}
		RemoteAddr[len - 1] = '\0';     //Remote_addr[len-1] is a "."
			WeLog_2txt("%s: RemoteAddr - %s!", PROGNAME,RemoteAddr);
			
		g_sSockinf[id].send_ad.sin_family = AF_INET;
		g_sSockinf[id].send_ad.sin_addr.s_addr = inet_addr(RemoteAddr);
		port = (unsigned short)*dst_port;
		g_sSockinf[id].send_ad.sin_port = htons(port);				
		
		if ((int)*isBcast == 1)
		{
			nb = setsockopt(g_sSockinf[id].ks_simulink_sd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
			if (nb == -1) {
				WeLog_2txt("%s: setsockopt failed!", PROGNAME);
				
				return ;
			}
			else
			{
				WeLog_2txt("%s: setsockopt broadcast suc!", PROGNAME);
				
			}
			return;
		}
		
		if ((inet_addr(RemoteAddr) & inet_addr("240.0.0.0")) == inet_addr("224.0.0.0"))
		{
			if (setsockopt(g_sSockinf[id].ks_simulink_sd, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&TTL, sizeof(TTL)) == -1)
			{
				WeLog_2txt("Could not set TTL for multicast send (%d)", errno);
				
				return ;
			}
			if (setsockopt(g_sSockinf[id].ks_simulink_sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&LOOP, sizeof(LOOP)) == -1)
			{
				WeLog_2txt("Could not set TTL for multicast IP_MULTICAST_LOOP send (%d)", errno);
				
				return ;
			}
		}
	}	
	
	WeLog_2txt("kl ethernet send start out, id = %d , sd = %d\n", id, g_sSockinf[id].ks_simulink_sd);
	
}

void ks_net_send_Outputs_wrapper(const real_T *data,
	const real_T *en,
	const real_T *r1,
	real_T *y0,
	real_T *err,
	real_T *r0, 
	const real_T  *ctl_id,
	const int_T  p_width0, 
	const real_T  *protocol,
	const int_T  p_width1, 
	const real_T  *isBcast,
	const int_T  p_width2, 
	const real_T  *dst_addr,
	const int_T  p_width3, 
	const real_T  *dst_port,
	const int_T  p_width4, 
	const real_T  *r_para0,
	const int_T  p_width5, 
	const real_T  *r_para1,
	const int_T  p_width6, 
	const real_T  *r_para2,
	const int_T  p_width7, 
	const real_T  *r_para3,
	const int_T  p_width8, 
	const real_T  *r_para4,
	const int_T p_width9,
	const int_T y_width,
	const int_T u_width)
{
	int start = (int)*en;
	int i;
	int	id = (int)*ctl_id;
	char ptr[DATA_SEND_MAX];
	int	maxnum = u_width;
	
//	WeLog_2txt("kl ethernet send in, id = %d , start = %d,u_width=%d,proto=%d\n", id, start,\
//		u_width,(int)*protocol);
	
	if (start == 1)
	{
		if (maxnum > DATA_SEND_MAX)
		{
			maxnum = DATA_SEND_MAX;
		}
		for (i = 0; i < maxnum; i++)
		{
			ptr[i] = (char)data[i];
		}
		if ((int)*protocol == 1)
		{
			if (g_sSockinf[id].isNonBlock == 1)
			{
				g_err[id]  = sendto(g_sSockinf[id].ks_simulink_sd, ptr, maxnum,\
					MSG_DONTWAIT, (struct sockaddr *)&g_sSockinf[id].send_ad, sizeof(struct sockaddr_in));
			}
			else
			{
				g_err[id]  = sendto(g_sSockinf[id].ks_simulink_sd, ptr, maxnum,\
					0, (struct sockaddr *)&g_sSockinf[id].send_ad, sizeof(struct sockaddr_in));
			}
			
		}
		else
		{
			if (g_sSockinf[id].isNonBlock == 1)
			{
				g_err[id]  = send(g_sSockinf[id].ks_simulink_sd, ptr, maxnum,\
					MSG_DONTWAIT|MSG_NOSIGNAL); //MSG_NOSIGNAL 禁止send()函数向系统发送异常消息
			}
			else
			{
				g_err[id]  = send(g_sSockinf[id].ks_simulink_sd, ptr, maxnum,\
					MSG_NOSIGNAL); 
			}			
		}
	}
	
	if (g_err[id] < 0)
	{
		*err = -1;
		return;
	}
	*err = 0;	
}

void ks_net_send_Terminate_wrapper(const real_T *data,
	const real_T *en,
	const real_T *r1,
	real_T *y0,
	real_T *err,
	real_T *r0, 
	const real_T  *ctl_id,
	const int_T  p_width0, 
	const real_T  *protocol,
	const int_T  p_width1, 
	const real_T  *isBcast,
	const int_T  p_width2, 
	const real_T  *dst_addr,
	const int_T  p_width3, 
	const real_T  *dst_port,
	const int_T  p_width4, 
	const real_T  *r_para0,
	const int_T  p_width5, 
	const real_T  *r_para1,
	const int_T  p_width6, 
	const real_T  *r_para2,
	const int_T  p_width7, 
	const real_T  *r_para3,
	const int_T  p_width8, 
	const real_T  *r_para4,
	const int_T p_width9,
	const int_T y_width,
	const int_T u_width)
{
	WeLog_Deinit();
}
