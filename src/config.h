#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/emac.h"
#include "driverlib/adc.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"//与操作GPIO有关
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"//与系统配置有关
#include "driverlib/systick.h"
#include "driverlib/watchdog.h" //与看门狗操作有关
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "driverlib/timer.h"
#include "driverlib/udma.h"
#include "inc/hw_emac.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "inc/hw_watchdog.h"
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "lwip/err.h"



#include "SysInit.h"
#include "Relink.h"
#include "CmdProc.h"
#include "PostBack.h"
#include "httpweb.h"
#include "UARTFun.h"
#include "GPIOConf.h"
#include "TCPProc.h"



#define DEF_UART_BASE             UART3_BASE
#define DEF_SYSCTL_PERIPH_UART    SYSCTL_PERIPH_UART2
#define DEF_GPIO_PORT_BASE        GPIO_PORTJ_BASE
#define DEF_UART_GPIO_PIN         GPIO_PIN_4| GPIO_PIN_5
#define DEF_INT_UART              INT_UART2			

#define	RS485_GPIO_PORT	    GPIO_PORTN_BASE	
#define RS485_RE_DE         GPIO_PIN_2

#define FLAG_SYSTICK            	0
#define LOGIN_WEB_TICK    				25000

#define Uart_DMA_Channel        UDMA_CH1_UART2TX
#define Uart_DMA_SecChannel     UDMA_SEC_CHANNEL_UART2TX_1
/***********Functions**************/
///watchDog
extern void watchDogInit(unsigned long ulValue);
extern void watchDogFeed(void);
extern void wdogFeed( unsigned char flag );
extern void TCP_Relink(void);
extern unsigned char checkmacpwd(unsigned char *mac, unsigned char *pwd);
extern void FlashEEPROM_ReadN(unsigned short usAddr, unsigned char *pcData, unsigned short usN, unsigned char wz);
///UDP
extern void UPS_UDP_Init(void);
extern void UDP_JHJ_Receive(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                     struct ip_addr *addr, u16_t port);

extern void CacheInputProc(StCacheProc *ucDstAddr,StSysInf *ucSrcAddr,
                    unsigned short usCopyLen,unsigned char ucDataType);

/***********Variables*************/

extern StSysInf g_stSysInf;
extern StCacheProc g_stCacheProc;
extern err_t net_err;
extern unsigned long BaudRate_sz[];
extern unsigned int logintick;
extern unsigned int loginipaddr;
extern unsigned char IsNetInit;
extern unsigned short Neterrcnt;
extern unsigned char link_fail;
extern unsigned short PLCNeterrcnt;
extern unsigned char TCP_Rec_PLC_fail;
extern unsigned char PLCIsNetInit;
extern unsigned char ackBufTag;
extern unsigned short UART_Timer;
extern unsigned char fw_flag;
extern unsigned char TCP_js_flag;
extern unsigned char link_state;
extern unsigned char flag_c2000;
extern unsigned char TCP_Rec_PLC;
extern unsigned char g_ucHaveCMD;
extern unsigned char Mcenter_MODEL;
extern unsigned char PLC_MODEL;
extern unsigned char PLC_cmd_right;
extern unsigned char Mcenter_cmd_right;
extern unsigned char UART_BUSY_flag;
extern unsigned short TCP_RX_length;
extern unsigned char UART_TX_temp[1024];
extern unsigned char UART_RX_STATE;
extern unsigned long UART_Mcenter_Count;
extern unsigned char UART_Mcenter_temp[1024];
extern unsigned int uiShRecLen;
extern unsigned char uartMcenterEmpty;
extern unsigned short Ser_RX_len; 
extern unsigned char UART_PLC_right;
extern unsigned char UART_PLC_temp[1024];
extern unsigned long UART_PLC_Count;
extern unsigned char g_ucWebRestart;
extern unsigned char g_ucUDPNetRestart;
extern unsigned char g_ucNetReset;
extern unsigned char UART_RX_Flag;
extern volatile unsigned long UART_RX_Timer;
extern unsigned char TCP_Return_Flag;
extern unsigned long ulUartRxRealLen;
#endif
