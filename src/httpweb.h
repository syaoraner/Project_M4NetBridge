#ifndef __HTTPWEB_H__
#define __HTTPWEB_H__

struct realdata{
  long adval[16];//0-9V1 1-5V1 2-A/D 
  long wd_cpu;   //内部cpu温度
  long wd_18b20; //环境18B20温度
  unsigned char io_i[16];//0-防盗1 1-防盗2 2-S4 3-S5 
  unsigned char io_o[16];//0-继电器1 1-继电器2 2-风扇 3-ON 4-OFF
  unsigned char io_s[16];//手动控制输出
};

void httpset_init(void);//初始化WEB服务器
void netSetInit(void);//初始化网络设置缓冲区
unsigned long GetNetIPConfig();//获取IP地址
unsigned long GetNetMaskConfig();//获取掩码
unsigned long GetNetGwConfig();//获取网关

#endif