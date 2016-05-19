#ifndef __SYSINIT_H__
#define __SYSINIT_H__
#include "config.h"
#define MaxNum 10
typedef struct _StSysInf
{
  struct tcp_pcb *MCClipcb; 
  struct ip_addr stSerMCenAddr;
  struct ip_addr stLocalAddr;
  unsigned long ulSysClock;
  unsigned char ucNetCfgBuf[64];
  unsigned char ucMacAddr[6];
  unsigned short usSerPLCPort;
  unsigned short usSerMCPort;
  unsigned short usLocalPort;
  
  unsigned char ucTcpRecBuf[1024];
  unsigned short usTcpRecLen;
  unsigned char ucTcpTxBuf[1024];
  unsigned short usTcpTxLen;
  
  unsigned char ucUartRxBuf[1024];
  unsigned short usUartRxLen;
  unsigned char ucUartTxBuf[1024];
  unsigned short usUartTxLen;
  
  unsigned char ucComModel;
  unsigned long ulHWLinkStateTic;
  unsigned char ucMCReLinkFlag;
  
  unsigned char ucUartBusyFlag;
  unsigned char ucTCPConFlag;
  
}StSysInf;

typedef struct _StCacheProc
{
  unsigned short usMCRxLen[MaxNum];
  unsigned char ucMCRxBuf[MaxNum][1024];
  unsigned char ucMCInIndex;
  unsigned char ucMCOutIndex;
  
  unsigned short usUartRxLen[MaxNum];
  unsigned char ucUartRxBuf[MaxNum][1024];
  unsigned char ucUartInIndex;
  unsigned char ucUartOutIndex;
}StCacheProc;



void Init(void);

#endif
