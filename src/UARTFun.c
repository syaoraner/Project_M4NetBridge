#include "config.h"

#if defined(ewarm)
#pragma data_alignment=1024
unsigned char ui8ControlTable[1024];
#elif defined(ccs)
#pragma DATA_ALIGN(ui8ControlTable, 1024)
unsigned char ui8ControlTable[1024];
#else
unsigned char ui8ControlTable[1024] __attribute__ ((aligned(1024)));
#endif
unsigned char ackBufTag=0;
unsigned char UART_RX_STATE = 0;
unsigned char UART_RX_Flag = 0;
unsigned long UART_Mcenter_Count = 0;
unsigned char Mcenter_C_RX = 0;
unsigned long ulUartRxRealLen = 0;




void UARTInit(void)
{     
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
  SysCtlPeripheralEnable(DEF_SYSCTL_PERIPH_UART);    
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0);		
  GPIOPinConfigure(GPIO_PJ0_U3RX);
  GPIOPinConfigure(GPIO_PJ1_U3TX);
  GPIOPinTypeUART(DEF_GPIO_PORT_BASE, GPIO_PIN_0|GPIO_PIN_1);  
  UARTConfigSetExpClk(DEF_UART_BASE,
                      g_stSysInf.ulSysClock ,
                      9600,
                      UART_CONFIG_WLEN_8 |
                        UART_CONFIG_STOP_ONE |
                          UART_CONFIG_PAR_NONE);
  UARTFIFOLevelSet(DEF_UART_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
  UARTFIFOEnable(DEF_UART_BASE);
  UARTTxIntModeSet(DEF_UART_BASE,UART_TXINT_MODE_EOT);
  IntEnable(DEF_INT_UART);
  UARTIntEnable(DEF_UART_BASE, UART_INT_RX | UART_INT_RT | UART_INT_DMATX);
  UARTDMAEnable(DEF_UART_BASE,UART_DMA_TX);
  IntPrioritySet(DEF_INT_UART, 0x20);
  UARTEnable(DEF_UART_BASE);
}


void DMA_Init(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
  uDMAEnable();
  IntEnable(INT_UDMAERR);
  uDMAControlBaseSet(ui8ControlTable);
  IntEnable(INT_UDMA);
  uDMAChannelAssign(Uart_DMA_Channel);
  uDMAChannelAttributeDisable(Uart_DMA_SecChannel,
                              UDMA_ATTR_ALTSELECT |
                                UDMA_ATTR_HIGH_PRIORITY |
                                  UDMA_ATTR_REQMASK);
  uDMAChannelAttributeEnable(Uart_DMA_SecChannel, UDMA_ATTR_USEBURST);
  uDMAChannelControlSet(Uart_DMA_SecChannel | UDMA_PRI_SELECT,
                        UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE |
                          UDMA_ARB_1);
}

void DMA_SendData(unsigned char *ucBuf,unsigned short ulLen)
{
  uDMAChannelAssign(Uart_DMA_Channel);
  uDMAChannelAttributeDisable(Uart_DMA_SecChannel,//ͨ��ѡ��
                              UDMA_ATTR_ALTSELECT |//Ϊ���ͨ��ѡ��ͬ�Ŀ��ƽṹ
                                UDMA_ATTR_HIGH_PRIORITY |//�趨���ͨ���������ȼ�
                                  UDMA_ATTR_REQMASK);//�������ͨ������������Ӳ������
  uDMAChannelAttributeEnable(Uart_DMA_SecChannel, UDMA_ATTR_USEBURST);
  uDMAChannelControlSet(Uart_DMA_SecChannel | UDMA_PRI_SELECT,//ͨ���� 
                        UDMA_SIZE_8 | UDMA_SRC_INC_8 |UDMA_DST_INC_NONE |//���ݴ�С Դ��ַ���� Ŀ���ַ���� 
                          UDMA_ARB_4);//�ٲô�С
  uDMAChannelTransferSet(Uart_DMA_SecChannel | UDMA_PRI_SELECT,//ͨ����
                         UDMA_MODE_BASIC, ucBuf,//��������ִ��һ����������  �ô����Դ��ַ
                         (void *)(DEF_UART_BASE + UART_O_DR),//�ô����Ŀ���ַ
                         ulLen);//�ô�������ݴ�С
  uDMAChannelEnable(Uart_DMA_SecChannel);//��ʼ����
  
}

void uDMAErrorHandler(void)
{
  unsigned long ulStatus;
 
  ulStatus = uDMAErrorStatusGet();
  
  if (ulStatus)
  {
    uDMAErrorStatusClear();
  }
}

//void uDMATransferHandler(void)
//{
//  unsigned long ulStatus;
//  ulStatus=uDMAIntStatus();
//  uDMAIntClear(ulStatus);
//  if(GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_0))
//  GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,~GPIO_PIN_0);
//}

void UARTISR(void)
{
  unsigned long ulStatus;  
  signed long ucChar;
  unsigned char tep= 0;

//  �ж����ĸ�DMAͨ�������жϣ����ж�
  ulStatus=uDMAIntStatus();
  uDMAIntClear(ulStatus);
//  DMA�����ж�
  if ((ulStatus & UDMA_DEF_ADC03_SEC_RESERVED) == UDMA_DEF_ADC03_SEC_RESERVED)        
  {
    while(UARTBusy(DEF_UART_BASE));
//    RTC��Ϊ����
    if(GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_0))
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,~GPIO_PIN_0);
  }
  
  ulStatus = UARTIntStatus(DEF_UART_BASE, true);                
  UARTIntClear(DEF_UART_BASE, ulStatus); 
  if(UART_RX_Timer >= 55 && g_stSysInf.usUartRxLen > 0)
  {
    CacheInputProc(&g_stCacheProc,&g_stSysInf,g_stSysInf.usUartRxLen,DataType_UARTRec);
    UART_RX_Flag = 0;
  }
  if((ulStatus & UART_INT_RX) || (ulStatus & UART_INT_RT)) 
  {
    UART_RX_Flag = 1; 
    while(UARTCharsAvail(DEF_UART_BASE))//�ղ���
    {
      ucChar = UARTCharGet(DEF_UART_BASE);
      if(ucChar != -1)
      {    
        UART_RX_Timer = 0;
        tep = (unsigned char)ucChar;
        g_stSysInf.ucUartRxBuf[g_stSysInf.usUartRxLen++] = tep;
      }
    }
  }
}
