#ifndef __TCPPROC_H__
#define __TCPPROC_H__

void TCP_MClient_Init(void);
void TCP_MClient_Close(void);
err_t TCPMCli_Connected(void *arg,struct tcp_pcb *pcb,err_t err);
err_t TCPMCli_recv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err);
void tcpLwipInit(void);

void TCP_PLCClient_Init(void);
err_t TCP_CliePLC_Connected(void *arg,struct tcp_pcb *pcb,err_t err);
void TCP_CliePLC_Send(unsigned char *data,unsigned long numhf);
err_t TCP_PLCCliRec(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err);
void TCP_CliePLC_Close(void);
#endif
