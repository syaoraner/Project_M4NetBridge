#include "config.h"
unsigned char flag_c2000 = 0;
unsigned char TCP_Rec_PLC = 0;


void TCP_Relink(void)
{
  unsigned char ucLinkState = 1;
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
      ucLinkState=0;
      for(ucReadCnt=0;ucReadCnt<10;ucReadCnt++)
      {
        if(ucStateBuf[ucReadCnt])
        {
          ucLinkState++;
        }
      }
      if(ucLinkState <= 3)
      {
        ucLinkState=0;
      }
      else
      {
        ucLinkState = 0x01;
      }
      ucReadCnt=0;
    }
  }
  
  if(ucLinkState == 0x01)
  {
    TCP_MCenter_Relink(g_stSysInf.ucMCReLinkFlag);
    TCP_PLC_Relink(g_stSysInf.ucPLCReLinkFlag);
  }
  else
  {
    TCP_MCenter_Relink(1);
    TCP_PLC_Relink(1);
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

void TCP_PLC_Relink(unsigned char ucRelinkFlag)
{
  if(1 == ucRelinkFlag)
  {
    g_stSysInf.ucPLCReLinkFlag = 0;
    g_stSysInf.ucTCPPLCConFlag = 0;
    wdogFeed( fw_flag );
    IntMasterDisable();     
    TCP_CliePLC_Close();
    IntMasterEnable();  
    
    wdogFeed( fw_flag );
    SysCtlDelay(g_stSysInf.ulSysClock/30);//delay 100ms
    wdogFeed( fw_flag );
    SysCtlDelay(g_stSysInf.ulSysClock/30);//delay 100ms
    
    wdogFeed( fw_flag );
    IntMasterDisable();
    TCP_PLCClient_Init();
    IntMasterEnable();
  }
}


