#ifndef __SAVE_H__
#define __SAVE_H__


char FlashEEPROM_Read(unsigned short usAddr, unsigned char wz);
void FlashEEPROM_ReadN(unsigned short usAddr, unsigned char *pcData, unsigned short usN, unsigned char wz);
unsigned short crcjs(unsigned char *buf, unsigned short xx, unsigned short gs);
unsigned char checkmacpwd(unsigned char *mac, unsigned char *pwd);
unsigned char FlashCmp(unsigned char *nsbuf, unsigned short usAddr, unsigned long nslen);


#endif