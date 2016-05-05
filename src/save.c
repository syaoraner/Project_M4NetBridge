#include "inc/hw_types.h"
#include "driverlib/flash.h"
#include "inc/hw_flash.h"
#include "save.h"

#define  FLASH_DEF_SECTION    176        

char FlashEEPROM_Read(unsigned short usAddr, unsigned char wz)
{
    char *pcData;
    pcData = (char *)((FLASH_DEF_SECTION + wz) * 1024 + (usAddr & 0x03FF));
    return(*pcData);
}

 
void FlashEEPROM_ReadN(unsigned short usAddr, unsigned char *pcData, unsigned short usN, unsigned char wz)
{
    unsigned short i;
    if (usN > 0)
    {
        for (i = 0; i < usN; i++)
        {
            pcData[i] = FlashEEPROM_Read(usAddr + i, wz);
        }
    }
}

unsigned char FlashCmp(unsigned char *nsbuf, unsigned short usAddr, unsigned long nslen)
{
    unsigned short i;
    if (nslen > 0)
    {
        for (i = 0; i < nslen; i++)
        {
            if(nsbuf[i] != FlashEEPROM_Read(i, usAddr))
            {
                return 1;
            }
        }
    }
    return 0;
}

unsigned short crcjs1(unsigned char *buf, unsigned short xx, unsigned short gs, unsigned short cc)
{
    unsigned short wCrc = 0xffff;
    unsigned short i, j, k;
    for (i = 0; i < gs; i++)
    {
        j = (unsigned short)buf[xx + i];
        wCrc ^= j;
        for (k = 0; k < 8; k++)
        {
            if ((wCrc & 1) > 0)
            {
                wCrc >>= 1;
                wCrc ^= cc;
            }
            else
            {
                wCrc >>= 1;
            }
        }
    }
    return wCrc;
}

unsigned short crcjs(unsigned char *buf, unsigned short xx, unsigned short gs)
{
  return crcjs1(buf,xx,gs,0xA001);
}

unsigned char Asc2Hex(unsigned char x,unsigned char y)
{
  unsigned char a;
  if(x>='0'&&x<='9')
  {
    a=x-'0';
  }
  if(x>='A'&&x<='F')
  {
    a=x-'7';
  }
  if(x>='a'&&x<='f')
  {
    a= x-'W';
  }
  a<<=4;
  if(y>='0'&&y<='9')
  {
    a+=y-'0';
  }
  if(y>='A'&&y<='F')
  {
    a+=y-'7';
  }
  if(y>='a'&&y<='f')
  {
    a+= y-'W';
  }
  return a;
}

unsigned char checkmacpwd(unsigned char *mac, unsigned char *pwd)
{
  unsigned short x,y,z;
  unsigned char buf[4];

  x=crcjs1(mac,0,6,0xA001);
  y=crcjs1(mac,0,6,0xC002);
  x>>=1;
  x^=y;
  z=crcjs1(mac,0,6,0xB003);
  y>>=1;
  z^=y;
  buf[3]=(unsigned char)(z&0x00ff);
  z>>=8;
  buf[0]=(unsigned char)(z&0x00ff);  
  buf[1]=(unsigned char)(x&0x00ff);
  x>>=7;
  buf[2]=(unsigned char)(x&0x00ff);
  for(x=0;x<4;x++)
  {
    if(Asc2Hex(pwd[x*2],pwd[x*2+1])!=buf[x])
    {
      return 2;
    }
  }
  return 3;
}