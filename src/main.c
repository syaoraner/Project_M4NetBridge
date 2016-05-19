#include "config.h"
volatile unsigned char PROTECT = 1; 
static volatile unsigned long g_ulFlags;
unsigned char IsNetInit = 0;
unsigned char link_fail=0;
unsigned char link_state = 0;
unsigned long g_usUDPRestartTiming = 0;
unsigned long g_usWebRestartTiming = 0;
unsigned long g_usNetResetTiming = 0;
volatile unsigned long UART_RX_Timer = 0;
unsigned long g_ulSynSentTime = 0;
unsigned long g_ulReLinkTimer = 0;
unsigned char g_ucExWDFlag = GPIO_PIN_1;
unsigned long g_ulLEDTic = 0;
unsigned char g_ucLEDFlag = GPIO_PIN_0;


StSysInf g_stSysInf;
StCacheProc g_stCacheProc;
void SysTickIntHandler(void)
{
  HWREGBITW(&g_ulFlags, FLAG_SYSTICK) = 1;
  logintick++;
  g_stSysInf.ulHWLinkStateTic++;
  g_ulLEDTic++;
  if(g_ulLEDTic >= 500)
  {
    g_ulLEDTic = 0;
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_0, g_ucLEDFlag);
    g_ucLEDFlag =  ~g_ucLEDFlag;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, g_ucExWDFlag);
    g_ucExWDFlag = ~g_ucExWDFlag;
  }
  /*******MCenter pcb state**********/
#if 1
  if(g_stSysInf.MCClipcb->state == SYN_SENT)
  {
    g_ulSynSentTime++;
  }
  else
  {
    g_ulSynSentTime = 0;
  }
  
  if(g_ulSynSentTime >= 2500)
  {
    g_ulSynSentTime = 0; 
    g_stSysInf.MCClipcb->state = CLOSED;
  }
  
  if((g_stSysInf.MCClipcb->state == CLOSED) || 
     (g_stSysInf.MCClipcb->state == CLOSE_WAIT))
  {
    g_ulReLinkTimer++;
  }
  else
  {
    g_ulReLinkTimer = 0;
  }
  
  if(g_ulReLinkTimer >= 2000)
  {
    g_ulReLinkTimer = 0;
    g_stSysInf.ucMCReLinkFlag = 1;
  }
#endif
   
  if(g_ucWebRestart == 1)
  {
    g_usWebRestartTiming++;
    if(g_usWebRestartTiming >= 2000)
    {
      g_usWebRestartTiming = 0;
      g_ucWebRestart = 0;
      SysCtlReset();
    }
  }
  
  if(g_ucUDPNetRestart == 1)
  {
    g_usUDPRestartTiming++;
    if(g_usUDPRestartTiming >= 1000)
    {
      g_usUDPRestartTiming = 0;
      g_ucUDPNetRestart = 0;
      SysCtlReset();
    }
  }
  
  if(g_ucNetReset == 1 )
  {
    g_usNetResetTiming++;
    if(g_usNetResetTiming >= 1000)
    {
      g_usNetResetTiming = 0;
      g_ucNetReset = 0;
      SysCtlReset();
    }
  }
  
  if(IsNetInit == 0)
  {
    Neterrcnt++;
    if(Neterrcnt >= 7500)
    {      
      link_fail = 1;
      Neterrcnt = 0;
    }
  }
  
  
  if(UART_RX_Flag == 1)
  {
    UART_RX_Timer++;
  }
  
  if(logintick > LOGIN_WEB_TICK)
  {
    loginipaddr = 0;
    logintick = 0;
  }
  //	g_ulSystemTimeMS += 2;
  lwIPTimer(2);
}

void lwIPHostTimerHandler(void)
{
  static unsigned long ulLastIPAddress = 0;
  unsigned long ulIPAddress;
  ulIPAddress = lwIPLocalIPAddrGet();
  if(ulIPAddress == 0)
  {
    ;
  }
  else if(ulLastIPAddress != ulIPAddress)
  {
    ulLastIPAddress = ulIPAddress;
    ulIPAddress = lwIPLocalNetMaskGet();
    ulIPAddress = lwIPLocalGWAddrGet();
  }
}

void main()
{
  unsigned char ucLedBlink=0x01;
  Init();
  while(1)
  {
    ucLedBlink=~ucLedBlink;
    GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_0,ucLedBlink);
    watchDogFeed();    
    TCP_Relink();
    DataProcess();     
  }
}

