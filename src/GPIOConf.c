#include "config.h"

void GPIOConf()
{
	/*
	Just for GASP-KZB  V1.3  Port A Configuration
	PA0			:	U0RX
	PA1 		:	U0TX
	PA2     : RTC
	PA3~PA7 :	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);///Enable GPIO 
	GPIOPinWrite(GPIO_PORTA_BASE,0x07,0X03); // 0000 0011
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);    
	GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1); 
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,~GPIO_PIN_2);
	
	/*
	Just for GASP-KZB  V1.3  Port B Configuration
	PB0			:
	PB1 		:
	PB2     : 
	PB3     :
	PB4~PB7 :	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);///Enable GPIO 
	GPIOPinWrite(GPIO_PORTB_BASE,0x0F,0x00); // 0000 0011 
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,0x0F);
	GPIOPinWrite(GPIO_PORTB_BASE,0x0F,0x00);
	
	/*
	Just for GASP-KZB  V1.3  Port C Configuration
	PC0~PC7	:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);///Enable GPIO 
	
	
	/*  
	Just for GASP-KZB  V1.3  Port D Configuration
	PD0~PD3	:	NC
	PD4     :	MCU LED指示灯
	PD5~PD7 :	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);///Enable GPIO 
	GPIOPinWrite(GPIO_PORTD_BASE,0x10,0x00);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_4);
	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_4,~GPIO_PIN_4);

	
	/*
	Just for GASP-KZB  V1.3  Port E Configuration
	PE0~PE7	:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);///Enable GPIO 
	GPIOPinWrite(GPIO_PORTE_BASE,0xFF,0x00);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_6|GPIO_PIN_7);
	
	/*
	Just for GASP-KZB  V1.3  Port F Configuration
	PF0~PF7	:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,0x08);
	/*
	Just for GASP-KZB  V1.3  Port G Configuration
	PG0			:
	PG1 		:
	PG2     : 
	PG3     :
	PG4~PG7 :	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);///Enable GPIO 
	GPIOPinWrite(GPIO_PORTG_BASE,0x0F,0x00); // 0000 0000
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE,0x0F);
	GPIOPinWrite(GPIO_PORTG_BASE,0x0F,0x00);
	
	/*
	Just for GASP-KZB  V1.3  Port H Configuration
	PH0~PH3		:	地址设置端口
	PH4~PH7		:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);///Enable GPIO
	GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);//地址设置端口PH0、1、2、3
	
	/*
	Just for GASP-KZB  V1.3  Port J Configuration
	PJ0~PJ7		:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);///Enable GPIO 
	
	/*
	Just for GASP-KZB  V1.3  Port K Configuration
	PJ0~PJ7		:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);///Enable GPIO
	
	
	/*
	Just for GASP-KZB  V1.3  Port L Configuration
	PL0~PL7	:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);///??GPIO
	
	
	/*
	Just for GASP-KZB  V1.3  Port M Configuration
	PM0~PM7		:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);///??GPIO
	
	
	/*
	Just for GASP-KZB  V1.3  Port N Configuration
	PN0~PN7		:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);///??GPIO
	
	
	/*
	Just for GASP-KZB  V1.3  Port P Configuration
	PP0~PP7		:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);///??GPIO
	
	
	/*
	Just for GASP-KZB  V1.3  Port Q Configuration
	PQ0~PQ7		:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);///??GPIO
	
	
	/*
	Just for GASP-KZB  V1.3  Port R Configuration
	PR0~PR7		:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOR);///??GPIO
	
	
	/*
	Just for GASP-KZB  V1.3  Port S Configuration
	PS0~PS7		:	NC
	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOS);///??GPIO
	
//	/*
//	Just for GASP-KZB  V1.3  Port T Configuration
//	PT0~PT3		:	NC
//	*/
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOT);///??GPIO
}
