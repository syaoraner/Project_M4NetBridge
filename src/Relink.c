#include "config.h"

//-------------------------------------------------------------------------------
//定时监测网口状态
//如果网口连接正常，则定时监测网络连接状态，一定时间网络连接状态为COLOSE或者WAITE则进行重连；
//如果网口连接异常，则直接重连；
//-------------------------------------------------------------------------------
void TCP_Relink(void)
{
  unsigned char ucLineConnetcState = 1;
  unsigned char ucStateBuf[10] = {0};
  unsigned char ucReadCnt = 0;
  unsigned int i = 0;
  
  if(g_stSysInf.ulHWLinkStateTic >= 10000)
  {
    g_stSysInf.ulHWLinkStateTic = 0;
    for(i=0;i<10;i++)
    {
      ucStateBuf[ucReadCnt++] = (unsigned char)GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0);
    }
    if(ucReadCnt >=10)
    {
      ucLineConnetcState=0;
      for(ucReadCnt=0;ucReadCnt<10;ucReadCnt++)
      {
        if(ucStateBuf[ucReadCnt])
        {
          ucLineConnetcState++;
        }
      }
      if(ucLineConnetcState <= 3)
      {
        ucLineConnetcState=0;
      }
      else
      {
        ucLineConnetcState = 0x01;
      }
      ucReadCnt=0;
    }
  }
//  如果网线连接正常，则判断网络连接，如果未连接，则间歇性重连
  if(ucLineConnetcState == 0x01)
  {
    TCP_MCenter_Relink(g_stSysInf.ucMCReLinkFlag);
  }
  else
  {
    TCP_MCenter_Relink(1);
  }
}

void TCP_MCenter_Relink(unsigned char ucRelinkFlag)
{
  if(1 == ucRelinkFlag)
  {
    g_stSysInf.ucMCReLinkFlag = 0;
    g_stSysInf.ucTCPConFlag = 0;
    wdogFeed( fw_flag );
//    IntMasterDisable();
    TCP_MClient_Close();         ///关闭上次连接
//    IntMasterEnable();
    wdogFeed( fw_flag );
    SysCtlDelay(g_stSysInf.ulSysClock/30);//delay 100ms
    wdogFeed( fw_flag );
//    IntMasterDisable();
    TCP_MClient_Init();          ///Client 初始化 
//    IntMasterEnable();
  }
}
