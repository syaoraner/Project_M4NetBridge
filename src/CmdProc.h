#ifndef __CMDPROC_H__
#define __CMDPROC_H__
#include "config.h"
void DataProcess(void);
void TCP_HuiFa_zxz(unsigned short usLen,unsigned char ucFlag);
void CacheInputProc(StCacheProc *ucDstAddr,StSysInf *ucSrcAddr,
                    unsigned short usCopyLen,unsigned char ucDataType);
void ConfInfoReady(void);
#endif
