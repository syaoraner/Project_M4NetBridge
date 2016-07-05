#include "config.h"
unsigned char g_ucHaveCMD = 0;
unsigned char UART_BUSY_flag = 0;
unsigned int d_con = 0;
unsigned char ds_sj_dao = 0;
unsigned char ds_sj_dao_021S = 0;
unsigned char TCP_Return_Flag = 0;
unsigned char TCP_TxBuf[1024] = {0};
unsigned short TCP_TxLen = 0;

//-----------------------------------------------------------------------------
//���ڽ����ݰ��뻷��buf
//-----------------------------------------------------------------------------
void CacheInputProc(StCacheProc *ucDstAddr,StSysInf *ucSrcAddr,
                    unsigned short usCopyLen,unsigned char ucDataType)
{
//  unsigned short i = 0;
  if(usCopyLen > 1024)
  {
    return;
  }
//  ���ڽ������ݴ��뻷��buf
  if(ucDataType == DataType_EthRec)
  {
    if(g_stSysInf.ucNetCfgBuf[44] == 0)
    {
      ucDstAddr->usMCRxLen[ucDstAddr->ucMCInIndex] = usCopyLen - 3;
      memcpy(ucDstAddr->ucMCRxBuf[ucDstAddr->ucMCInIndex],&ucSrcAddr->ucTcpRecBuf[3],usCopyLen-3);
    }
    else if(g_stSysInf.ucNetCfgBuf[44] == 2)
    {
      ucDstAddr->usMCRxLen[ucDstAddr->ucMCInIndex] = usCopyLen;
      memcpy(ucDstAddr->ucMCRxBuf[ucDstAddr->ucMCInIndex],&ucSrcAddr->ucTcpRecBuf[0],usCopyLen);
    }
      ucDstAddr->ucMCInIndex++;
      if(ucDstAddr->ucMCInIndex >= MaxNum)
      {
        ucDstAddr->ucMCInIndex = 0;
      }
      memset(g_stSysInf.ucTcpRecBuf,0x00,1024);
      g_stSysInf.usTcpRecLen = 0;
    }

//  UART�������ݴ��뻷��buf
  else if(ucDataType == DataType_UARTRec)
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
//���ڴӻ���buf�������
//@param  *ucDstAddr      Ŀ���ַָ��
//@param  *ucSrcAddr      Դ��ַָ��
//@param  ucDataType      ���ݴ���ķ��� 0����M4��485��2����M4�ط�MC
//--------------------------------------------------------------------------------
void CacheOutputProc(StSysInf *ucDstAddr,StCacheProc *ucSrcAddr,unsigned char ucDataType)
{
  if(0 == ucDataType)
  {
//    M4�ط�MC buf��0
    memset(ucDstAddr->ucUartTxBuf,0x00,1024);
//    ��������
    memcpy(ucDstAddr->ucUartTxBuf,ucSrcAddr->ucMCRxBuf[ucSrcAddr->ucMCOutIndex],
             ucSrcAddr->usMCRxLen[ucSrcAddr->ucMCOutIndex]);
//    �����ݰ�����
    ucDstAddr->usUartTxLen = ucSrcAddr->usMCRxLen[ucSrcAddr->ucMCOutIndex];
//    M4����buf��0
    memset(ucSrcAddr->ucMCRxBuf[ucSrcAddr->ucMCOutIndex],0x00,1024);
    //      DEBUG�·�����ֱ�ӻط�
//#if     TCDEBUG
//      g_stSysInf.usTcpTxLen = g_stCacheProc.usMCRxLen[g_stCacheProc.ucMCOutIndex];
//      TCP_HuiFa_zxz(g_stSysInf.usTcpTxLen,1);
//#endif
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
    memset(ucDstAddr->ucTcpTxBuf,0x00,1024);
    memcpy(ucDstAddr->ucTcpTxBuf,ucSrcAddr->ucUartRxBuf[ucSrcAddr->ucUartOutIndex],
             ucSrcAddr->usUartRxLen[ucSrcAddr->ucUartOutIndex]);
    ucDstAddr->usTcpTxLen = ucSrcAddr->usUartRxLen[ucSrcAddr->ucUartOutIndex];
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
	
//  �ﵽ���ݰ���С���ʱ���Ҵ��ڽ��ջ��������ݣ���������������buf
  if(UART_RX_Timer >= 20 && g_stSysInf.usUartRxLen > 0)
  {
//    UARTIntDisable(DEF_UART_BASE, UART_INT_RX | UART_INT_RT | UART_INT_DMATX);
    CacheInputProc(&g_stCacheProc,&g_stSysInf,g_stSysInf.usUartRxLen,DataType_UARTRec);
//    UARTIntEnable(DEF_UART_BASE, UART_INT_RX | UART_INT_RT | UART_INT_DMATX);
    UART_RX_Flag = 0;
  }
  
  ///M4-->����
  if(g_stCacheProc.ucMCInIndex != g_stCacheProc.ucMCOutIndex)
  {
    watchDogFeed();
    if(0 == UARTBusy(DEF_UART_BASE))
    {
      CacheOutputProc(&g_stSysInf,&g_stCacheProc,0);
//      g_stSysInf.ucUartBusyFlag = 1;
//      ��RTC������
      GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,GPIO_PIN_0);
      DMA_SendData(g_stSysInf.ucUartTxBuf,g_stSysInf.usUartTxLen);
//      DMA���ݷ�������жϣ���RTC��������
//      SysCtlDelay(g_stSysInf.ulSysClock/300);      
    }
  }  
  ///M4-->����
  if(g_stCacheProc.ucUartInIndex != g_stCacheProc.ucUartOutIndex)
  {
    watchDogFeed();
//    �ط�MC���ݴ�����TCP_HuiFa_zxz���ж����и������Ÿ÷���˭
    if(g_stCacheProc.ucUartRxBuf[g_stCacheProc.ucUartOutIndex][0] != 3)
    {
      CacheOutputProc(&g_stSysInf,&g_stCacheProc,2);
      TCP_HuiFa_zxz(g_stSysInf.usTcpTxLen,g_stSysInf.ucTcpTxBuf);
//      SysCtlDelay(g_stSysInf.ulSysClock/3000);
    }
    else
    {
    }
  }
}

//--------------------------------------------------------------------------------
//TCPת�����ݣ�����ģʽ�жϼӲ���0xfa 0x01 0x01֡ͷ
//--------------------------------------------------------------------------------
void TCP_HuiFa_zxz(unsigned short usLen,unsigned char *p_ucTcpTxBuf)
{
  unsigned long temp_n = 1;
  if((usLen >0 ) && (usLen <1024 ))
  {
    if(g_stSysInf.ucTCPConFlag == 1)//
    {  
      TCP_TxLen = 0;
//      ���ʹ�ÿؼ�����֡ͷ
      if(g_stSysInf.ucNetCfgBuf[44] == 0)
      {
        TCP_TxBuf[TCP_TxLen++] = 0xfa;
        TCP_TxBuf[TCP_TxLen++] = 0x01;
        TCP_TxBuf[TCP_TxLen++] = 0x01;
      }      
      for(temp_n = 0;temp_n < usLen;temp_n++)
      {
        TCP_TxBuf[TCP_TxLen++] = p_ucTcpTxBuf[temp_n];
        if(p_ucTcpTxBuf[temp_n] == 0xfa)
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
