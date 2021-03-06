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
  *   Created: Fri Nov  9 14:01:18 2018
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
#include <pthread.h>

static int g_err[SFD_MAX_NUM];
static char g_Tend[SFD_MAX_NUM];
static pthread_t	g_recvTid[SFD_MAX_NUM];

static void* recvThread(void* arg)
{	
	char		str[100];
	DATA_TYPE	recv_buf[DATA_RECV_MAX];
	int			id, count = -1,loop_p;
	Ks_Thread_Para inf = *(Ks_Thread_Para*)arg;
	
	id = inf.id;
	loop_p = inf.period;
	
	WeLog_2txt( "ks_net_recv_Start_wrapper pthread in ,id = %d\n",id);
	
	
	while (1)
	{
		if (g_Tend[id] == 1)
			break;
		
		if (g_sSockinf[id].ks_simulink_sd > 0)
		{
			count = -1;
			count = RecvPacket(id, recv_buf, g_sSockinf[id].para.want_len, 0); //线程中直接轮询，暂不考虑时间等待
			if (count > 0)
			{
				my_fifo_put(g_ptrKsRecvFifo[id],(unsigned char *)recv_buf,count);
			}
		}
		usleep(loop_p);
	}
	
	WeLog_2txt( "ks_net_recv_Start_wrapper pthread out ,id = %d\n", id);
	

	return 0;
}

void ks_net_recv_Start_wrapper(const real_T *u0,
                          const real_T *timeout,
                          const real_T *u_r0,
                          real_T *recvdata,
                          real_T *err,
                          real_T *y_r0  , 
                          const real_T  *ctl_id, const int_T  p_width0, 
                          const real_T  *protocol, const int_T  p_width1, 
                          const real_T  *isBcast, const int_T  p_width2, 
                          const real_T  *src_addr, const int_T  p_width3, 
                          const real_T  *src_port, const int_T  p_width4, 
                          const real_T  *enRecvTh, const int_T  p_width5, 
                          const real_T  *r_para0, const int_T  p_width6, 
                          const real_T  *r_para1, const int_T  p_width7, 
                          const real_T  *r_para2, const int_T  p_width8, 
                          const real_T  *r_para3, const int_T  p_width9, 
                          const real_T  *r_para4,  const int_T p_width10,
			     const int_T y_width, const int_T u_width)
{
	char	thread = (char)*enRecvTh;
	int		id = (int)*ctl_id;
	int		loopPeriod = (int)*r_para0;
	char	isRecvMulcast = (char)*r_para1;
	struct	ip_mreq mreq; 
	char	LocalAddr[16], buf[5];
	int		len,i;
	
	if (isRecvMulcast == 1)//从组播地址接收
	{
		if (p_width8 == 4)
		{
			len = 0;
			for (i = 0; i < 4; i++)
			{
				sprintf(buf, "%d.", (int)r_para2[i]);
				strncpy(&LocalAddr[len], buf, strlen(buf));
				len += strlen(buf);
			}
			LocalAddr[len - 1] = '\0';//组播地址
		}
		if ((inet_addr(LocalAddr) & inet_addr("240.0.0.0")) == inet_addr("224.0.0.0"))
		{
			mreq.imr_multiaddr.s_addr = inet_addr(LocalAddr);
			if (p_width9 == 4)
			{
				len = 0;
				for (i = 0; i < 4; i++)
				{
					sprintf(buf, "%d.", (int)r_para3[i]);
					strncpy(&LocalAddr[len], buf, strlen(buf));
					len += strlen(buf);
				}
				LocalAddr[len - 1] = '\0';//本地地址
			}
			mreq.imr_interface.s_addr = inet_addr(LocalAddr);
	      
			// Have the multicast socket join the multicast group
			if(setsockopt(g_sSockinf[id].ks_simulink_sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) == -1)
			{
				WeLog_2txt("%s: ERROR: Could not join multicast group (%d)\n", PROGNAME, errno);
				
				return ;
			}
		}
	}
	
	
	if (thread == 1)//开线程接收
	{
		g_err[id] = 0;
		g_ptrKsRecvFifo[id] = NULL;
		
		g_ptrKsRecvFifo[id] = my_fifo_alloc(FIFOBUFSIZE);
		if (g_ptrKsRecvFifo[id] == NULL)
		{
			WeLog_2txt("ks_net_recv_Start_wrapper my_fifo_alloc failed id = %d\n", id);
			
			g_err[id] = -1;
			return;
		}
		
		g_Tend[id] = 0;
		g_sSockinf[id].para.id = id;
		if (loopPeriod <=0)
		{
			loopPeriod = 99;
		}
		g_sSockinf[id].para.period = loopPeriod;
		if (pthread_create(&g_recvTid[id], NULL, recvThread, &g_sSockinf[id].para) != 0)
		{
			WeLog_2txt( "ks_net_recv_Start_wrapper pthread_create failed");
			
		}
	}
}

void ks_net_recv_Outputs_wrapper(const real_T *u0,
	const real_T *timeout,
	const real_T *u_r0,
	real_T *recvdata,
	real_T *err,//用来标识数据是否是新，本步长成功读取后加 1
	real_T *y_r0, 
	const real_T  *ctl_id,
	const int_T  p_width0, 
	const real_T  *protocol,
	const int_T  p_width1, 
	const real_T  *isBcast,
	const int_T  p_width2, 
	const real_T  *src_addr,
	const int_T  p_width3, 
	const real_T  *src_port,
	const int_T  p_width4, 
	const real_T  *enRecvTh,
	const int_T  p_width5, 
	const real_T  *r_para0,
	const int_T  p_width6, 
	const real_T  *r_para1,
	const int_T  p_width7, 
	const real_T  *r_para2,
	const int_T  p_width8, 
	const real_T  *r_para3,
	const int_T  p_width9, 
	const real_T  *r_para4,
	const int_T p_width10,
	const int_T y_width,
	const int_T u_width)
{
	int				id = (int)*ctl_id;
	char			thread = (char)*enRecvTh;
	int				len = 0;
	unsigned long	loopperiod = (unsigned long)*r_para0;
	int				wantdatalen = u_width;
	char			recv_buf[DATA_RECV_MAX];
	static unsigned long long isnewdata = 0;
	int				i;
	int				tout = (int)*timeout;
	unsigned short	tmp_port = 0;
	unsigned char	ip[4] = {0,0,0,0};
	char			ipEqual = 1;
	static int		pre_wantdatalen = 0;
	
	if (wantdatalen > DATA_RECV_MAX)
	{
		wantdatalen = DATA_RECV_MAX;
	}
	
	if (thread == 1)//开线程接收的
	{
		if (pre_wantdatalen != wantdatalen)
		{
			pre_wantdatalen = wantdatalen;
			g_sSockinf[id].para.want_len = wantdatalen;
		}
		len = my_fifo_get(g_ptrKsRecvFifo[id], (unsigned char *)recv_buf, wantdatalen);
		*y_r0 = g_err[id];
	}
	else
	{
		len = RecvPacket(id, recv_buf, wantdatalen, tout);	
		*y_r0 = len;
	}
		
	if (len > 0)
	{
		*err = ++isnewdata;
			
		if ((int)*protocol == 1)//udp
		{
			IPtoInt(g_sSockinf[id].recv_ad,ip,&tmp_port);
			if ((int)src_addr[0] == 0 && (int)src_addr[1] == 0 && (int)src_addr[2] == 0 && (int)src_addr[3] == 0) //不过滤源ip
			{
				if ((int)*src_port == 0) //不过滤源端口
				{
					for (i = 0; i < len; i++)
					{
						recvdata[i] = recv_buf[i] * 1.0;
					}	
				}
				else
				{
					if ((int)*src_port == tmp_port)//过滤源端口
					{
						for (i = 0; i < len; i++)
						{
							recvdata[i] = (real_T)recv_buf[i];
						}
					}
				}
			}
			else	//过滤ip
			{
				for (i = 0; i < 4; i++)
				{
					if ((int)src_addr[i] != ip[i])
					{
						ipEqual = 0;
					}
				}
				if (ipEqual == 1)
				{
					if ((int)*src_port == 0) //不过滤源端口
					{
						for (i = 0; i < len; i++)
						{
							recvdata[i] = recv_buf[i] * 1.0;
						}
					}
					else
					{
						if ((int)*src_port == tmp_port)//过滤源端口
						{
							for (i = 0; i < len; i++)
							{
								recvdata[i] = recv_buf[i] * 1.0;
							}
						}
					}
				}
			}
		}
		if ((int)*protocol == 2)
		{
			for (i = 0; i < len; i++)
			{
				recvdata[i] = recv_buf[i] * 1.0;
			}
		}				
	}	
}

void ks_net_recv_Terminate_wrapper(const real_T *u0,
	const real_T *timeout,
	const real_T *u_r0,
	real_T *recvdata,
	real_T *err,
	real_T *y_r0, 
	const real_T  *ctl_id,
	const int_T  p_width0, 
	const real_T  *protocol,
	const int_T  p_width1, 
	const real_T  *isBcast,
	const int_T  p_width2, 
	const real_T  *src_addr,
	const int_T  p_width3, 
	const real_T  *src_port,
	const int_T  p_width4, 
	const real_T  *enRecvTh,
	const int_T  p_width5, 
	const real_T  *r_para0,
	const int_T  p_width6, 
	const real_T  *r_para1,
	const int_T  p_width7, 
	const real_T  *r_para2,
	const int_T  p_width8, 
	const real_T  *r_para3,
	const int_T  p_width9, 
	const real_T  *r_para4,
	const int_T p_width10,
	const int_T y_width,
	const int_T u_width)
{
	int	id = (int)*ctl_id;
	
	g_Tend[id] = 1;
	if (g_ptrKsRecvFifo[id] != NULL)
	{
		my_fifo_free(g_ptrKsRecvFifo[id]);		
	}
	
	WeLog_Deinit();
}
