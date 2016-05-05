#include "config.h"

struct udp_pcb *UdpPcb;
struct udp_pcb *UpsUdpPcb;


///extern unsigned char fw_flag;
extern volatile unsigned char PROTECT;
//*****************************************************************************
//
//! Handles the C2000 jiaoyan.
//
//*****************************************************************************
unsigned short netxy(unsigned char *buf,unsigned short xx,unsigned short gs)
{
    //            #region
    unsigned char bl, dl;
    unsigned short i;
    unsigned short eax = 0;
    for (i = 0; i < gs; i++)
    {
        bl = buf[xx + i];
        dl = 0x80;
        while (dl > 0)
        {
            if ((eax & 0x8000) > 0)
            {
                eax += eax;
                eax ^= 0x1021;
            }
            else
            {
                eax += eax;
            }
            if ((bl & dl) > 0)
            {
                eax ^= 0x1021;
            }
            dl >>= 1;
        }
    }
    return eax;
    //            #endregion
}


void UPS_UDP_Init(void)
{
    UpsUdpPcb = udp_new();
    //  udp_bind(UpsUdpPcb,IP_ADDR_ANY,7373);  
    udp_bind(UpsUdpPcb,IP_ADDR_ANY,21678);   /* 绑定本地地址&svripaddr */
    ///  UDP_Connect_RX = udp_connect(UpsUdpPcb,&svripaddr,21677);    /* 连接远程主机 */
    udp_recv(UpsUdpPcb,UDP_JHJ_Receive,NULL);     
}


//*****************************************************************************
//Handles the UDP recv
/******* 这是一个回调函数，当有UDP数据收到时会被调用********/
//*****************************************************************************
unsigned char UDP_Setting_Flag = 0;  //UDP设置参数标志
unsigned char udp_tx_buf[1500]={0xfa,0x01,0x34,0x33,0x21,0x56,0x23,0xa5,0x7b,0x29,0xc5,0x5d,0x3c,0x32,0x12,0xfe,0x00,0x00};
unsigned char UDP_RXD_databuf[1500];
volatile unsigned char UDP_RX_right = 0;
unsigned char *udp_data=udp_tx_buf;
unsigned int C2000_leijia;
struct pbuf *udp_txp;

unsigned char l_ucUDPNetcfgset[76] = {0};
unsigned char g_ucUDPNetRestart = 0;
unsigned char g_ucUDPNeedRestart = 0;
unsigned char g_ucNetReset = 0;
void UDP_JHJ_Receive(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                     struct ip_addr *addr, u16_t port)
{
    unsigned char *p_udprxd;
    unsigned char UDP_RX_State = 0;
    unsigned char UDP_Setting_State = 0;
    unsigned short i;
    unsigned int length = 0;
    unsigned int setting_length = 0;
    unsigned long UDP_Serial_Set_Baudrate;
    unsigned long Serial_Baudrate;
    //  IntMasterDisable();
    
    
    if(p != NULL)              /* 如果收到的数据不为空   */
    {
        p_udprxd = p->payload;
        length = p->tot_len;///    
        
        for(i=0;i<p->tot_len;i++)
        {//接收全部数据
            UDP_RXD_databuf[i] = p_udprxd[i];
        } 
        pbuf_free(p);
        ////
        for(i=19;i<50;i++)
        {
            udp_tx_buf[i] = 0;
        }
        for(i=0;i<18;i++)
        {
            udp_tx_buf[i] = UDP_RXD_databuf[i];
        }
        if(UDP_RX_State == 0)
        {
            if((UDP_RXD_databuf[0] == 0xfa)&&(UDP_RXD_databuf[1] == 0x01))
            {
                UDP_RX_State = 1;
            }  
        }
        if(UDP_RX_State == 1)
        {     
            if(length == 30)     //IP搜索命令长度
            {
                UDP_RX_State = 2;
            }
            else if(length == 434)
            {
                UDP_Setting_State = 1;
            }
            else
            {
                UDP_RX_State = 0;
                UDP_Setting_State = 0;
            }
        }
        if(UDP_RX_State == 2)
        {       
            if((UDP_RXD_databuf[18] == 0xff)&&(UDP_RXD_databuf[19] == 0x01))
            {
                UDP_RX_right = 1;          
                
                udp_tx_buf[18] = 0xff;
                udp_tx_buf[19] = 0x02;
                udp_tx_buf[20] = g_stSysInf.ucNetCfgBuf[16];//mac[0]
                udp_tx_buf[21] = g_stSysInf.ucNetCfgBuf[17];
                udp_tx_buf[22] = g_stSysInf.ucNetCfgBuf[18];
                udp_tx_buf[23] = g_stSysInf.ucNetCfgBuf[19];
                udp_tx_buf[24] = g_stSysInf.ucNetCfgBuf[20];
                udp_tx_buf[25] = g_stSysInf.ucNetCfgBuf[21];
                udp_tx_buf[26] = 0x00;
                udp_tx_buf[27] = 0x0c;
                udp_tx_buf[28] = g_stSysInf.ucNetCfgBuf[16];//mac[0]
                udp_tx_buf[29] = g_stSysInf.ucNetCfgBuf[17];
                udp_tx_buf[30] = g_stSysInf.ucNetCfgBuf[18];
                udp_tx_buf[31] = g_stSysInf.ucNetCfgBuf[19];
                udp_tx_buf[32] = g_stSysInf.ucNetCfgBuf[20];
                udp_tx_buf[33] = g_stSysInf.ucNetCfgBuf[21];
                udp_tx_buf[34] = g_stSysInf.ucNetCfgBuf[23];//IP
                udp_tx_buf[35] = g_stSysInf.ucNetCfgBuf[24];//
                udp_tx_buf[36] = g_stSysInf.ucNetCfgBuf[25];//
                udp_tx_buf[37] = g_stSysInf.ucNetCfgBuf[26];//
                udp_tx_buf[38] = 0x02;
                udp_tx_buf[39] = 0x16; 
                
                udp_data = udp_tx_buf;
                //C2000 jiaoyan
                C2000_leijia = netxy(udp_data,0,40);  // udp_data       
                udp_tx_buf[40] = (unsigned char)(C2000_leijia & 0xff);
                C2000_leijia >>= 8;
                udp_tx_buf[41] = (unsigned char)(C2000_leijia & 0xff);
                
                udp_txp = pbuf_alloc(PBUF_TRANSPORT,42,PBUF_RAM);   //sizeof(udp_tx_buf)
                
                udp_data = udp_txp->payload;   //UDP_TXD_databuf=(void *)udp_tx_buf;
                for(i = 0;i < 42; i++)
                {
                    udp_data[i] = udp_tx_buf[i];
                }
                
                //        udp_send(UpsUdpPcb,udp_txp); 
                udp_sendto(UpsUdpPcb, udp_txp,IP_ADDR_BROADCAST, 21677);///&svripaddr
                //       SysCtlDelay(SysCtlClockGet()/3000);//delay 1ms  
                pbuf_free(udp_txp);                       /* 释放缓冲区数据 */
                
                UDP_RX_State = 0;
                //        SysCtlDelay(SysCtlClockGet()/3000);//delay 1ms          
            } ///UDP_RXD_databuf[19] == 0x01
            else if((UDP_RXD_databuf[18] == 0xff)&&(UDP_RXD_databuf[19] == 0x03))
            {
                ////20130506  add   command
                ///MAC
                if((UDP_RXD_databuf[20] == g_stSysInf.ucNetCfgBuf[16])&&
                   (UDP_RXD_databuf[21] == g_stSysInf.ucNetCfgBuf[17])&&
                       (UDP_RXD_databuf[22] == g_stSysInf.ucNetCfgBuf[18])&&
                           (UDP_RXD_databuf[23] == g_stSysInf.ucNetCfgBuf[19])&&
                               (UDP_RXD_databuf[24] == g_stSysInf.ucNetCfgBuf[20])&&
                                   (UDP_RXD_databuf[25] == g_stSysInf.ucNetCfgBuf[21]))
                {
                    udp_tx_buf[18] = 0xff;
                    udp_tx_buf[19] = 0x04;
                    udp_tx_buf[20] = g_stSysInf.ucNetCfgBuf[16];//mac[0]
                    udp_tx_buf[21] = g_stSysInf.ucNetCfgBuf[17];
                    udp_tx_buf[22] = g_stSysInf.ucNetCfgBuf[18];
                    udp_tx_buf[23] = g_stSysInf.ucNetCfgBuf[19];
                    udp_tx_buf[24] = g_stSysInf.ucNetCfgBuf[20];
                    udp_tx_buf[25] = g_stSysInf.ucNetCfgBuf[21];   
                    
                    udp_tx_buf[26] = 0x01;
                    udp_tx_buf[27] = 0x94;
                    
                    udp_tx_buf[28] = 0;//set
                    udp_tx_buf[29] = 0;
                    udp_tx_buf[30] = 0;
                    udp_tx_buf[31] = g_stSysInf.ucNetCfgBuf[22];
                    
                    udp_tx_buf[32] = g_stSysInf.ucNetCfgBuf[23];//IP
                    udp_tx_buf[33] = g_stSysInf.ucNetCfgBuf[24];
                    udp_tx_buf[34] = g_stSysInf.ucNetCfgBuf[25];
                    udp_tx_buf[35] = g_stSysInf.ucNetCfgBuf[26];//
                    
                    udp_tx_buf[36] = g_stSysInf.ucNetCfgBuf[27];//子网掩码
                    udp_tx_buf[37] = g_stSysInf.ucNetCfgBuf[28];//
                    udp_tx_buf[38] = g_stSysInf.ucNetCfgBuf[29];
                    udp_tx_buf[39] = g_stSysInf.ucNetCfgBuf[30];   
                    
                    udp_tx_buf[40] = g_stSysInf.ucNetCfgBuf[31];//unkown
                    udp_tx_buf[41] = g_stSysInf.ucNetCfgBuf[32];
                    udp_tx_buf[42] = g_stSysInf.ucNetCfgBuf[33];
                    udp_tx_buf[43] = g_stSysInf.ucNetCfgBuf[34];// 
                    
                    udp_tx_buf[44] = 0x09;//unkown
                    udp_tx_buf[45] = 0x01;
                    udp_tx_buf[46] = 0x0;
                    udp_tx_buf[47] = 0x06;// 
                    
                    udp_tx_buf[51] =g_stSysInf.ucNetCfgBuf[35];
                    ///设备端口  
                    udp_tx_buf[54] = g_stSysInf.ucNetCfgBuf[36];//7300
                    udp_tx_buf[55] = g_stSysInf.ucNetCfgBuf[37]; 
                    //服务器IP
                    udp_tx_buf[56] = g_stSysInf.ucNetCfgBuf[38];
                    udp_tx_buf[57] = g_stSysInf.ucNetCfgBuf[39];
                    udp_tx_buf[58] = g_stSysInf.ucNetCfgBuf[40];
                    udp_tx_buf[59] = g_stSysInf.ucNetCfgBuf[41];//      
                    //服务端口
                    udp_tx_buf[62] = g_stSysInf.ucNetCfgBuf[42];//7300
                    udp_tx_buf[63] = g_stSysInf.ucNetCfgBuf[43];    
                    ////虚拟控件        
                    udp_tx_buf[67] = 0;
                    ///波特率
                    Serial_Baudrate = BaudRate_sz[g_stSysInf.ucNetCfgBuf[45]];     
                    udp_tx_buf[211] = (unsigned char)Serial_Baudrate;
                    Serial_Baudrate >>= 8;
                    udp_tx_buf[210] = (unsigned char)Serial_Baudrate;  
                    Serial_Baudrate >>= 8;
                    udp_tx_buf[209] = (unsigned char)Serial_Baudrate;
                    Serial_Baudrate >>= 8;
                    udp_tx_buf[208] = (unsigned char)Serial_Baudrate;      
                    
                    udp_tx_buf[215] = g_stSysInf.ucNetCfgBuf[46];
                    udp_tx_buf[219] = g_stSysInf.ucNetCfgBuf[47];
                    udp_tx_buf[223] = g_stSysInf.ucNetCfgBuf[48];
                    udp_tx_buf[227] = g_stSysInf.ucNetCfgBuf[49];
                    
                    udp_tx_buf[230] = g_stSysInf.ucNetCfgBuf[50];
                    udp_tx_buf[231] = g_stSysInf.ucNetCfgBuf[51];
                    
                    udp_tx_buf[234] = g_stSysInf.ucNetCfgBuf[52];
                    udp_tx_buf[235] = g_stSysInf.ucNetCfgBuf[53]; 
                    udp_data = udp_tx_buf;
                    //C2000 jiaoyan
                    C2000_leijia = netxy(udp_data,0,432);  // udp_data       
                    udp_tx_buf[432] = (unsigned char)(C2000_leijia & 0xff);
                    C2000_leijia >>= 8;
                    udp_tx_buf[433] = (unsigned char)(C2000_leijia & 0xff);
                    
                    udp_txp = pbuf_alloc(PBUF_TRANSPORT,434,PBUF_RAM);   //sizeof(udp_tx_buf)
                    
                    udp_data = udp_txp->payload;   //UDP_TXD_databuf=(void *)udp_tx_buf;
                    for(i = 0;i < 434; i++)
                    {
                        udp_data[i] = udp_tx_buf[i];
                    }
                    //        udp_send(UpsUdpPcb,udp_txp); 
                    udp_sendto(UpsUdpPcb, udp_txp,IP_ADDR_BROADCAST, 21677);//&svripaddr
                    //     SysCtlDelay(SysCtlClockGet()/3000);//delay 1ms  
                    pbuf_free(udp_txp);                       /* 释放缓冲区数据 */
                    
                    UDP_RX_State = 0;
                    //  SysCtlDelay(SysCtlClockGet()/3000);//delay 1ms     
                }
            }///UDP_RXD_databuf[19] == 0x03
            else if((UDP_RXD_databuf[18] == 0xff)&&(UDP_RXD_databuf[19] == 0x07))
            {
                udp_tx_buf[18] = 0xff;
                udp_tx_buf[19] = 0x08;        
                udp_tx_buf[20] = g_stSysInf.ucNetCfgBuf[16];//mac[0]
                udp_tx_buf[21] = g_stSysInf.ucNetCfgBuf[17];
                udp_tx_buf[22] = g_stSysInf.ucNetCfgBuf[18];
                udp_tx_buf[23] = g_stSysInf.ucNetCfgBuf[19];
                udp_tx_buf[24] = g_stSysInf.ucNetCfgBuf[20];
                udp_tx_buf[25] = g_stSysInf.ucNetCfgBuf[21]; 
                udp_tx_buf[26] = 0;
                udp_tx_buf[27] = 0x04;
                udp_tx_buf[31] = UDP_Setting_Flag;
                udp_data = udp_tx_buf;
                //C2000 jiaoyan
                C2000_leijia = netxy(udp_data,0,32);  // udp_data       
                udp_tx_buf[32] = (unsigned char)(C2000_leijia & 0xff);
                C2000_leijia >>= 8;
                udp_tx_buf[33] = (unsigned char)(C2000_leijia & 0xff);
                
                udp_txp = pbuf_alloc(PBUF_TRANSPORT,34,PBUF_RAM);   //sizeof(udp_tx_buf)
                
                udp_data = udp_txp->payload;   //UDP_TXD_databuf=(void *)udp_tx_buf;
                for(i = 0;i < 34; i++)
                {
                    udp_data[i] = udp_tx_buf[i];
                }
                //        udp_send(UpsUdpPcb,udp_txp); 
                udp_sendto(UpsUdpPcb, udp_txp,IP_ADDR_BROADCAST , 21677);///&svripaddr
                //        SysCtlDelay(SysCtlClockGet()/3000);//delay 1ms  
                pbuf_free(udp_txp);                       /* 释放缓冲区数据 */
                
                UDP_RX_State = 0;
                //        SysCtlDelay(SysCtlClockGet()/3000);//delay 1ms         
            }
            else if((UDP_RXD_databuf[18] == 0xff)&&(UDP_RXD_databuf[19] == 0x0b))
            {
                ////20120507  add  reset command
                ///MAC
                if((UDP_RXD_databuf[20] == g_stSysInf.ucNetCfgBuf[16])&&
                   (UDP_RXD_databuf[21] == g_stSysInf.ucNetCfgBuf[17])&&
                       (UDP_RXD_databuf[22] == g_stSysInf.ucNetCfgBuf[18])&&
                           (UDP_RXD_databuf[23] == g_stSysInf.ucNetCfgBuf[19])&&
                               (UDP_RXD_databuf[24] == g_stSysInf.ucNetCfgBuf[20])&&
                                   (UDP_RXD_databuf[25] == g_stSysInf.ucNetCfgBuf[21]))
                {
                    udp_tx_buf[18]  = 0xff;
                    udp_tx_buf[19]  = 0x0c;        
                    udp_tx_buf[20] = g_stSysInf.ucNetCfgBuf[16];//mac[0]
                    udp_tx_buf[21] = g_stSysInf.ucNetCfgBuf[17];
                    udp_tx_buf[22] = g_stSysInf.ucNetCfgBuf[18];
                    udp_tx_buf[23] = g_stSysInf.ucNetCfgBuf[19];
                    udp_tx_buf[24] = g_stSysInf.ucNetCfgBuf[20];
                    udp_tx_buf[25] = g_stSysInf.ucNetCfgBuf[21]; 
                    udp_tx_buf[26]  = 0;
                    udp_tx_buf[27]  = 0x04;
                    udp_tx_buf[28]  = 0;
                    udp_tx_buf[29]  = 0;
                    udp_tx_buf[30]  = 0;
                    udp_tx_buf[31]  = 1;
                    
                    //C2000 jiaoyan
                    C2000_leijia = netxy(udp_data,0,32);  // udp_data       
                    udp_tx_buf[32] = (unsigned char)(C2000_leijia & 0xff);
                    C2000_leijia >>= 8;
                    udp_tx_buf[33] = (unsigned char)(C2000_leijia & 0xff);
                    
                    udp_txp = pbuf_alloc(PBUF_TRANSPORT,34,PBUF_RAM);   //sizeof(udp_tx_buf)
                    
                    udp_data = udp_txp->payload;   //UDP_TXD_databuf=(void *)udp_tx_buf;
                    for(i = 0;i < 34; i++)
                    {
                        udp_data[i] = udp_tx_buf[i];
                    }
                    
                    udp_sendto(UpsUdpPcb, udp_txp,IP_ADDR_BROADCAST , 21677);///&svripaddr 
                    
                    g_ucNetReset = 1;
                    ////////////////                    fw_flag = 1; 
                }
            }
        }//UDP_RX_State == 2
        else if(UDP_Setting_State == 1)
        {
            PROTECT = GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_3);
            if(UDP_RXD_databuf[19] == 0x05)   // &&(PROTECT == 0)
            { 
                ///MAC
                if((UDP_RXD_databuf[20] == g_stSysInf.ucNetCfgBuf[16])&&
                   (UDP_RXD_databuf[21] == g_stSysInf.ucNetCfgBuf[17])&&
                       (UDP_RXD_databuf[22] == g_stSysInf.ucNetCfgBuf[18])&&
                           (UDP_RXD_databuf[23] == g_stSysInf.ucNetCfgBuf[19])&&
                               (UDP_RXD_databuf[24] == g_stSysInf.ucNetCfgBuf[20])&&
                                   (UDP_RXD_databuf[25] == g_stSysInf.ucNetCfgBuf[21]))
                {
                    //length
                    setting_length = UDP_RXD_databuf[26];
                    setting_length <<= 8;
                    setting_length |= UDP_RXD_databuf[27];
                    if(setting_length == (length-30))
                    {
                        for(i=0;i<76;i++)
                        {
                            l_ucUDPNetcfgset[i] = 0;
                        }
                        for(i=22;i<35;i++)
                        {
                            l_ucUDPNetcfgset[i] = UDP_RXD_databuf[i+9];
                        }
                        
                        
                        ///通讯模式   0-Client，1-Server，2-UDP
                        l_ucUDPNetcfgset[35] = UDP_RXD_databuf[51];
                        ///设备端口  
                        l_ucUDPNetcfgset[36] = UDP_RXD_databuf[54];//7300
                        l_ucUDPNetcfgset[37] = UDP_RXD_databuf[55] ;
                        //服务器IP
                        l_ucUDPNetcfgset[38] = UDP_RXD_databuf[56];
                        l_ucUDPNetcfgset[39] = UDP_RXD_databuf[57];
                        l_ucUDPNetcfgset[40] = UDP_RXD_databuf[58];
                        l_ucUDPNetcfgset[41] = UDP_RXD_databuf[59];//           
                        //服务端口
                        l_ucUDPNetcfgset[42] = UDP_RXD_databuf[62];//7300
                        l_ucUDPNetcfgset[43] = UDP_RXD_databuf[63];    
                        ///是否启用C2000控件，0-启用，1-不启用
                        l_ucUDPNetcfgset[44] = UDP_RXD_databuf[67]; 
                        
                        l_ucUDPNetcfgset[49] = UDP_RXD_databuf[227];
                        l_ucUDPNetcfgset[51] = UDP_RXD_databuf[231];
                        l_ucUDPNetcfgset[52] = UDP_RXD_databuf[234];
                        l_ucUDPNetcfgset[53] = UDP_RXD_databuf[235];
                        
                        
                        //serial setting
                        //baud rate
                        UDP_Serial_Set_Baudrate = UDP_RXD_databuf[208];
                        UDP_Serial_Set_Baudrate <<= 8;
                        UDP_Serial_Set_Baudrate += UDP_RXD_databuf[209];
                        UDP_Serial_Set_Baudrate <<= 8;
                        UDP_Serial_Set_Baudrate += UDP_RXD_databuf[210];
                        UDP_Serial_Set_Baudrate <<= 8;
                        UDP_Serial_Set_Baudrate += UDP_RXD_databuf[211];                
                        if(UDP_Serial_Set_Baudrate == 1200)
                        {
                            l_ucUDPNetcfgset[45] = 0;
                        }
                        else if(UDP_Serial_Set_Baudrate == 2400)
                        {
                            l_ucUDPNetcfgset[45] = 1;
                        }
                        else if(UDP_Serial_Set_Baudrate == 4800)
                        {
                            l_ucUDPNetcfgset[45] = 2;
                        }  
                        else if(UDP_Serial_Set_Baudrate == 9600)
                        {
                            l_ucUDPNetcfgset[45] = 3;
                        }   
                        else if(UDP_Serial_Set_Baudrate == 19200)
                        {
                            l_ucUDPNetcfgset[45] = 4;
                        }     
                        else if(UDP_Serial_Set_Baudrate == 38400)
                        {
                            l_ucUDPNetcfgset[45] = 5;
                        } 
                        else if(UDP_Serial_Set_Baudrate == 57600)
                        {
                            l_ucUDPNetcfgset[45] = 6;
                        }   
                        else if(UDP_Serial_Set_Baudrate == 115200)
                        {
                            l_ucUDPNetcfgset[45] = 7;
                        } 
                        else if(UDP_Serial_Set_Baudrate == 230400)
                        {
                            l_ucUDPNetcfgset[45] = 8;
                        }            
                        else
                        {
                            l_ucUDPNetcfgset[45] = 1;
                        }
                        //data length 
                        l_ucUDPNetcfgset[46] = UDP_RXD_databuf[215];
                        
                        //CheckBit
                        l_ucUDPNetcfgset[47] = UDP_RXD_databuf[219];
                        
                        //stop wei
                        l_ucUDPNetcfgset[48] = UDP_RXD_databuf[223];
                        
                        for(i=22;i<54;i++)
                        {
                            if(l_ucUDPNetcfgset[i] != g_stSysInf.ucNetCfgBuf[i])
                            {
                                //                                UDP_Setting_Flag = 1;
                                g_ucUDPNeedRestart = 1;
                                g_stSysInf.ucNetCfgBuf[i] = l_ucUDPNetcfgset[i];
                            }
                        }
                        UDP_Setting_Flag = 1;
                    }//length
                    else
                    {
                        UDP_Setting_Flag = 0;
                    }
                    
                    if(g_ucUDPNeedRestart == 1)
                    {          
                        g_ucUDPNeedRestart = 0;
                        //                        udp_tx_buf[18] = 0xff;
                        //                        udp_tx_buf[19] = 0x06;  
                        g_ucUDPNetRestart = 1;
                        
                        FlashErase(240<<10);                                    
                        FlashProgram((uint32_t *)g_stSysInf.ucNetCfgBuf, (240<<10), 1024); 
                        
                        //         FM25L256_Write(32300,g_stSysInf.ucNetCfgBuf,64);
                        SysCtlDelay(g_stSysInf.ulSysClock/3000);//delay 1ms  
                    }
                    udp_tx_buf[18] = 0xff;
                    udp_tx_buf[19] = 0x06;  
                    udp_tx_buf[20] = g_stSysInf.ucNetCfgBuf[16];//mac[0]
                    udp_tx_buf[21] = g_stSysInf.ucNetCfgBuf[17];
                    udp_tx_buf[22] = g_stSysInf.ucNetCfgBuf[18];
                    udp_tx_buf[23] = g_stSysInf.ucNetCfgBuf[19];
                    udp_tx_buf[24] = g_stSysInf.ucNetCfgBuf[20];
                    udp_tx_buf[25] = g_stSysInf.ucNetCfgBuf[21]; 
                    udp_tx_buf[26] = 0;
                    udp_tx_buf[27] = 0x04;
                    udp_tx_buf[31] = UDP_Setting_Flag;
                    udp_data = udp_tx_buf;
                    //C2000 jiaoyan
                    C2000_leijia = netxy(udp_data,0,32);  // udp_data       
                    udp_tx_buf[32] = (unsigned char)(C2000_leijia & 0xff);
                    C2000_leijia >>= 8;
                    udp_tx_buf[33] = (unsigned char)(C2000_leijia & 0xff);
                    
                    udp_txp = pbuf_alloc(PBUF_TRANSPORT,34,PBUF_RAM);   //sizeof(udp_tx_buf)
                    
                    udp_data = udp_txp->payload;   //UDP_TXD_databuf=(void *)udp_tx_buf;
                    for(i = 0;i < 34; i++)
                    {
                        udp_data[i] = udp_tx_buf[i];
                    }
                    //        udp_send(UpsUdpPcb,udp_txp); 
                    udp_sendto(UpsUdpPcb, udp_txp,IP_ADDR_BROADCAST , 21677);//&svripaddr
                    //        SysCtlDelay(SysCtlClockGet()/3000);//delay 1ms  
                    pbuf_free(udp_txp);                       /* 释放缓冲区数据 */
                    
                    UDP_RX_State = 0;
                    SysCtlDelay(g_stSysInf.ulSysClock/3);//delay 1s 
                    
                    //        fw_flag = 1;  
                    //                    g_ucUDPNetRestart = 1;
                    //                    SysCtlReset();
                    
                } ///MAC       
            }//if(UDP_RXD_databuf[19] == 0x05)
            else///
            {
                UDP_Setting_Flag = 0;
            }     
            
        }//if(UDP_Setting_State == 1)
        else 
        {
            UDP_Setting_State = 0;
            UDP_RX_State = 0;
            length = 0;
        }
    }  
    //    IntMasterEnable(); 
    //    g_ulSystemTimeMS = 0;
    //    SysCtlDelay(SysCtlClockGet()/3000);//delay 1ms
}


