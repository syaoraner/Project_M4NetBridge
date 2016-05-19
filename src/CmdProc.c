#include "config.h"
unsigned char g_ucHaveCMD = 0;
unsigned char UART_BUSY_flag = 0;
unsigned int d_con = 0;
unsigned char ds_sj_dao = 0;
unsigned char ds_sj_dao_021S = 0;
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
//  ���ڽ������ݴ��뻷��buf
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
//  UART�������ݴ��뻷��buf
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

//--------------------------------------------------------------------------------
//���ڻ���buf������
//@param  *ucDstAddr      Ŀ���ַָ��
//@param  *ucSrcAddr      Դ��ַָ��
//@param  ucDataType      ���ݴ���ķ��� 0����MC��M4��2����M4�ط�MC
//--------------------------------------------------------------------------------
void CacheOutputProc(StSysInf *ucDstAddr,StCacheProc *ucSrcAddr,unsigned char ucDataType)
{
  if(0 == ucDataType)
  {
//    M4�ط�MC buf��0
    memset(ucDstAddr->ucTcpTxBuf,0x00,1024);
//    ��������
    memcpy(ucDstAddr->ucTcpTxBuf,ucSrcAddr->ucMCRxBuf[ucSrcAddr->ucMCOutIndex],
             ucSrcAddr->usMCRxLen[ucSrcAddr->ucMCOutIndex]);
//    �����ݰ�����
    ucDstAddr->usTcpTxLen = ucSrcAddr->usMCRxLen[ucSrcAddr->ucMCOutIndex];
//    M4����buf��0
    memset(ucSrcAddr->ucMCRxBuf[ucSrcAddr->ucMCOutIndex],0x00,1024);
//    ÿ����һ�����ݣ�����buf����++
    ucSrcAddr->ucMCOutIndex++;
//    �������buf���ף����ض���
    if(ucSrcAddr->ucMCOutIndex >= MaxNum)
    {
      ucSrcAddr->ucMCOutIndex = 0;
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
//-----------------------------------------------------------------------
//ǰ̨����ת���������ӻ���buf�������ݲ�ת��
//-----------------------------------------------------------------------
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
  
  ///M4-->MCenter
  if(g_stCacheProc.ucUartInIndex != g_stCacheProc.ucUartOutIndex)
  {
    watchDogFeed();
//    �ط�MC���ݴ�����TCP_HuiFa_zxz���ж����и������Ÿ÷���˭
    if(g_stCacheProc.ucUartRxBuf[g_stCacheProc.ucUartOutIndex][0] != 3)
    {
      CacheOutputProc(&g_stSysInf,&g_stCacheProc,2);
      TCP_HuiFa_zxz(g_stSysInf.usUartTxLen,g_stSysInf.ucUartTxBuf[0]);
      SysCtlDelay(g_stSysInf.ulSysClock/30);
      SysCtlDelay(g_stSysInf.ulSysClock/30);
    }
    else
    {
    }
  }
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
    
    else
    {
    }
  }
}
