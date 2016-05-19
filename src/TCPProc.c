#include "config.h"
err_t net_err = ERR_CONN;
unsigned short Neterrcnt = 0;
unsigned short PLCNeterrcnt = 0;
unsigned char TCP_js_flag = 0;
unsigned char Mcenter_MODEL = 0;
unsigned char PLC_MODEL = 0;
unsigned char PLC_cmd_right = 0;
unsigned char Mcenter_cmd_right = 0;
unsigned short Ser_RX_len = 0; 
unsigned char AC_DC = 0;
void tcpLwipInit()
{
  //读MAC
  g_stSysInf.ucMacAddr[0] = g_stSysInf.ucNetCfgBuf[16];//0x0;//((ulUser0 >>  0) & 0xff);
  g_stSysInf.ucMacAddr[1] = g_stSysInf.ucNetCfgBuf[17];//0x9;//((ulUser0 >>  8) & 0xff);
  g_stSysInf.ucMacAddr[2] = g_stSysInf.ucNetCfgBuf[18];//0xf6;//((ulUser0 >> 16) & 0xff);
  g_stSysInf.ucMacAddr[3] = g_stSysInf.ucNetCfgBuf[19];//0x2;//((ulUser1 >>  0) & 0xff);
  g_stSysInf.ucMacAddr[4] = g_stSysInf.ucNetCfgBuf[20];//0xfc;//((ulUser1 >>  8) & 0xff);
  g_stSysInf.ucMacAddr[5] = g_stSysInf.ucNetCfgBuf[21];//0xa2;//((ulUser1 >> 16) & 0xff);   
  //设置server IP
  IP4_ADDR(&g_stSysInf.stSerMCenAddr,g_stSysInf.ucNetCfgBuf[38],g_stSysInf.ucNetCfgBuf[39],\
    g_stSysInf.ucNetCfgBuf[40],g_stSysInf.ucNetCfgBuf[41]); 
//PLC未使用
//  IP4_ADDR(&g_stSysInf.stSerPLCAddr,g_stSysInf.ucNetCfgBuf[56],g_stSysInf.ucNetCfgBuf[57],\
//    g_stSysInf.ucNetCfgBuf[58],g_stSysInf.ucNetCfgBuf[59]);
  //设置client IP
  IP4_ADDR(&g_stSysInf.stLocalAddr,g_stSysInf.ucNetCfgBuf[23],g_stSysInf.ucNetCfgBuf[24],\
    g_stSysInf.ucNetCfgBuf[25],g_stSysInf.ucNetCfgBuf[26]);
  
  
  //读取网络通讯端口
  g_stSysInf.usSerMCPort	= (unsigned short)g_stSysInf.ucNetCfgBuf[42]<<8 | g_stSysInf.ucNetCfgBuf[43];
//PLC未使用
//  g_stSysInf.usSerPLCPort	= (unsigned short)g_stSysInf.ucNetCfgBuf[60]<<8 | g_stSysInf.ucNetCfgBuf[61];
  g_stSysInf.usLocalPort	= (unsigned short)g_stSysInf.ucNetCfgBuf[36]<<8 | g_stSysInf.ucNetCfgBuf[37];
  
  //初始化lwIP，设置IP地址
  //配置协议栈，
  lwIPInit(g_stSysInf.ulSysClock,g_stSysInf.ucMacAddr, GetNetIPConfig(), GetNetMaskConfig(), GetNetGwConfig(), IPADDR_USE_STATIC); //设置LWIP
  //  LocatorMACAddrSet(g_stSysInf.ucMacAddr);  
}

//-------------------------------------------------------------------------------
//client初始化并重连server
//-------------------------------------------------------------------------------
void TCP_MClient_Init(void)
{
  Neterrcnt = 0;
  link_fail = 0;
  //设置服务器IP
  IP4_ADDR(&g_stSysInf.stSerMCenAddr,g_stSysInf.ucNetCfgBuf[38],g_stSysInf.ucNetCfgBuf[39],g_stSysInf.ucNetCfgBuf[40],g_stSysInf.ucNetCfgBuf[41]);
  g_stSysInf.MCClipcb = NULL;
  g_stSysInf.MCClipcb = tcp_new();                        
  tcp_bind(g_stSysInf.MCClipcb,IP_ADDR_ANY,g_stSysInf.usLocalPort);  ////&svripaddr
  IntPrioritySet(INT_EMAC0, 0x00);
  tcp_setprio(g_stSysInf.MCClipcb, TCP_PRIO_NORMAL);///TCP_PRIO_MAX  
  IsNetInit = 0;
  net_err = tcp_connect(g_stSysInf.MCClipcb,&g_stSysInf.stSerMCenAddr,g_stSysInf.usSerMCPort,TCPMCli_Connected);
}
void TCP_MClient_Close(void)
{
  tcp_abort(g_stSysInf.MCClipcb);
  //tcp_close(g_stSysInf.MCClipcb); //上一句已经memp_free
}
err_t TCPMCli_Connected(void *arg,struct tcp_pcb *pcb,err_t err)
{
  unsigned char req_temp;
//0xfa,0x07,0x16,0x02,0xfa,0x02,0x06,0x05,0x04,0x03,0x02,0x01,0xfa,0xff  C2000控件协议帧头，一共14个字节中间6个字节为MAC
//  上电请求连接时发出
  unsigned char TCP_REQ_buf[14] = {0xfa,0x07,0x16,0x02,0xfa,0x02};
  Neterrcnt = 0;
  IsNetInit = 1;
  Mcenter_MODEL = 1;
  g_stSysInf.ucTCPConFlag = 1;
  //建立连接时指定接收数据时的回调函数
  tcp_recv(pcb,TCPMCli_recv);    
  for(req_temp=0;req_temp<6;req_temp++)
  {
    TCP_REQ_buf[6+req_temp] = g_stSysInf.ucNetCfgBuf[16+req_temp];
  }
  
  TCP_REQ_buf[12]=0xfa;
  TCP_REQ_buf[13]=0xff;
  
  tcp_write(pcb, TCP_REQ_buf, 14, 1);
  //三次握手最后一次客户端回ACK，顺带C2000协议14个字节
  tcp_output(pcb);
  
  return ERR_OK;
}

err_t TCPMCli_recv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err)
{
  unsigned char *pucData;
  unsigned short i = 0; 
  unsigned char TCP_RX_right = 0;
  
  Neterrcnt = 0;  
  wdogFeed( fw_flag );
  if( err == ERR_OK )
  {
    if ( p != NULL )
    {
      pucData = p->payload;
      tcp_recved(pcb, p->tot_len);
      
      if(p->tot_len > 0)        //收到的字节数不为0
      {        
        memset(g_stSysInf.ucTcpRecBuf,0x00,1024);
        g_stSysInf.usTcpRecLen = 0;
        for(i = 0; i < (p->tot_len); i++)
        {
          g_stSysInf.ucTcpRecBuf[g_stSysInf.usTcpRecLen++] = pucData[i];
          if((pucData[i] == 0xfa) && (pucData[i+1] == 0))
          {
            i++;
          }
        } 
        
        if((0xfa == g_stSysInf.ucTcpRecBuf[0]) && (0xfa == g_stSysInf.ucTcpRecBuf[1]))
        {
          flag_c2000 = 1;  
          TCP_js_flag  = 1;
        }
      }
      pbuf_free(p);
      if((0xfa == g_stSysInf.ucTcpRecBuf[0]) && (0x01 == g_stSysInf.ucTcpRecBuf[1]) && (0x01 == g_stSysInf.ucTcpRecBuf[2]))
      {
        TCP_RX_right = 1; 
        TCP_js_flag = 1; 
        //head: 5a 5a 5a 5a 
        if((0xee == g_stSysInf.ucTcpRecBuf[3]) && (0xee == g_stSysInf.ucTcpRecBuf[4]) && (0xee == g_stSysInf.ucTcpRecBuf[5])&& (0xee == g_stSysInf.ucTcpRecBuf[6]))
        {
          TCP_RX_right = 2;
          g_stSysInf.ucComModel = 0;
        }
        else if((0x5e == g_stSysInf.ucTcpRecBuf[3]) && (0x5e == g_stSysInf.ucTcpRecBuf[4]) && (0x5e == g_stSysInf.ucTcpRecBuf[5])&& (0x5e == g_stSysInf.ucTcpRecBuf[6]))
        {
          TCP_RX_right = 2;
          g_stSysInf.ucComModel = 1;
        }
        else
        {
          TCP_RX_right = 2;
          g_stSysInf.ucComModel = 2;
        }
      }
      else  
      {
        TCP_RX_right = 0;
      }
      
      if((TCP_RX_right == 2) && (g_stSysInf.ucComModel == 2))
      {
        CacheInputProc(&g_stCacheProc,&g_stSysInf,g_stSysInf.usTcpRecLen,0);
      }     
    }
    else
    {
    }
  }
  return ERR_OK;
}