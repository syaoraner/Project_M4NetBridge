#include "config.h"

void watchDogInit(unsigned long ulValue)
{ 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);  
  WatchdogResetEnable(WATCHDOG0_BASE); 
  WatchdogStallEnable(WATCHDOG0_BASE); 
  WatchdogReloadSet(WATCHDOG0_BASE, ulValue * (g_stSysInf.ulSysClock / 3000)); 
  WatchdogEnable(WATCHDOG0_BASE); 
  WatchdogLock(WATCHDOG0_BASE);
}

void watchDogFeed()
{
  WatchdogUnlock(WATCHDOG0_BASE);
  WatchdogIntClear(WATCHDOG0_BASE);
  WatchdogLock(WATCHDOG0_BASE);
}

void wdogFeed( unsigned char flag )
{	
  if( !flag )
  {
  WatchdogUnlock(WATCHDOG0_BASE); 
  WatchdogIntClear(WATCHDOG0_BASE); 
  WatchdogLock(WATCHDOG0_BASE);
  }  
}