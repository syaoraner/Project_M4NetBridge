#include "config.h"
unsigned char g_ucHaveCMD = 0;
unsigned char UART_BUSY_flag = 0;
unsigned int d_con = 0;
unsigned char ds_sj_dao = 0;
unsigned char ds_sj_dao_021S = 0;
unsigned char UART_PLC_right= 0;
unsigned char TCP_Return_Flag = 0;
unsigned char TCP_TxBuf[1024] = {0};
unsigned short TCP_TxLen = 0;
void CacheInputProc(StCacheProc *ucDstAddr,StSysInf *ucSrcAddr,
                    unsigned short usCopyLen,unsigned char ucDataType)
{
//  unsigned short i = 0;
  if(usCopyLen > 1024)
  {
    return;
  }
  if(ucDataType == 0)
  {
    ucDstAddr->usMCRxLen[ucDstAddr->ucMCInIndex] = usCopyLen - 3;
    memcpy(ucDstAddr->ucMCRxBuf[ucDstAddr->ucMCInIndex],&ucSrcAddr->ucTcpRecBuf[3],usCopyLen);
    ucDstAddr->ucMCInIndex++;
    if(ucDstAddr->ucMCInIndex >= MaxNum)
    {
      ucDstAddr->ucMCInIndex = 0;
    }
    memset(g_stSysInf.ucTcpRecBuf,0x00,1024);
    g_stSysInf.usTcpRecLen = 0;
  }
  else if(ucDataType == 1)
  {
    ucDstAddr->usPLCRxLen[ucDstAddr->ucPLCInIndex] = usCopyLen;
    memcpy(ucDstAddr->ucPLCRxBuf[ucDstAddr->ucPLCInIndex],ucSrcAddr->ucTcpPLCRecBuf,usCopyLen);
    ucDstAddr->ucPLCInIndex++;
    if(ucDstAddr->ucPLCInIndex >= MaxNum)
    {
      ucDstAddr->ucPLCInIndex = 0;
    }
    memset(g_stSysInf.ucTcpPLCRecBuf,0x00,1024);
    g_stSysInf.usTcpPLCRecLen = 0;
  }
  else if(ucDataType == 2)
  {
    ucDstAddr->usUartRxLen[ucDstAddr->ucUartInIndex] = usCopyLen;
    memcpy(ucDstAddr->ucUartRxBuf[ucDstAddr->ucUartInIndex],ucSrcAddr->ucUartRxBuf,usCopyLen);
    ucDstAddr->ucUartInIndex++;
    if(ucDstAddr->ucUartInIndex >= MaxNum)
    {
      ucDstAddr->ucUartInIndex = 0;
    }
    memset(g_stSysInf.ucUartRxBuf,0x00,1024);
    g_stSysInf.usUartRxLen = 0;
  }
}

void CacheOutputProc(StSysInf *ucDstAddr,StCacheProc *ucSrcAddr,unsigned char ucDataType)
{
  if(0 == ucDataType)
  {
    memset(ucDstAddr->ucTcpTxBuf,0x00,1024);
    memcpy(ucDstAddr->ucTcpTxBuf,ucSrcAddr->ucMCRxBuf[ucSrcAddr->ucMCOutIndex],
             ucSrcAddr->usMCRxLen[ucSrcAddr->ucMCOutIndex]);
    ucDstAddr->usTcpTxLen = ucSrcAddr->usMCRxLen[ucSrcAddr->ucMCOutIndex];
    memset(ucSrcAddr->ucMCRxBuf[ucSrcAddr->ucMCOutIndex],0x00,1024);
    ucSrcAddr->ucMCOutIndex++;
    if(ucSrcAddr->ucMCOutIndex >= MaxNum)
    {
      ucSrcAddr->ucMCOutIndex = 0;
    }
  }
  else if(1 == ucDataType)
  {
    memset(ucDstAddr->ucTcpPLCTxBuf,0x00,1024);
    memcpy(ucDstAddr->ucTcpPLCTxBuf,ucSrcAddr->ucPLCRxBuf[ucSrcAddr->ucPLCOutIndex],
             ucSrcAddr->usPLCRxLen[ucSrcAddr->ucPLCOutIndex]);
    ucDstAddr->usTcpPLCTxLen = ucSrcAddr->usPLCRxLen[ucSrcAddr->ucPLCOutIndex];
    memset(ucSrcAddr->ucPLCRxBuf[ucSrcAddr->ucPLCOutIndex],0x00,1024);
    ucSrcAddr->ucPLCOutIndex++;
    if(ucSrcAddr->ucPLCOutIndex >= MaxNum)
    {
      ucSrcAddr->ucPLCOutIndex = 0;
    }
  }
  else if(2 == ucDataType)
  {
    memset(ucDstAddr->ucUartTxBuf,0x00,1024);
    memcpy(ucDstAddr->ucUartTxBuf,ucSrcAddr->ucUartRxBuf[ucSrcAddr->ucUartOutIndex],
             ucSrcAddr->usUartRxLen[ucSrcAddr->ucUartOutIndex]);
    ucDstAddr->usUartTxLen = ucSrcAddr->usUartRxLen[ucSrcAddr->ucUartOutIndex];
    g_stCacheProc.ucUartOutIndex++;
    if(g_stCacheProc.ucUartOutIndex >= MaxNum)
    {
      g_stCacheProc.ucUartOutIndex = 0;
    }
  }
}

void DataProcess()
{
  ///MCenter-->M4
  if(g_stCacheProc.ucMCInIndex != g_stCacheProc.ucMCOutIndex)
  {
    watchDogFeed();
    if(0 == g_stSysInf.ucUartBusyFlag)
    {
      CacheOutputProc(&g_stSysInf,&g_stCacheProc,0);
      g_stSysInf.ucUartBusyFlag = 1;
      DMA_SendData(g_stSysInf.ucTcpTxBuf,g_stSysInf.usTcpTxLen);
      SysCtlDelay(g_stSysInf.ulSysClock/300);
    }
  }
  
  ///PLC-->M4
  if(g_stCacheProc.ucPLCInIndex != g_stCacheProc.ucPLCOutIndex)
  {
    watchDogFeed();
    if(0 == g_stSysInf.ucUartBusyFlag)
    {
      CacheOutputProc(&g_stSysInf,&g_stCacheProc,1);
      g_stSysInf.ucUartBusyFlag = 1;
      DMA_SendData(g_stSysInf.ucTcpPLCTxBuf,g_stSysInf.usTcpPLCTxLen);
      SysCtlDelay(g_stSysInf.ulSysClock/300);
    }
  }
  ///M4-->MCenter/PLC
  if(g_stCacheProc.ucUartInIndex != g_stCacheProc.ucUartOutIndex)
  {
    watchDogFeed();
    if(g_stCacheProc.ucUartRxBuf[g_stCacheProc.ucUartOutIndex][0] != 3)
    {
      CacheOutputProc(&g_stSysInf,&g_stCacheProc,2);
      TCP_HuiFa_zxz(g_stSysInf.usUartTxLen,g_stSysInf.ucUartTxBuf[0]);
      SysCtlDelay(g_stSysInf.ulSysClock/30);
      SysCtlDelay(g_stSysInf.ulSysClock/30);
    }
    else
    {
      if(0 == g_stSysInf.ucUartBusyFlag)
      {
        g_stCacheProc.ucUartOutIndex++;
        memset(g_stSysInf.ucUartTxBuf,0x00,1024);
        g_stSysInf.usUartTxLen = 0;
        g_stSysInf.ucUartBusyFlag = 1;
        ConfInfoReady();
        DMA_SendData(g_stSysInf.ucUartTxBuf,g_stSysInf.usUartTxLen);
        SysCtlDelay(g_stSysInf.ulSysClock/300);
      }
    }
  }
}

void ConfInfoReady(void)
{
  unsigned long ulCheckSum = 0;
  unsigned char i = 0;
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = 0xda;
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = 0x90;
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = 0xda;
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = 0x90;
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = 3;
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = 0;///Len
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = 0;
  ///NetBridge IP Port
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)(g_stSysInf.stLocalAddr.addr & 0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)((g_stSysInf.stLocalAddr.addr >> 8) & 0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)((g_stSysInf.stLocalAddr.addr >> 16) & 0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)((g_stSysInf.stLocalAddr.addr >> 24) & 0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)(g_stSysInf.usLocalPort & 0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)((g_stSysInf.usLocalPort >> 8) & 0xFF);
  ///PLC IP Port 
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)(g_stSysInf.stSerPLCAddr.addr & 0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)((g_stSysInf.stSerPLCAddr.addr >> 8) & 0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)((g_stSysInf.stSerPLCAddr.addr >> 16) & 0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)((g_stSysInf.stSerPLCAddr.addr >> 24) & 0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)(g_stSysInf.usSerPLCPort & 0xFF);;
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)((g_stSysInf.usSerPLCPort >> 8) & 0xFF);;
  
  ///Cal Frame Len
  g_stSysInf.ucUartTxBuf[6] = g_stSysInf.usUartTxLen + 2 - 7;
  
  ///CheckSum
  for(i=4;i<g_stSysInf.usUartTxLen;i++)
  {
    ulCheckSum += g_stSysInf.ucUartTxBuf[i];
  }
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)((ulCheckSum >> 8) &0xFF);
  g_stSysInf.ucUartTxBuf[g_stSysInf.usUartTxLen++] = (unsigned char)(ulCheckSum &0xFF);
  
}
void TCP_HuiFa_zxz(unsigned short usLen,unsigned char ucFlag)
{
  unsigned long temp_n = 1;
  if((usLen >0 ) && (usLen <1024 ))
  {
    if((ucFlag == 1) && (g_stSysInf.ucTCPConFlag == 1))//
    {  
      TCP_TxLen = 0;
      TCP_TxBuf[TCP_TxLen++] = 0xfa;
      TCP_TxBuf[TCP_TxLen++] = 0x01;
      TCP_TxBuf[TCP_TxLen++] = 0x01;
      for(temp_n = 3;temp_n < usLen;temp_n++)
      {
        TCP_TxBuf[TCP_TxLen++] = g_stSysInf.ucUartTxBuf[temp_n];
        if(g_stSysInf.ucUartTxBuf[temp_n] == 0xfa)
        {
          TCP_TxBuf[TCP_TxLen++] = 0x00;        
        }
      }
      tcp_write(g_stSysInf.MCClipcb, TCP_TxBuf,TCP_TxLen,1);
      tcp_output(g_stSysInf.MCClipcb);
      memset(TCP_TxBuf,0x00,1024);
    }
    else if((ucFlag == 2) && (g_stSysInf.ucTCPPLCConFlag == 1))//
    {
      TCP_TxLen = 0;
      TCP_TxLen = g_stSysInf.usUartTxLen - 3;
      for(temp_n=0;temp_n<TCP_TxLen;temp_n++)
      {
        TCP_TxBuf[temp_n] = g_stSysInf.ucUartTxBuf[temp_n+3];
      }
      tcp_write(g_stSysInf.PLCClipcb, TCP_TxBuf,TCP_TxLen,1);
      tcp_output(g_stSysInf.PLCClipcb);
      memset(TCP_TxBuf,0x00,1024);
    }
  }
}
