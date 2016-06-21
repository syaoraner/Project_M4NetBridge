#include "config.h"

//-------------------------------------------------------------------------------
//��ʱ�������״̬
//�������������������ʱ�����������״̬��һ��ʱ����������״̬ΪCOLOSE����WAITE�����������
//������������쳣����ֱ��������
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
//  ��������������������ж��������ӣ����δ���ӣ����Ъ������
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
    TCP_MClient_Close();         ///�ر��ϴ�����
//    IntMasterEnable();
    wdogFeed( fw_flag );
    SysCtlDelay(g_stSysInf.ulSysClock/30);//delay 100ms
    wdogFeed( fw_flag );
//    IntMasterDisable();
    TCP_MClient_Init();          ///Client ��ʼ�� 
//    IntMasterEnable();
  }
}
