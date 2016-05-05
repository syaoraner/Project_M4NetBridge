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
unsigned long UART_PLC_Count = 0;
unsigned long ulUartRxRealLen = 0;
unsigned char PLC_C_RX = 0;
unsigned int uiShRecLen = 0;
unsigned char uartMcenterEmpty = 0;
unsigned long yjs_shuju_leng = 0;



void UARTInit(void)
{     
  SysCtlPeripheralEnable(DEF_SYSCTL_PERIPH_UART);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);   
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_2);		
  GPIOPinConfigure(GPIO_PD4_U2RX);
  GPIOPinConfigure(GPIO_PD5_U2TX);
  GPIOPinTypeUART(DEF_GPIO_PORT_BASE, GPIO_PIN_4|GPIO_PIN_5);  
  UARTConfigSetExpClk(DEF_UART_BASE,
                      g_stSysInf.ulSysClock ,
                      115200,
                      UART_CONFIG_WLEN_8 |
                        UART_CONFIG_STOP_ONE |
                          UART_CONFIG_PAR_NONE);
  UARTFIFOLevelSet(DEF_UART_BASE, UART_FIFO_TX1_8, UART_FIFO_RX2_8);
  UARTFIFOEnable(DEF_UART_BASE);
  UARTTxIntModeSet(DEF_UART_BASE,UART_TXINT_MODE_EOT);
  IntEnable(DEF_INT_UART);
  UARTIntEnable(DEF_UART_BASE, UART_INT_RX | UART_INT_RT| UART_INT_TX);
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
  uDMAChannelAttributeDisable(Uart_DMA_SecChannel,//通道选择
                              UDMA_ATTR_ALTSELECT |//为这个通道选择不同的控制结构
                                UDMA_ATTR_HIGH_PRIORITY |//设定这个通道到高优先级
                                  UDMA_ATTR_REQMASK);//屏蔽这个通道的相关外设的硬件请求
  uDMAChannelAttributeEnable(Uart_DMA_SecChannel, UDMA_ATTR_USEBURST);
  uDMAChannelControlSet(Uart_DMA_SecChannel | UDMA_PRI_SELECT,//通道号 
                        UDMA_SIZE_8 | UDMA_SRC_INC_8 |UDMA_DST_INC_NONE |//数据大小 源地址增量 目标地址增量 
                          UDMA_ARB_4);//仲裁大小    
  uDMAChannelTransferSet(Uart_DMA_SecChannel | UDMA_PRI_SELECT,//通道号
                         UDMA_MODE_BASIC, ucBuf,//根据请求执行一个基本传输  该传输的源地址
                         (void *)(DEF_UART_BASE + UART_O_DR),//该传输的目标地址
                         ulLen);//该传输的数据大小
  uDMAChannelEnable(Uart_DMA_SecChannel);//开始传输
  
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

void UARTISR(void)
{
  unsigned long ulStatus;  
  signed long ucChar;
  unsigned char tep= 0;
  
  ulStatus=uDMAIntStatus();
  uDMAIntClear(ulStatus);
  if ((ulStatus & UDMA_DEF_USBEP1TX_SEC_UART2TX) == UDMA_DEF_USBEP1TX_SEC_UART2TX)        
  {
    while(UARTBusy(DEF_UART_BASE));
    g_stSysInf.ucUartBusyFlag = 0;
  }  
  
  ulStatus = UARTIntStatus(DEF_UART_BASE, true);                
  UARTIntClear(DEF_UART_BASE, ulStatus); 
  if((ulStatus & UART_INT_RX) || (ulStatus & UART_INT_RT)) 
  {
    UART_RX_Flag = 1; 
    while(UARTCharsAvail(DEF_UART_BASE))//空不空
    {
      ucChar = UARTCharGet(DEF_UART_BASE);
      if(ucChar != -1)
      {    
        UART_RX_Timer = 0;
        tep = (unsigned char)ucChar;
        if(UART_RX_STATE == 0)
        {
          if(0xda == tep)
          {
            UART_RX_STATE = 1;
          }
        }
        else if(UART_RX_STATE == 1)
        {
          if(0x90 == tep)
          {
            UART_RX_STATE = 2;
          }
          else
          {
            UART_RX_STATE = 0;
          }                    
        }
        else if(UART_RX_STATE == 2)
        {
          if(0xda == tep)
          {
            UART_RX_STATE = 3;
          }
          else
          {
            UART_RX_STATE = 0;
          }
        }
        else if(UART_RX_STATE == 3)
        {
          if(0x90 == tep)
          {
            UART_RX_STATE = 4;
            g_stSysInf.usUartRxLen = 0;
            memset(g_stSysInf.ucUartRxBuf,0x00,1024);
          }
          else
          {
            UART_RX_STATE = 0;
          }
        }
        else if(UART_RX_STATE == 4)
        {
          g_stSysInf.ucUartRxBuf[g_stSysInf.usUartRxLen++] = tep;
          
          if(g_stSysInf.usUartRxLen >= 3)
          {
            ulUartRxRealLen = g_stSysInf.ucUartRxBuf[2];
            ulUartRxRealLen |= g_stSysInf.ucUartRxBuf[1] << 8;
          }
          
          if(g_stSysInf.usUartRxLen >= ulUartRxRealLen + 3)
          {
            UART_RX_STATE = 0;
            CacheInputProc(&g_stCacheProc,&g_stSysInf,g_stSysInf.usUartRxLen,2);
          }
        }
      }
    }
  }
}
