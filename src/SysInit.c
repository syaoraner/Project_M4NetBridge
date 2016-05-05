#include "config.h"
void Init()
{
  g_stSysInf.ulSysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                              SYSCTL_OSC_MAIN|
                                                SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 50000000);
  // �ر����ж�
  IntMasterDisable();                      
  //�򿪿��Ź�,���ÿ��Ź�װ��ֵ
  watchDogInit(1000);
  //MAC��PHYʹ�ܣ���lwip_init()����ʹ�ܣ�������Բ���
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EPHY0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EMAC0);
  //����ָʾ��
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinConfigure(GPIO_PF0_EN0LED0);
  GPIOPinConfigure(GPIO_PF1_EN0LED2);
  GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_0);
  //�ⲿ���Ź�ι�����
  GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_7,
                   GPIO_STRENGTH_8MA,
                   GPIO_PIN_TYPE_STD);
  GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE,GPIO_PIN_7);
  
  //��ʼ��GPIO
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_0,0x00);
  
  GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_0,
                   GPIO_STRENGTH_8MA,
                   GPIO_PIN_TYPE_STD);
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_0);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);    
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);   
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);   
  
  ////��ʼ��StSysInf
  
  
  //��ʼ���������û�����
  netSetInit();
  
  DMA_Init();
  //��ʼ������
  UARTInit();
  //  UARTInit(BaudRate_sz[Baud_k]);   
  tcpLwipInit();
  
  ///TCP��ʼ��
  TCP_MClient_Init();
  
  TCP_PLCClient_Init();
  
  ////UDP��ʼ��
  UPS_UDP_Init(); 
  
  //��ʼ����������
  httpset_init();
  SysCtlDelay(g_stSysInf.ulSysClock/3000);//delay 300ms    
  
  ///����SysTick
  SysTickPeriodSet(g_stSysInf.ulSysClock / 500);   //2ms
  SysTickEnable();
  SysTickIntEnable();
  
  ///ʹ�ܴ����ж�
  IntMasterEnable(); 
}