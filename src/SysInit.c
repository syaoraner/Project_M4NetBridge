#include "config.h"
void Init()
{
  g_stSysInf.ulSysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                              SYSCTL_OSC_MAIN|
                                                SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 50000000);
  // 关闭总中断
  IntMasterDisable();                      
  //打开看门狗,设置看门狗装载值
  watchDogInit(1000);
  //MAC和PHY使能，在lwip_init()中有使能，这里可以不用
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EPHY0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EMAC0);
  //网口指示灯
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinConfigure(GPIO_PF0_EN0LED0);
  GPIOPinConfigure(GPIO_PF1_EN0LED2);
  GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_0);
  //外部看门狗喂狗输出
  GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_7,
                   GPIO_STRENGTH_8MA,
                   GPIO_PIN_TYPE_STD);
  GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE,GPIO_PIN_7);
  
  //初始化GPIO
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
  
  ////初始化StSysInf
  
  
  //初始化网络设置缓冲区
  netSetInit();
  
  DMA_Init();
  //初始化串口
  UARTInit();
  //  UARTInit(BaudRate_sz[Baud_k]);   
  tcpLwipInit();
  
  ///TCP初始化
  TCP_MClient_Init();
  
  TCP_PLCClient_Init();
  
  ////UDP初始化
  UPS_UDP_Init(); 
  
  //初始化网络配置
  httpset_init();
  SysCtlDelay(g_stSysInf.ulSysClock/3000);//delay 300ms    
  
  ///设置SysTick
  SysTickPeriodSet(g_stSysInf.ulSysClock / 500);   //2ms
  SysTickEnable();
  SysTickIntEnable();
  
  ///使能处理中断
  IntMasterEnable(); 
}