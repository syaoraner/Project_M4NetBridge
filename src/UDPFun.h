#ifndef __UDPFUN_H__
#define __UDPFUN_H__
#include "config.h"

unsigned short netxy(unsigned char *buf,unsigned short xx,unsigned short gs);
void UPS_UDP_Init(void);
void UDP_JHJ_Receive(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                     struct ip_addr *addr, u16_t port);
#endif

