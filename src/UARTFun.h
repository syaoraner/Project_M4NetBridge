#ifndef __UARTFUN_H__
#define __UARTFUN_H__

void UARTInit(void);
void UARTISR(void);
void DMA_SendData(unsigned char *ucBuf,unsigned short ulLen);
void DMA_Init(void);
void uDMAErrorHandler(void);
//void uDMATransferHandler(void);
#endif

