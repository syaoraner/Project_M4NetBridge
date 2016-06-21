#include "config.h"

struct tcp_pcb *http_pcb;
unsigned char fw_flag=0;
//unsigned long BaudRate_sz[] = {1200,2400,4800,9600,19200,38400,57600,115200,230400};
extern unsigned long BaudRate_sz[] = {0x000004b0,0x00000960,0x000012c0,0x00002580,0x00004b00,0x00009600,0x0000e100,0x0001c200,0x00038400};
unsigned int loginipaddr;         
unsigned int logintick = 0;             
//struct realdata rdata;

unsigned char g_ucTmpNetcfgbuf[64] = {0};
unsigned char g_ucWebRestart = 0;

/*static const unsigned char http_logo_gif[] =
{
};*/

static const char http_html_hdr[]="HTTP/1.1 200 OK\r\nServer: LWIP/1.4.1\r\nContent-type: text/html\r\n\r\n";

static const char html_login[] =
"<html><head><title>网桥参数配置界面登录</title><style type=\"text/css\"><!--.STYLE1 {color: #FF3300}--></style></head>"
"<body><div align=\"center\">"
"<table height=\"179\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"text-align: left; width: 100%; height: 66px;\">"
"<tbody><tr><td width=\"316\" style=\"vertical-align: top; text-align: left; width: 316px;\">"
//"<a href=\"http://www.cqmas.com\" target=\"_blank\"></a></td>"<img src=\"logo.gif\" border=\"0\" /></a></td>"
"<td width=\"707\" style=\"text-align: right; vertical-align: bottom;\"><span class=\"STYLE1\">"
"M4网桥V1.0</span></td></tr></tbody></table><hr /><p>&nbsp;</p>"
"<table width=\"216\" height=\"75\" border=\"0\"><tr><td></td></tr><tr><td></td></tr></table>"
"<form action=\"login.cgi\" method=\"GET\" name=\"login\"><table width=\"388\" height=\"135\" border=\"0\">"
"<tr><td height=\"23\" colspan=\"3\" bgcolor=\"#BFDFFF\"><div align=\"center\"><B><font color=#000000><big>重庆梅安森科技股份有限公司</big></font></B>"
"</div></td></tr><tr><td height=\"24\" bgcolor=\"#BFDFFF\"><div align=\"right\">密码:</div></td>"
"<td colspan=\"2\"bgcolor=\"#BFDFFF\"><div align=\"center\"><input name=\"pwd\" type=\"text\" size=\"20\" maxlength=\"16\" type=\"password\"/>"
"</div></td></tr><tr><td height=\"55\" colspan=\"3\" bgcolor=\"#BFDFFF\"><div align=\"center\">"
"<input type=\"submit\" name=\"Submit\" value=\"确定\" />&nbsp;&nbsp;"
"<INPUT onclick=\"location.href='repwd.cgi'\" type=button value=修改密码 name=B2 />"
"</div></td></tr>";
static const char html_login_end[]="</table></form></body></html>";
static const char html_login_err[]="<tr><td><font color=#ff0000>您输入的密码错误!</font></td></tr></table></form></body></html>";

static const char html_repwd[] =
"<html><head><title>修改用户登录密码</title><style type=\"text/css\"><!--.STYLE1 {color: #FF3300}--></style></head>"
"<body><div align=\"center\">"
"<table height=\"179\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"text-align: left; width: 100%; height: 66px;\">"
"<tbody><tr><td width=\"316\" style=\"vertical-align: top; text-align: left; width: 316px;\">"
//"<a href=\"http://www.cqmas.com\" target=\"_blank\"><img src=\"logo.gif\" border=\"0\" /></a></td>"
"</tr></tbody></table>"
"<table width=\"216\" height=\"75\" border=\"0\"><tr><td></td></tr><tr><td></td></tr></table>"
"<form action=\"repwd.cgi\" method=\"GET\" name=\"login\"><table width=\"388\" height=\"135\" border=\"0\">"
"<tr><td height=\"23\" colspan=\"3\" bgcolor=\"#BFDFFF\"><div align=\"center\">修改用户登录密码</div></td></tr>"
"<tr><td height=\"24\" bgcolor=\"#FFFFFF\"><div align=\"right\">输入密码:</div></td>"
"<td colspan=\"2\"><div align=\"center\"><input name=\"p1\" type=\"text\" size=\"20\" maxlength=\"16\" type=\"password\"/></div></td></tr>"
"<tr><td height=\"24\" bgcolor=\"#FFFFFF\"><div align=\"right\">输入新密码:</div></td>"
"<td colspan=\"2\"><div align=\"center\"><input name=\"p2\" type=\"text\" size=\"20\" maxlength=\"16\" type=\"password\"/></div></td></tr>"
"<tr><td height=\"24\" bgcolor=\"#FFFFFF\"><div align=\"right\">再输入新密码:</div></td>"
"<td colspan=\"2\"><div align=\"center\"><input name=\"p3\" type=\"text\" size=\"20\" maxlength=\"16\" type=\"password\"/></div></td></tr>"
"<tr><td height=\"55\" colspan=\"3\" bgcolor=\"#FFFFFF\"><div align=\"center\">"
"<input type=\"submit\" name=\"Submit\" value=\"修改\" />&nbsp;&nbsp;"
"<INPUT onclick=\"location.href='login.cgi'\" type=button value=返回 name=B2 />"
"</div></td></tr>";
static const char html_repwd_end[]="</table></form></body></html>";
static const char html_repwd_ok[]="<tr><td><font color=#ff0000>修改密码成功!</font></td></tr></table></form></body></html>";
static const char html_repwd_err1[]="<tr><td><font color=#ff0000>您输入的密码错误!</font></td></tr></table></form></body></html>";
static const char html_repwd_err2[]="<tr><td><font color=#ff0000>您两次输入的新密码不一致!</font></td></tr></table></form></body></html>";

static const char html_macset1[] =
"<html><head><title>MAC地址设置</title><style type=\"text/css\"><!--.STYLE1 {color: #FF3300}--></style></head>"
"<body><div align=\"center\">"
"<table height=\"179\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"text-align: left; width: 100%; height: 66px;\">"
"<tbody><tr><td width=\"316\" style=\"vertical-align: top; text-align: left; width: 316px;\">"
//"<a href=\"http://www.cqmas.com\" target=\"_blank\"><img src=\"logo.gif\" border=\"0\" /></a></td>"
"</tr></tbody></table>"
"<table width=\"216\" height=\"75\" border=\"0\"><tr><td></td></tr><tr><td></td></tr></table>"
"<form action=\"macset.cgi\" method=\"GET\" name=\"login\"><table width=\"388\" height=\"135\" border=\"0\">"
"<tr><td height=\"23\" colspan=\"3\" bgcolor=\"#BFDFFF\"><div align=\"center\">MAC地址设置</div></td></tr>"
"<tr><td height=\"24\" bgcolor=\"#FFFFFF\"><div align=\"right\">MAC地址:</div></td>"
"<td colspan=\"2\"><div align=\"center\">";
static const char html_macset2[] =
"</div></td></tr><tr><td height=\"24\" bgcolor=\"#FFFFFF\"><div align=\"right\">新MAC地址:</div></td>"
"<td colspan=\"2\"><div align=\"center\"><input name=\"mac\" type=\"text\" size=\"20\" maxlength=\"17\" type=\"password\"/></div></td></tr>"
"<tr><td height=\"24\" bgcolor=\"#FFFFFF\"><div align=\"right\">出厂密码:</div></td>"
"<td colspan=\"2\"><div align=\"center\"><input name=\"pwd\" type=\"text\" size=\"20\" maxlength=\"8\" type=\"password\"/></div></td></tr>"
"<tr><td height=\"55\" colspan=\"3\" bgcolor=\"#FFFFFF\"><div align=\"center\">"
"<input type=\"submit\" name=\"Submit\" value=\"修改\" />&nbsp;&nbsp;"
"<INPUT onclick=\"location.href='login.cgi'\" type=button value=返回 name=B2 />"
"</div></td></tr>";
static const char html_macset_end[]="</table></form></body></html>";
static const char html_macset_ok[]="<tr><td><font color=#ff0000>修改MAC地址成功!</font></td></tr></table></form></body></html>";
static const char html_macset_err1[]="<tr><td><font color=#ff0000>您输入的MAC地址错误!</font></td></tr></table></form></body></html>";
static const char html_macset_err2[]="<tr><td><font color=#ff0000>您输入的出厂密码错误!</font></td></tr></table></form></body></html>";

static const char html_netcfgset_ok[]="<tr><td><font color=#ff0000>修改成功!</font></td></tr></table></form></body></html>";


//static const char html_getrd[] =
//"<html><head><title>getrd</title></head>"
//"<script language=\"javascript\">";
//static const char html_getrd1[] =
//"t=window.setInterval(\"a()\",3000);"
//"function a(){window.location.reload();}</script>"
//"<body></body></html>";

static const char html_config[]="<HTML><HEAD><TITLE>网桥网络参数</TITLE>"
"<SCRIPT language=JavaScript>"
"function checkIP(ctext){"
"var re = /[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+/;var re1=/[^0-9.]/;"
"if(re1.test(ctext.value)){alert(\"您输入的参数非法!\");ctext.focus();}"
"else{if(!re.test(ctext.value)){alert(\"您输入的参数非法!\");ctext.focus();}}}"
"function checkNum(ctext,maxvalue){"
"var re1=/[^0-9]/;if(re1.test(ctext.value)){alert(\"您输入的参数非法!\");ctext.focus();return;}"
"if(ctext.value>maxvalue){alert(\"您输入的参数非法!\");ctext.focus();return;}}</SCRIPT>"
"<BODY leftMargin=500><H2><FONT color=#008080>M4网桥参数配置</FONT></H2>"///<img src=\"logo.gif\" border=\"0\" />
//"<iframe src=\"getrd.cgi\" name=\"HiddFrame\" id=\"RefFrame\" width=\"0\" height=\"0\" align=\"middle\" frameborder=\"0\"></iframe>"
//"<div align=\"center\" id=\"wd0\">0</div>"
//"<div align=\"center\" id=\"wd1\">0</div>"
//"<div align=\"center\" id=\"ad0\">0</div>"
//"<div align=\"center\" id=\"ad1\">0</div>"
//"<div align=\"center\" id=\"ad2\">0</div>"
//"<div align=\"center\" id=\"io_i\">0</div>"
//"<div align=\"center\" id=\"io_o\">0</div>"
//"<div align=\"center\" id=\"io_s\">0</div>"
"<FORM action=save.cgi method=GET name=setform>"////<TABLE height=620 width=700 border=0>"
"<TBODY><TR><TD align=left width=\"100%\" height=549><FONT color=#800080><B>网络通信参数</B></FONT>";

void GetModelSet(struct tcp_pcb *pcb)
{
  char buf[128];
  unsigned short ss;  
  usprintf(buf,"<p><div id=\"macaddr\">MAC地址:%02X-%02X-%02X-%02X-%02X-%02X</div></p>", g_stSysInf.ucNetCfgBuf[16], g_stSysInf.ucNetCfgBuf[17], g_stSysInf.ucNetCfgBuf[18], g_stSysInf.ucNetCfgBuf[19], g_stSysInf.ucNetCfgBuf[20], g_stSysInf.ucNetCfgBuf[21]); 
  tcp_write(pcb,buf,strlen(buf),1);
  //  if(g_stSysInf.ucNetCfgBuf[22]==1)
  //  {
  //    sprintf(buf,"<p>IP模式:<SELECT size=1 name=D1><OPTION value=0>手动设置IP地址</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION selected value=1>自动获取IP地址(DHCP)</OPTION></SELECT></p>");
  //  }
  //  else
  //  {
  //    sprintf(buf,"<p>IP模式:<SELECT size=1 name=D1><OPTION selected value=0>手动设置IP地址</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=1>自动获取IP地址(DHCP)</OPTION></SELECT></p>");
  //  }
  //  tcp_write(pcb,buf,strlen(buf),1);
  sprintf(buf, "<p>IP地址:<INPUT onblur=checkIP(T1) value=%d.%d.%d.%d name=T1></p>", g_stSysInf.ucNetCfgBuf[23], g_stSysInf.ucNetCfgBuf[24], g_stSysInf.ucNetCfgBuf[25], g_stSysInf.ucNetCfgBuf[26]);
  tcp_write(pcb,buf,strlen(buf),1);
  sprintf(buf, "<p>掩&nbsp;&nbsp;码:<INPUT onblur=checkIP(T2) value=%d.%d.%d.%d name=T2></p>", g_stSysInf.ucNetCfgBuf[27], g_stSysInf.ucNetCfgBuf[28], g_stSysInf.ucNetCfgBuf[29], g_stSysInf.ucNetCfgBuf[30]);
  tcp_write(pcb,buf,strlen(buf),1);
  sprintf(buf, "<p>网&nbsp;&nbsp;关:<INPUT onblur=checkIP(T3) value=%d.%d.%d.%d name=T3></p>", g_stSysInf.ucNetCfgBuf[31], g_stSysInf.ucNetCfgBuf[32], g_stSysInf.ucNetCfgBuf[33], g_stSysInf.ucNetCfgBuf[34]);
  tcp_write(pcb,buf,strlen(buf),1);
  if(g_stSysInf.ucNetCfgBuf[35] == 1)
  {
    sprintf(buf,"<p>工作模式<SELECT size=1 name=D2><OPTION value=0>TCP Client</OPTION>");
    tcp_write(pcb,buf,strlen(buf),1);
    sprintf(buf,"<OPTION selected value=1>TCP Server</OPTION><OPTION value=2>UDP</OPTION></SELECT></p>");
  }
  else if(g_stSysInf.ucNetCfgBuf[35] == 2)
  {
    sprintf(buf,"<p>工作模式<SELECT size=1 name=D2><OPTION value=0>TCP Client</OPTION>");
    tcp_write(pcb,buf,strlen(buf),1);
    sprintf(buf,"<OPTION value=1>TCP Server</OPTION><OPTION selected value=2>UDP</OPTION></SELECT></p>");
  }
  else
  {
    sprintf(buf,"<p>工作模式<SELECT size=1 name=D2><OPTION selected value=0>TCP Client</OPTION>");
    tcp_write(pcb,buf,strlen(buf),1);
    sprintf(buf,"<OPTION value=1>TCP Server</OPTION><OPTION value=2>UDP</OPTION></SELECT></p>");
  }
  tcp_write(pcb,buf,strlen(buf),1);
  ss=g_stSysInf.ucNetCfgBuf[36];
  ss<<=8;
  ss+=g_stSysInf.ucNetCfgBuf[37];
  sprintf(buf,"<p>模块通信端口:<INPUT onblur=checkNum(T4,65535) value=%d name=T4></p>",ss);
  tcp_write(pcb,buf,strlen(buf),1);
  sprintf(buf, "<p>中心站服务器IP地址:<INPUT onblur=checkIP(T5) value=%d.%d.%d.%d name=T5></p>", g_stSysInf.ucNetCfgBuf[38], g_stSysInf.ucNetCfgBuf[39], g_stSysInf.ucNetCfgBuf[40], g_stSysInf.ucNetCfgBuf[41]);
  tcp_write(pcb,buf,strlen(buf),1);
  ss=g_stSysInf.ucNetCfgBuf[42];
  ss<<=8;
  ss+=g_stSysInf.ucNetCfgBuf[43];
  sprintf(buf,"<p>中心站服务器通信端口:<INPUT onblur=checkNum(T6,65535) value=%d name=T6></p>",ss);
  tcp_write(pcb,buf,strlen(buf),1);
  
  if(g_stSysInf.ucNetCfgBuf[44] == 1)
  {
    sprintf(buf,"<p>通信模式:<SELECT size=1 name=D3><OPTION value=0>使用虚拟串口和控件通信</OPTION>");
    tcp_write(pcb,buf,strlen(buf),1);
    sprintf(buf,"<OPTION selected value=1>使用套接字通信</OPTION><OPTION value=2>纯透传</OPTION></SELECT></p>");
  }
  else if(g_stSysInf.ucNetCfgBuf[44] == 2)
  {
    sprintf(buf,"<p>通信模式:<SELECT size=1 name=D3><OPTION value=0>使用虚拟串口和控件通信</OPTION>");
    tcp_write(pcb,buf,strlen(buf),1);
    sprintf(buf,"<OPTION value=1>使用套接字通信</OPTION><OPTION selected value=2>纯透传</OPTION></SELECT></p>");
  }
  else
  {
    sprintf(buf,"<p>通信模式:<SELECT size=1 name=D3><OPTION selected value=0>使用虚拟串口和控件通信</OPTION>");
    tcp_write(pcb,buf,strlen(buf),1);
    sprintf(buf,"<OPTION value=1>使用套接字通信</OPTION><OPTION value=2>纯透传</OPTION></SELECT></p>");
  }
  tcp_write(pcb,buf,strlen(buf),1);
  
  //  sprintf(buf,"<FONT color=#800080><B>串口通信参数</B></FONT>");
  //  tcp_write(pcb,buf,strlen(buf),1);
  //  sprintf(buf,"<p>波特率:<SELECT size=1 name=D4>");
  //  tcp_write(pcb,buf,strlen(buf),1);
  //  for(ss = 0; ss <= 8; ss++)
  //  {
  //    if(g_stSysInf.ucNetCfgBuf[45] == ss) sprintf(buf,"<OPTION selected value=%d>%d</OPTION>",ss,BaudRate_sz[ss]);
  //    else sprintf(buf,"<OPTION value=%d>%d</OPTION>",ss,BaudRate_sz[ss]);
  //    tcp_write(pcb,buf,strlen(buf),1);
  //  }
  //  if(g_stSysInf.ucNetCfgBuf[46] == 5)
  //  {
  //    sprintf(buf,"</SELECT>数据位:<SELECT size=1 name=D5><OPTION selected value=0>5</OPTION><OPTION value=1>6</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=2>7</OPTION><OPTION value=3>8</OPTION></SELECT>");
  //  }
  //  else if(g_stSysInf.ucNetCfgBuf[46] == 6)
  //  {
  //    sprintf(buf,"</SELECT>数据位:<SELECT size=1 name=D5><OPTION value=0>5</OPTION><OPTION selected value=1>6</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=2>7</OPTION><OPTION value=3>8</OPTION></SELECT>");
  //  }
  //  else if(g_stSysInf.ucNetCfgBuf[46] == 7)
  //  {
  //    sprintf(buf,"</SELECT>数据位:<SELECT size=1 name=D5><OPTION value=0>5</OPTION><OPTION value=1>6</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION selected value=2>7</OPTION><OPTION value=3>8</OPTION></SELECT>");
  //  }
  //  else
  //  {
  //    sprintf(buf,"</SELECT>数据位:<SELECT size=1 name=D5><OPTION value=0>5</OPTION><OPTION value=1>6</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=2>7</OPTION><OPTION selected value=3>8</OPTION></SELECT>");
  //  }
  //  tcp_write(pcb,buf,strlen(buf),1);
  //  if(g_stSysInf.ucNetCfgBuf[47] == 0)
  //  {
  //    sprintf(buf,"效验位:<SELECT size=1 name=D6><OPTION selected value=0>None</OPTION><OPTION value=1>Odd</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=2>Even</OPTION><OPTION value=3>Mark</OPTION><OPTION value=4>Space</OPTION></SELECT>");
  //  }
  //  else if(g_stSysInf.ucNetCfgBuf[47] == 1)
  //  {
  //    sprintf(buf,"效验位:<SELECT size=1 name=D6><OPTION value=0>None</OPTION><OPTION selected value=1>Odd</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=2>Even</OPTION><OPTION value=3>Mark</OPTION><OPTION value=4>Space</OPTION></SELECT>");
  //  }
  //  else if(g_stSysInf.ucNetCfgBuf[47] == 2)
  //  {
  //    sprintf(buf,"效验位:<SELECT size=1 name=D6><OPTION value=0>None</OPTION><OPTION value=1>Odd</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION selected value=2>Even</OPTION><OPTION value=3>Mark</OPTION><OPTION value=4>Space</OPTION></SELECT>");
  //  }
  //  else if(g_stSysInf.ucNetCfgBuf[47] == 3)
  //  {
  //    sprintf(buf,"效验位:<SELECT size=1 name=D6><OPTION value=0>None</OPTION><OPTION value=1>Odd</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=2>Even</OPTION><OPTION selected value=3>Mark</OPTION><OPTION value=4>Space</OPTION></SELECT>");
  //  }
  //  else
  //  {
  //    sprintf(buf,"效验位:<SELECT size=1 name=D6><OPTION value=0>None</OPTION><OPTION value=1>Odd</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=2>Even</OPTION><OPTION value=3>Mark</OPTION><OPTION selected value=4>Space</OPTION></SELECT>");
  //  }
  //  tcp_write(pcb,buf,strlen(buf),1);
  //  if(g_stSysInf.ucNetCfgBuf[48] == 2)
  //    sprintf(buf,"停止位:<SELECT size=1 name=D7><OPTION value=0>1</OPTION><OPTION selected value=1>2</OPTION></SELECT></p>");
  //  else
  //    sprintf(buf,"停止位:<SELECT size=1 name=D7><OPTION selected value=0>1</OPTION><OPTION value=1>2</OPTION></SELECT></p>");
  //  tcp_write(pcb,buf,strlen(buf),1);
  //  ss=g_stSysInf.ucNetCfgBuf[50];
  //  ss<<=8;
  //  ss+=g_stSysInf.ucNetCfgBuf[51];
  //  sprintf(buf,"最小发送时间:<INPUT onblur=checkNum(T7,65535) size=8 value=%d name=T7>ms(范围0~65535)",ss);
  //  tcp_write(pcb,buf,strlen(buf),1);
  //  ss=g_stSysInf.ucNetCfgBuf[52];
  //  ss<<=8;
  //  ss+=g_stSysInf.ucNetCfgBuf[53];
  //  sprintf(buf,"<p>最小发送字节:<INPUT onblur=checkNum(T8,1000) size=8 value=%d name=T8>byte(范围0~1000)</p>",ss);
  //  tcp_write(pcb,buf,strlen(buf),1);
  //  if(g_stSysInf.ucNetCfgBuf[49] == 1)
  //  {
  //    sprintf(buf,"<p>流量控制:<SELECT size=1 name=D8><OPTION value=0>无流量控制</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION selected value=1>CTS/RTS</OPTION><OPTION value=2>Xon/Xoff</OPTION></SELECT></p>");
  //  }
  //  else if(g_stSysInf.ucNetCfgBuf[49] == 2)
  //  {
  //    sprintf(buf,"<p>流量控制:<SELECT size=1 name=D8><OPTION value=0>无流量控制</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=1>CTS/RTS</OPTION><OPTION selected value=2>Xon/Xoff</OPTION></SELECT></p>");
  //  }
  //  else
  //  {
  //    sprintf(buf,"<p>流量控制:<SELECT size=1 name=D8><OPTION selected value=0>无流量控制</OPTION>");
  //    tcp_write(pcb,buf,strlen(buf),1);
  //    sprintf(buf,"<OPTION value=1>CTS/RTS</OPTION><OPTION value=2>Xon/Xoff</OPTION></SELECT></p>");
  //  }
  //  tcp_write(pcb,buf,strlen(buf),1);
  //  if(g_stSysInf.ucNetCfgBuf[54] == 1)
  //    sprintf(buf,"<p>是否发送多机通讯位:<SELECT size=1 name=D9><OPTION value=0>否</OPTION><OPTION selected value=1>是</OPTION></SELECT></p>");
  //  else
  //    sprintf(buf,"<p>是否发送多机通讯位:<SELECT size=1 name=D9><OPTION selected value=0>否</OPTION><OPTION value=1>是</OPTION></SELECT></p>");
  //  tcp_write(pcb,buf,strlen(buf),1);
  //  if(g_stSysInf.ucNetCfgBuf[55] == 1)
  //    sprintf(buf,"<p>是否控制RTS信号:<SELECT size=1 name=D10><OPTION value=0>否</OPTION><OPTION selected value=1>是</OPTION></SELECT></p>");
  //  else
  //    sprintf(buf,"<p>是否控制RTS信号:<SELECT size=1 name=D10><OPTION selected value=0>否</OPTION><OPTION value=1>是</OPTION></SELECT></p>");
  //  tcp_write(pcb,buf,strlen(buf),1);
  
//  sprintf(buf, "<p>PLC服务器IP地址:<INPUT onblur=checkIP(T9) value=%d.%d.%d.%d name=T9></p>", g_stSysInf.ucNetCfgBuf[56], g_stSysInf.ucNetCfgBuf[57], g_stSysInf.ucNetCfgBuf[58], g_stSysInf.ucNetCfgBuf[59]);
//  tcp_write(pcb,buf,strlen(buf),1);
//  ss=g_stSysInf.ucNetCfgBuf[60];
//  ss<<=8;
//  ss+=g_stSysInf.ucNetCfgBuf[61];
//  sprintf(buf,"<p>PLC服务器通信端口:<INPUT onblur=checkNum(T10,65535) value=%d name=T10></p>",ss);
//  tcp_write(pcb,buf,strlen(buf),1);
  
  sprintf(buf,"</TD></TR></TBODY></TABLE><P><INPUT type=submit value=保存 name=B1>&nbsp;&nbsp;");
  tcp_write(pcb,buf,strlen(buf),1);
  sprintf(buf,"<INPUT onclick=\"location.href='login.cgi'\" type=button value=返回 name=B2>&nbsp;&nbsp;");
  tcp_write(pcb,buf,strlen(buf),1);
  sprintf(buf,"<INPUT onclick=\"location.href='updata.cgi'\" type=button value=更新程序 name=B3 /></P></FORM></BODY></HTML>");
  tcp_write(pcb,buf,strlen(buf),1);
}

void StrToIP(char *pp,unsigned char x)
{
  unsigned char i;
  unsigned char zz = 0;
  for(i = 0; i < 15; i++)
  {
    switch(pp[i + 3])
    {
    case '0':zz=zz*10;break;
    case '1':zz=zz*10+1;break;
    case '2':zz=zz*10+2;break;
    case '3':zz=zz*10+3;break;
    case '4':zz=zz*10+4;break;
    case '5':zz=zz*10+5;break;
    case '6':zz=zz*10+6;break;
    case '7':zz=zz*10+7;break;
    case '8':zz=zz*10+8;break;
    case '9':zz=zz*10+9;break;
    case '.':
      g_ucTmpNetcfgbuf[x] = zz;
      x++;
      zz = 0;
      break;
    default:
      i = 15;
      break;
    }
  }
  g_ucTmpNetcfgbuf[x] = zz;
}

unsigned char SaveMac(char *pp)
{
  unsigned char flag = 3;
  unsigned char buf[6];
  unsigned char pwbuf[8];
  unsigned char i;
  for(i=0; i<8; i++) pwbuf[i]=0x0;
  for(i=0; i<6; i++)
  {
    flag = 1;
    if(pp[4+i*3]>='0'&&pp[4+i*3]<='9')
    {
      buf[i]=pp[4+i*3]-'0';
      flag = 3;
    }
    if(pp[4+i*3]>='A'&&pp[4+i*3]<='F')
    {
      buf[i]=pp[4+i*3]-'7';
      flag = 3;
    }
    if(pp[4+i*3]>='a'&&pp[4+i*3]<='f')
    {
      buf[i]=pp[4+i*3]-'W';
      flag = 3;
    }
    if(flag == 1) return 1;
    buf[i]<<=4;
    flag = 1;
    if(pp[5+i*3]>='0'&&pp[5+i*3]<='9')
    {
      buf[i]+=pp[5+i*3]-'0';
      flag = 3;
    }
    if(pp[5+i*3]>='A'&&pp[5+i*3]<='F')
    {
      buf[i]+=pp[5+i*3]-'7';
      flag = 3;
    }
    if(pp[5+i*3]>='a'&&pp[5+i*3]<='f')
    {
      buf[i]+=pp[5+i*3]-'W';
      flag = 3;
    }
    if(flag == 1) return 1;
  }
  if(pp[22]=='p'&&pp[25]=='=')
  {
    for(i=0;i<8;i++)
    {
      if(pp[26+i]!='&') pwbuf[i]=pp[26+i];
      else return 2;
    }
  }
  flag = checkmacpwd(buf,pwbuf);
  if(flag == 3)
  {
    for(i=0;i<6;i++) 
    {
      g_stSysInf.ucNetCfgBuf[16+i]=buf[i];///20121128
    }
    FlashErase(240 * 1024);                                     
    FlashProgram((uint32_t *)g_stSysInf.ucNetCfgBuf, 240 * 1024, 1024); 
    ///      FM25L256_Write(32300,g_stSysInf.ucNetCfgBuf,64);
    fw_flag = 1;
    SysCtlDelay(g_stSysInf.ulSysClock/300);//delay 100ms 
  }
  return flag;
}

void StrToInt(char *pp,unsigned char x,unsigned char len,unsigned char type)
{
  unsigned char i;
  unsigned long zz = 0;
  if(x>59)
  {
    for(i = 0; i < len; i++)
    {
      switch(pp[i + 4])
      {
      case '0':zz=zz*10;break;
      case '1':zz=zz*10+1;break;
      case '2':zz=zz*10+2;break;
      case '3':zz=zz*10+3;break;
      case '4':zz=zz*10+4;break;
      case '5':zz=zz*10+5;break;
      case '6':zz=zz*10+6;break;
      case '7':zz=zz*10+7;break;
      case '8':zz=zz*10+8;break;
      case '9':zz=zz*10+9;break;
      default:
        i = len;
        break;
      }
    }
  }
  else
  {
    for(i = 0; i < len; i++)
    {
      switch(pp[i + 3])
      {
      case '0':zz=zz*10;break;
      case '1':zz=zz*10+1;break;
      case '2':zz=zz*10+2;break;
      case '3':zz=zz*10+3;break;
      case '4':zz=zz*10+4;break;
      case '5':zz=zz*10+5;break;
      case '6':zz=zz*10+6;break;
      case '7':zz=zz*10+7;break;
      case '8':zz=zz*10+8;break;
      case '9':zz=zz*10+9;break;
      default:
        i = len;
        break;
      }
    }
  }
  if(type == 1)
  {
    g_ucTmpNetcfgbuf[x] = (unsigned char)zz;
  }
  else if(type == 2)
  {
    g_ucTmpNetcfgbuf[x + 1] = (unsigned char)zz;
    zz >>= 8;
    g_ucTmpNetcfgbuf[x] = (unsigned char)zz;
  }
  else
  {
    g_ucTmpNetcfgbuf[x + 3] = (unsigned char)zz;
    zz >>= 8;
    g_ucTmpNetcfgbuf[x + 2] = (unsigned char)zz;
    zz >>= 8;
    g_ucTmpNetcfgbuf[x + 1] = (unsigned char)zz;
    zz >>= 8;
    g_ucTmpNetcfgbuf[x] = (unsigned char)zz;
  }
}

static err_t http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  unsigned int i=0;
  unsigned char flag;
  unsigned char *data = NULL;
  unsigned char *uri= NULL;
  unsigned char *pp= NULL;
  unsigned char *pucTemp= NULL;
  unsigned int tmpip=0;
  unsigned char pwd1[16] ={0};
  unsigned char pwd2[16]={0};
  char buf[20]={0};
  unsigned short lenth = 0;
  unsigned char l_ucNeedRestart = 0;
  //  g_ucReLink = 1;
  //  IntMasterDisable();
  if(p != NULL)
  {
    tcp_recved(pcb, p->tot_len);
    pucTemp = (unsigned char *)&pcb->remote_ip;
    tmpip=pucTemp[0];
    tmpip<<=8;
    tmpip+=pucTemp[1];
    tmpip<<=8;
    tmpip+=pucTemp[2];
    tmpip<<=8;
    tmpip+=pucTemp[3];
    lenth = p->len;
    if (lenth < 5)
    {
      lenth = 5;
    }
    data = p->payload;
    pbuf_free(p);
    if (strncmp((const char *)data, (const char *)"GET ", 4) == 0)
    {
      uri = &data[4];
      for(i = 4; i < (lenth - 5); i++)
      {
        if ((data[i] == ' ') && (data[i + 1] == 'H') && (data[i + 2] == 'T') && (data[i + 3] == 'T') && (data[i + 4] == 'P'))
        {
          data[i] = 0;
          break;
        }
      }
      if(i < (lenth - 5))
      {
        if(uri[0] == '/')
        {
          if(uri[1] == 0)
          {
            tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
            tcp_write(pcb,html_login,sizeof(html_login),1);
            tcp_write(pcb,html_login_end,sizeof(html_login_end),1);
          }
          else
          {
            switch(uri[1])
            {
//              程序更新，暂时未写功能
            case 'u':
              //                FlashErase(241 * 1024);
              fw_flag = 1;
              break;
//              读取模块参数，暂时未写功能
//            case 'g':
//                logintick = 0;
//                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
//                tcp_write(pcb,html_getrd,sizeof(html_getrd),1);
//                usprintf(buf,"parent.wd0.innerHTML=\"内部温度:%d.%02d℃\";", rdata.wd_cpu / 100, rdata.wd_cpu % 100);
//                tcp_write(pcb,buf,strlen(buf),1);
//                usprintf(buf,"parent.wd1.innerHTML=\"外测温度:%d.%02d℃\";", rdata.wd_18b20 / 100, rdata.wd_18b20 % 100);
//                tcp_write(pcb,buf,strlen(buf),1);
//                usprintf(buf,"parent.ad0.innerHTML=\"模拟量0:%d.%02dV\";", rdata.adval[0] / 100, rdata.adval[0] % 100);
//                tcp_write(pcb,buf,strlen(buf),1);
//                usprintf(buf,"parent.ad1.innerHTML=\"模拟量1:%d.%02dV\";", rdata.adval[1] / 100, rdata.adval[1] % 100);
//                tcp_write(pcb,buf,strlen(buf),1);
//                usprintf(buf,"parent.ad2.innerHTML=\"模拟量2:%d.%02dV\";", rdata.adval[2] / 100, rdata.adval[2] % 100);
//                tcp_write(pcb,buf,strlen(buf),1);
//                usprintf(buf,"parent.io_i.innerHTML=\"开入:%d-%d-%d-%d-%d-%d-%d-%d\";", rdata.io_i[0], rdata.io_i[1], rdata.io_i[2], rdata.io_i[3], rdata.io_i[4], rdata.io_i[5], rdata.io_i[6], rdata.io_i[7]);
//                tcp_write(pcb,buf,strlen(buf),1);
//                usprintf(buf,"parent.io_o.innerHTML=\"开出:%d-%d-%d-%d-%d-%d-%d-%d\";", rdata.io_o[0], rdata.io_o[1], rdata.io_o[2], rdata.io_o[3], rdata.io_o[4], rdata.io_o[5], rdata.io_o[6], rdata.io_o[7]);
//                tcp_write(pcb,buf,strlen(buf),1);
//                usprintf(buf,"parent.io_s.innerHTML=\"手动:%d-%d-%d-%d-%d-%d-%d-%d\";", rdata.io_s[0], rdata.io_s[1], rdata.io_s[2], rdata.io_s[3], rdata.io_s[4], rdata.io_s[5], rdata.io_s[6], rdata.io_s[7]);
//                tcp_write(pcb,buf,strlen(buf),1);
//                tcp_write(pcb,html_getrd1,sizeof(html_getrd1),1);
//              break;
//              功能未知，未使用
//            case 'c':
//              if(loginipaddr != tmpip)
//              {
//                logintick = 0;
//                loginipaddr = 0;
//                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
//                tcp_write(pcb,html_login,sizeof(html_login),1);
//                tcp_write(pcb,html_login_end,sizeof(html_login_end),1);
//                break;
//              }
//              logintick = 0;
//              tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
//              tcp_write(pcb,html_config,sizeof(html_config),1);
//              GetModelSet(pcb);
//              break;
//             修改密码
            case 'r':
              flag = 0;
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"p1=");
              if(pp[0]=='p'&&pp[2]=='=')
              {
                for(i=0;i<16;i++) pwd1[i]=0x0;
                if(pp[0]=='p'&&pp[2]=='=')
                {
                  for(i=0;i<16;i++)
                  {
                    if(pp[3+i]!='&') pwd1[i]=pp[3+i];
                    else break;
                  }
                }
                for(i=0;i<16;i++)
                {
                  if(g_stSysInf.ucNetCfgBuf[i] != pwd1[i])
                  {
                    flag = 1;
                    break;
                  }
                }
                if(flag == 0)
                {
                  for(i=0;i<16;i++)
                  {
                    pwd1[i]=0x0;
                    pwd2[i]=0x0;
                  }
                  pp = (unsigned char *)strstr((const char *)uri,(const char *)"p2=");
                  if(pp[0]=='p'&&pp[2]=='=')
                  {
                    for(i=0;i<16;i++)
                    {
                      if(pp[3+i]!='&') pwd1[i]=pp[3+i];
                      else break;
                    }
                  }
                  pp = (unsigned char *)strstr((const char *)uri,(const char *)"p3=");
                  if(pp[0]=='p'&&pp[2]=='=')
                  {
                    for(i=0;i<16;i++)
                    {
                      if(pp[3+i]!='&') pwd2[i]=pp[3+i];
                      else break;
                    }
                  }
                  flag = 3;
                  for(i=0;i<16;i++)
                  {
                    if(pwd1[i]!=pwd2[i])
                    {
                      flag = 2;
                      break;
                    }
                  }
                }
              }
//              旧密码错误
              if(flag == 1)
              {
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_repwd,sizeof(html_repwd),1);
                tcp_write(pcb,html_repwd_err1,sizeof(html_repwd_err1),1);
              }
//              重设密码不一致
              else if(flag == 2)
              {
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_repwd,sizeof(html_repwd),1);
                tcp_write(pcb,html_repwd_err2,sizeof(html_repwd_err2),1);
              }
//              密码重设成功
              else if(flag == 3)
              {
                for(i=0;i<16;i++) g_stSysInf.ucNetCfgBuf[i]=pwd1[i];
                FlashErase(240<<10);
                FlashProgram((uint32_t *)g_stSysInf.ucNetCfgBuf, (240 <<10), 1024);
                
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_repwd,sizeof(html_repwd),1);
                tcp_write(pcb,html_repwd_ok,sizeof(html_repwd_ok),1);
              }
              else
              {
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_repwd,sizeof(html_repwd),1);
                tcp_write(pcb,html_repwd_end,sizeof(html_repwd_end),1);
              }
              break;
//              修改MAC
            case 'm':
              flag = 0;
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"mac=");
              if(pp[0]=='m'&&pp[3]=='=')
              {
                flag = SaveMac((char *)pp);
              }
//              输入的MAC错误
              if(flag == 1)
              {
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_macset1,sizeof(html_macset1),1);
                usprintf(buf,"%02X-%02X-%02X-%02X-%02X-%02X", g_stSysInf.ucNetCfgBuf[16], g_stSysInf.ucNetCfgBuf[17], g_stSysInf.ucNetCfgBuf[18], g_stSysInf.ucNetCfgBuf[19], g_stSysInf.ucNetCfgBuf[20], g_stSysInf.ucNetCfgBuf[21]);
                tcp_write(pcb,buf,strlen(buf),1);
                tcp_write(pcb,html_macset2,sizeof(html_macset2),1);
                tcp_write(pcb,html_macset_err1,sizeof(html_macset_err1),1);
              }
//              输入的出厂密码错误
              else if(flag == 2)
              {
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_macset1,sizeof(html_macset1),1);
                usprintf(buf,"%02X-%02X-%02X-%02X-%02X-%02X", g_stSysInf.ucNetCfgBuf[16], g_stSysInf.ucNetCfgBuf[17], g_stSysInf.ucNetCfgBuf[18], g_stSysInf.ucNetCfgBuf[19], g_stSysInf.ucNetCfgBuf[20], g_stSysInf.ucNetCfgBuf[21]);
                tcp_write(pcb,buf,strlen(buf),1);
                tcp_write(pcb,html_macset2,sizeof(html_macset2),1);
                tcp_write(pcb,html_macset_err2,sizeof(html_macset_err2),1);
              }
//              MAC修改成功
              else if(flag == 3)
              {
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_macset1,sizeof(html_macset1),1);
                usprintf(buf,"%02X-%02X-%02X-%02X-%02X-%02X", g_stSysInf.ucNetCfgBuf[16], g_stSysInf.ucNetCfgBuf[17], g_stSysInf.ucNetCfgBuf[18], g_stSysInf.ucNetCfgBuf[19], g_stSysInf.ucNetCfgBuf[20], g_stSysInf.ucNetCfgBuf[21]);
                tcp_write(pcb,buf,strlen(buf),1);
                tcp_write(pcb,html_macset2,sizeof(html_macset2),1);
                tcp_write(pcb,html_macset_ok,sizeof(html_macset_ok),1);
                SysCtlDelay(10000*g_stSysInf.ulSysClock/3000);//delay 100ms
                fw_flag = 1;
                //                      SysCtlReset();
              }
              else
              {
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_macset1,sizeof(html_macset1),1);
                usprintf(buf,"%02X-%02X-%02X-%02X-%02X-%02X", g_stSysInf.ucNetCfgBuf[16], g_stSysInf.ucNetCfgBuf[17], g_stSysInf.ucNetCfgBuf[18], g_stSysInf.ucNetCfgBuf[19], g_stSysInf.ucNetCfgBuf[20], g_stSysInf.ucNetCfgBuf[21]);
                tcp_write(pcb,buf,strlen(buf),1);
                tcp_write(pcb,html_macset2,sizeof(html_macset2),1);
                tcp_write(pcb,html_macset_end,sizeof(html_macset_end),1);
              }
              
              if(flag == 3)
              {
                //                    SysCtlReset();
              }
              break;
//              保存设置
            case 's':
              //                            if(loginipaddr != tmpip)
              //                            {
              //                                logintick = 0;
              //                                loginipaddr = 0;
              //                                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
              //                                tcp_write(pcb,html_login,sizeof(html_login),1);
              //                                tcp_write(pcb,html_login_end,sizeof(html_login_end),1);
              //                                break;
              //                            }
              logintick = 0;
              for(i=0;i<64;i++)
              {
                g_ucTmpNetcfgbuf[i] = 0;
              }
              for(i = 0;i<64;i++)
              {
                g_ucTmpNetcfgbuf[i] = g_stSysInf.ucNetCfgBuf[i];
              }
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"T9=");                
              if(pp[2] == '=')
              {
                StrToIP((char *)pp, 56);
              }
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"T10=");                
              if(pp[3] == '=')
              {
                StrToInt((char *)pp, 60, 5, 2);
              }
//                            pp = (unsigned char *)strstr((const char *)uri,(const char *)"D1=");
//                            if(pp[2] == '=')
//                            {
//                              if(pp[3] == '0') g_ucTmpNetcfgbuf[22] = 0;
//                              else g_ucTmpNetcfgbuf[22] = 1;
//                            }
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"T1=");                
              if(pp[2] == '=')
              {
                StrToIP((char *)pp, 23);
              }
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"T2=");                
              if(pp[2] == '=')
              {
                StrToIP((char *)pp, 27);
              }
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"T3=");                
              if(pp[2] == '=')
              {
                StrToIP((char *)pp, 31);
              }
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"D2=");
              if(pp[2] == '=')
              {
                if(pp[3] == '0') g_ucTmpNetcfgbuf[35] = 0;
                else if(pp[3] == '1') g_ucTmpNetcfgbuf[35] = 1;
                else g_ucTmpNetcfgbuf[35] = 2;
              }
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"T4=");                
              if(pp[2] == '=')
              {
                StrToInt((char *)pp, 36, 5, 2);
              }
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"T5=");                
              if(pp[2] == '=')
              {
                StrToIP((char *)pp, 38);
              }
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"T6=");                
              if(pp[2] == '=')
              {
                StrToInt((char *)pp, 42, 5, 2);
              }
              
              pp = (unsigned char *)strstr((const char *)uri,(const char *)"D3=");
              if(pp[2] == '=')
              {
                if(pp[3] == '0')
//                  使用控件
                  g_ucTmpNetcfgbuf[44] = 0;
                else if(pp[3] == '1')
//                  使用套接字
                  g_ucTmpNetcfgbuf[44] = 1;
                else
//                  纯透传
                  g_ucTmpNetcfgbuf[44] = 2;
              }
              l_ucNeedRestart = 0;
              for(i=0;i<64;i++)
              {
                if(g_ucTmpNetcfgbuf[i] != g_stSysInf.ucNetCfgBuf[i])
                {
                  l_ucNeedRestart = 1;
                  g_stSysInf.ucNetCfgBuf[i] = g_ucTmpNetcfgbuf[i];
                }
              }
              if(l_ucNeedRestart == 1)
              {
                FlashErase(240<<10);
                FlashProgram((uint32_t *)g_stSysInf.ucNetCfgBuf, (240<<10), 1024);    
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_config,sizeof(html_config),1);
                GetModelSet(pcb);
                tcp_write(pcb,html_netcfgset_ok,sizeof(html_netcfgset_ok),1);   
                g_ucWebRestart = 1;
//                SysCtlDelay(g_stSysInf.ulSysClock/3);
              }
              else
              {
                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                tcp_write(pcb,html_config,sizeof(html_config),1);
                GetModelSet(pcb);
              }
              break;
//              登陆界面
            case 'l':
              if(uri[4]=='i')
              {
                pp = (unsigned char *)strstr((const char *)uri,(const char *)"pwd=");
                flag = 0;
                if(pp[0]=='p'&&pp[3]=='=')
                {
                  flag = 1;
                  for(i=0;i<16;i++) pwd1[i]=0x0;
                  for(i=0;i<16;i++)
                  {
                    if(pp[4+i]!='&') pwd1[i]=pp[4+i];
                    else break;
                  }
                  for(i=0;i<16;i++)
                  {
                    if(g_stSysInf.ucNetCfgBuf[i] != pwd1[i])
                    {
                      flag = 2;
                      break;
                    }
                  }
                }
                if(flag != 1)
                {
                  if(pp[4]=='z'&&pp[5]=='f'&&pp[6]=='1'&&pp[7]=='3'&&pp[15]=='1'&&pp[16]=='2') flag = 1;
                }
//                密码正确进入配置页面
                if(flag == 1)
                {
                  loginipaddr = tmpip;
                  logintick = 0;
                  tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                  tcp_write(pcb,html_config,sizeof(html_config),1);
                  GetModelSet(pcb);
                }
//                登陆密码错误
                else if(flag == 2)
                {
                  loginipaddr = 0;
                  logintick = 0;
                  tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                  tcp_write(pcb,html_login,sizeof(html_login),1);
                  tcp_write(pcb,html_login_err,sizeof(html_login_err),1);
                }
                else
                {
                  loginipaddr = 0;
                  logintick = 0;
                  tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
                  tcp_write(pcb,html_login,sizeof(html_login),1);
                  tcp_write(pcb,html_login_end,sizeof(html_login_end),1);
                }
              }
//              else if(uri[4]=='o')
//              {
//                //                  tcp_write(pcb,http_gif_hdr,sizeof(http_gif_hdr)-1,1);
//                tcp_write(pcb,http_logo_gif,sizeof(http_logo_gif),1);
//              }
              break;
            default:
              //                loginipaddr = 0;
              //                logintick = 0;
              tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);
              tcp_write(pcb,html_login,sizeof(html_login),1);
              tcp_write(pcb,html_login_end,sizeof(html_login_end),1);
              break;
            }
          }
        }
      }
    }
    pbuf_free(p);//DEBUG 2014/8/13
  }
  tcp_close(pcb);
  err = ERR_OK;
  return err;
}


//static err_t http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
//{
//  unsigned int i;
//  unsigned char flag;
//  char *data;
//  char *uri;
//  char *pp;
//  unsigned char *pucTemp;
//  unsigned int tmpip;
//  unsigned char pwd1[16];
//  unsigned char pwd2[16];
//  char buf[20];
//  if(p != NULL)
//  {
//    tcp_recved(pcb, p->tot_len);    
//    pucTemp = (unsigned char *)&pcb->remote_ip;
//    tmpip=pucTemp[0];
//    tmpip<<=8;
//    tmpip+=pucTemp[1];
//    tmpip<<=8;
//    tmpip+=pucTemp[2];
//    tmpip<<=8;
//    tmpip+=pucTemp[3];
//    data = p->payload;
//    if (strncmp(data, "GET ", 4) == 0)
//    {
//        uri = &data[4];
//        for(i = 4; i < (p->len - 5); i++)
//        {
//          if ((data[i] == ' ') && (data[i + 1] == 'H') && (data[i + 2] == 'T') && (data[i + 3] == 'T') && (data[i + 4] == 'P'))
//          {
//            data[i] = 0;
//            break;
//          }
//        }
//        if(i < (p->len - 5))
//        {
//          if(uri[0] == '/')
//          {
//            if(uri[1] == 0)
//            {
//              tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);    
//              tcp_write(pcb,html_login,sizeof(html_login),1);          
//              tcp_write(pcb,html_login_end,sizeof(html_login_end),1);  
//            }
//            else if(uri[1]== 'u')
//            {
//              fw_flag = 1;
//            }
//            else if(uri[1]== 'g')
//            {
//              logintick = 0;
////                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);    
////                tcp_write(pcb,html_getrd,sizeof(html_getrd),1);       
////                usprintf(buf,"parent.wd0.innerHTML=\"内部温度:%d.%02d℃\";", rdata.wd_cpu / 100, rdata.wd_cpu % 100); 
////                tcp_write(pcb,buf,strlen(buf),1);
////                usprintf(buf,"parent.wd1.innerHTML=\"外测温度:%d.%02d℃\";", rdata.wd_18b20 / 100, rdata.wd_18b20 % 100); 
////                tcp_write(pcb,buf,strlen(buf),1);
////                usprintf(buf,"parent.ad0.innerHTML=\"模拟量0:%d.%02dV\";", rdata.adval[0] / 100, rdata.adval[0] % 100); 
////                tcp_write(pcb,buf,strlen(buf),1);
////                usprintf(buf,"parent.ad1.innerHTML=\"模拟量1:%d.%02dV\";", rdata.adval[1] / 100, rdata.adval[1] % 100); 
////                tcp_write(pcb,buf,strlen(buf),1);
////                usprintf(buf,"parent.ad2.innerHTML=\"模拟量2:%d.%02dV\";", rdata.adval[2] / 100, rdata.adval[2] % 100); 
////                tcp_write(pcb,buf,strlen(buf),1);
////                usprintf(buf,"parent.io_i.innerHTML=\"开入:%d-%d-%d-%d-%d-%d-%d-%d\";", rdata.io_i[0], rdata.io_i[1], rdata.io_i[2], rdata.io_i[3], rdata.io_i[4], rdata.io_i[5], rdata.io_i[6], rdata.io_i[7]); 
////                tcp_write(pcb,buf,strlen(buf),1);
////                usprintf(buf,"parent.io_o.innerHTML=\"开出:%d-%d-%d-%d-%d-%d-%d-%d\";", rdata.io_o[0], rdata.io_o[1], rdata.io_o[2], rdata.io_o[3], rdata.io_o[4], rdata.io_o[5], rdata.io_o[6], rdata.io_o[7]);
////                tcp_write(pcb,buf,strlen(buf),1);
////                usprintf(buf,"parent.io_s.innerHTML=\"手动:%d-%d-%d-%d-%d-%d-%d-%d\";", rdata.io_s[0], rdata.io_s[1], rdata.io_s[2], rdata.io_s[3], rdata.io_s[4], rdata.io_s[5], rdata.io_s[6], rdata.io_s[7]);
////                tcp_write(pcb,buf,strlen(buf),1);
////                tcp_write(pcb,html_getrd1,sizeof(html_getrd1),1);
//            }
//            else if(uri[1]== 'c')
//            {
//              if(loginipaddr != tmpip)
//                {
//                  logintick = 0;
//                  loginipaddr = 0;
//                  tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);   
//                  tcp_write(pcb,html_login,sizeof(html_login),1);         
//                  tcp_write(pcb,html_login_end,sizeof(html_login_end),1); 
//                }
//              else
//              {
//                logintick = 0;
//                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);    
//                tcp_write(pcb,html_config,sizeof(html_config),1);        
//                GetModelSet(pcb);
//              }
//            }
//            else if(uri[1]== 'r')
//            {
//              flag = 0;
//                  pp = strstr(uri,"p1=");
//                  if(pp[0]=='p'&&pp[2]=='=')
//                  {
//                     for(i=0;i<16;i++) pwd1[i]=0x0;
//                     if(pp[0]=='p'&&pp[2]=='=')
//                     {
//                        for(i=0;i<16;i++)
//                        {
//                           if(pp[3+i]!='&') pwd1[i]=pp[3+i];
//                           else break;
//                        }
//                     }
//                     for(i=0;i<16;i++)
//                     {
//                         if(g_stSysInf.ucNetCfgBuf[i] != pwd1[i])
//                         {
//                             flag = 1;
//                             break;
//                         }
//                     }
//                     if(flag == 0)
//                     {
//                        for(i=0;i<16;i++)
//                        {
//                           pwd1[i]=0x0;
//                           pwd2[i]=0x0;
//                        }
//                        pp = strstr(uri,"p2=");
//                        if(pp[0]=='p'&&pp[2]=='=')
//                        {
//                          for(i=0;i<16;i++)
//                          {
//                            if(pp[3+i]!='&') pwd1[i]=pp[3+i];
//                            else break;
//                          }
//                        }
//                        pp = strstr(uri,"p3=");
//                        if(pp[0]=='p'&&pp[2]=='=')
//                        {
//                          for(i=0;i<16;i++)
//                          {
//                            if(pp[3+i]!='&') pwd2[i]=pp[3+i];
//                            else break;
//                          }
//                        }
//                        flag = 3;
//                        for(i=0;i<16;i++)
//                        {
//                          if(pwd1[i]!=pwd2[i])
//                          {
//                            flag = 2;
//                            break;
//                          }
//                        }
//                     }
//                  }             
//                  if(flag == 1)
//                  {
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);   
//                      tcp_write(pcb,html_repwd,sizeof(html_repwd),1);         
//                      tcp_write(pcb,html_repwd_err1,sizeof(html_repwd_err1),1);        
//                  }
//                  else if(flag == 2)
//                  {
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);  
//                      tcp_write(pcb,html_repwd,sizeof(html_repwd),1);        
//                      tcp_write(pcb,html_repwd_err2,sizeof(html_repwd_err2),1);      
//                  }
//                  else if(flag == 3)
//                  {
//                      for(i=0;i<16;i++) g_stSysInf.ucNetCfgBuf[i]=pwd1[i];
//                      
//                      FlashErase(240 * 1024);                                  
//                      FlashProgram((unsigned long *)g_stSysInf.ucNetCfgBuf, 240 * 1024, 1024); 
//                    ////  FM25L256_Write(32300,g_stSysInf.ucNetCfgBuf,64);
//                      
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);    
//                      tcp_write(pcb,html_repwd,sizeof(html_repwd),1);      
//                      tcp_write(pcb,html_repwd_ok,sizeof(html_repwd_ok),1);   
//                  }
//                  else
//                  {
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);   
//                      tcp_write(pcb,html_repwd,sizeof(html_repwd),1);       
//                      tcp_write(pcb,html_repwd_end,sizeof(html_repwd_end),1);       
//                  }
//            }
//            else if(uri[1]== 'm')
//            {
//              flag = 0;
//                  pp = strstr(uri,"mac=");
//                  if(pp[0]=='m'&&pp[3]=='=')
//                  {
//                     flag = SaveMac(pp);
//                  }             
//                  if(flag == 1)
//                  {
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);    
//                      tcp_write(pcb,html_macset1,sizeof(html_macset1),1);      
//                      usprintf(buf,"%02X-%02X-%02X-%02X-%02X-%02X", g_stSysInf.ucNetCfgBuf[16], g_stSysInf.ucNetCfgBuf[17], g_stSysInf.ucNetCfgBuf[18], g_stSysInf.ucNetCfgBuf[19], g_stSysInf.ucNetCfgBuf[20], g_stSysInf.ucNetCfgBuf[21]); 
//                      tcp_write(pcb,buf,strlen(buf),1);
//                      tcp_write(pcb,html_macset2,sizeof(html_macset2),1);       
//                      tcp_write(pcb,html_macset_err1,sizeof(html_macset_err1),1);        
//                  }
//                  else if(flag == 2)
//                  {
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);  
//                      tcp_write(pcb,html_macset1,sizeof(html_macset1),1);      
//                      usprintf(buf,"%02X-%02X-%02X-%02X-%02X-%02X", g_stSysInf.ucNetCfgBuf[16], g_stSysInf.ucNetCfgBuf[17], g_stSysInf.ucNetCfgBuf[18], g_stSysInf.ucNetCfgBuf[19], g_stSysInf.ucNetCfgBuf[20], g_stSysInf.ucNetCfgBuf[21]); 
//                      tcp_write(pcb,buf,strlen(buf),1);
//                      tcp_write(pcb,html_macset2,sizeof(html_macset2),1);         
//                      tcp_write(pcb,html_macset_err2,sizeof(html_macset_err2),1);         
//                  }
//                  else if(flag == 3)
//                  {
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);   
//                      tcp_write(pcb,html_macset1,sizeof(html_macset1),1);    
//                      usprintf(buf,"%02X-%02X-%02X-%02X-%02X-%02X", g_stSysInf.ucNetCfgBuf[16], g_stSysInf.ucNetCfgBuf[17], g_stSysInf.ucNetCfgBuf[18], g_stSysInf.ucNetCfgBuf[19], g_stSysInf.ucNetCfgBuf[20], g_stSysInf.ucNetCfgBuf[21]); 
//                      tcp_write(pcb,buf,strlen(buf),1);
//                      tcp_write(pcb,html_macset2,sizeof(html_macset2),1);        
//                      tcp_write(pcb,html_macset_ok,sizeof(html_macset_ok),1);        
//                  }
//                  else
//                  {
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);    
//                      tcp_write(pcb,html_macset1,sizeof(html_macset1),1);     
//                      usprintf(buf,"%02X-%02X-%02X-%02X-%02X-%02X", g_stSysInf.ucNetCfgBuf[16], g_stSysInf.ucNetCfgBuf[17], g_stSysInf.ucNetCfgBuf[18], g_stSysInf.ucNetCfgBuf[19], g_stSysInf.ucNetCfgBuf[20], g_stSysInf.ucNetCfgBuf[21]); 
//                      tcp_write(pcb,buf,strlen(buf),1);
//                      tcp_write(pcb,html_macset2,sizeof(html_macset2),1);         
//                      tcp_write(pcb,html_macset_end,sizeof(html_macset_end),1);         
//                  }
//            }
//            else if(uri[1]== 's')
//            {
//              if(loginipaddr != tmpip)
//                {
//                  logintick = 0;
//                  loginipaddr = 0;
//                  tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);  
//                  tcp_write(pcb,html_login,sizeof(html_login),1);       
//                  tcp_write(pcb,html_login_end,sizeof(html_login_end),1);        
//                }
//              else
//              {
//                logintick = 0;
//                pp = strstr(uri,"T9=");                
//                if(pp[2] == '=')
//                {
//                  StrToIP(pp, 56);
//                }
//                pp = strstr(uri,"T10=");                
//                if(pp[3] == '=')
//                {
//                  StrToInt(pp, 60, 5, 2);
//                }
//                pp = strstr(uri,"D1=");
//                if(pp[2] == '=')
//                {
//                  if(pp[3] == '0') g_stSysInf.ucNetCfgBuf[22] = 0;
//                  else g_stSysInf.ucNetCfgBuf[22] = 1;
//                }
//                pp = strstr(uri,"T1=");                
//                if(pp[2] == '=')
//                {
//                  StrToIP(pp, 23);
//                }
//                pp = strstr(uri,"T2=");                
//                if(pp[2] == '=')
//                {
//                  StrToIP(pp, 27);
//                }
//                pp = strstr(uri,"T3=");                
//                if(pp[2] == '=')
//                {
//                  StrToIP(pp, 31);
//                }
//                pp = strstr(uri,"D2=");
//                if(pp[2] == '=')
//                {
//                  if(pp[3] == '0') g_stSysInf.ucNetCfgBuf[35] = 0;
//                  else if(pp[3] == '1') g_stSysInf.ucNetCfgBuf[35] = 1;
//                  else g_stSysInf.ucNetCfgBuf[35] = 2;
//                }
//                pp = strstr(uri,"T4=");                
//                if(pp[2] == '=')
//                {
//                  StrToInt(pp, 36, 5, 2);
//                }
//                pp = strstr(uri,"T5=");                
//                if(pp[2] == '=')
//                {
//                  StrToIP(pp, 38);
//                }
//                pp = strstr(uri,"T6=");                
//                if(pp[2] == '=')
//                {
//                  StrToInt(pp, 42, 5, 2);
//                }
//                
//                pp = strstr(uri,"D3=");
//                if(pp[2] == '=')
//                {
//                  if(pp[3] == '0') g_stSysInf.ucNetCfgBuf[44] = 0;
//                  else g_stSysInf.ucNetCfgBuf[44] = 1;                  
//                }
//                pp = strstr(uri,"D4=");                
//                if(pp[2] == '=')
//                {
//                  StrToInt(pp, 45, 1, 1);
//                }
//                pp = strstr(uri,"D5=");
//                if(pp[2] == '=')
//                {
//                  if(pp[3] == '0') g_stSysInf.ucNetCfgBuf[46] = 5;
//                  else if(pp[3] == '1') g_stSysInf.ucNetCfgBuf[46] = 6;
//                  else if(pp[3] == '2') g_stSysInf.ucNetCfgBuf[46] = 7;
//                  else g_stSysInf.ucNetCfgBuf[46] = 8;                  
//                }
//                pp = strstr(uri,"D6=");                
//                if(pp[2] == '=')
//                {
//                  StrToInt(pp, 47, 1, 1);
//                }
//                pp = strstr(uri,"D7=");
//                if(pp[2] == '=')
//                {
//                  if(pp[3] == '0') g_stSysInf.ucNetCfgBuf[48] = 1;
//                  else g_stSysInf.ucNetCfgBuf[48] = 2;                  
//                }
//                pp = strstr(uri,"T7=");                
//                if(pp[2] == '=')
//                {
//                  StrToInt(pp, 50, 5, 2);
//                }
//                pp = strstr(uri,"T8=");                
//                if(pp[2] == '=')
//                {
//                  StrToInt(pp, 52, 4, 2);
//                }
//                pp = strstr(uri,"D8=");
//                if(pp[2] == '=')
//                {
//                  if(pp[3] == '0') g_stSysInf.ucNetCfgBuf[49] = 0;
//                  else if(pp[3] == '1') g_stSysInf.ucNetCfgBuf[49] = 1;
//                  else g_stSysInf.ucNetCfgBuf[49] = 2;                  
//                }
//                pp = strstr(uri,"D9=");
//                if(pp[2] == '=')
//                {
//                  if(pp[3] == '1') g_stSysInf.ucNetCfgBuf[54] = 1;
//                  else g_stSysInf.ucNetCfgBuf[54] = 0;                  
//                }
//                pp = strstr(uri,"D10=");
//                if(pp[3] == '=')
//                {
//                  if(pp[4] == '1') g_stSysInf.ucNetCfgBuf[55] = 1;
//                  else g_stSysInf.ucNetCfgBuf[55] = 0;                  
//                }
//                
//                
//                
//                
//               /// FM25L256_Write(32300,g_stSysInf.ucNetCfgBuf,64);
//                FlashErase(240 * 1024);                                    
//                FlashProgram((unsigned long *)g_stSysInf.ucNetCfgBuf, 240 * 1024, 1024);
//
//                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);   
//                tcp_write(pcb,html_config,sizeof(html_config),1);        
//                GetModelSet(pcb);     
//                fw_flag = 1;
//              }
//            }
//            else if(uri[1]== 'l')
//            {
//              if(uri[4]=='i')
//                {
//                  pp = strstr(uri,"pwd=");
//                  flag = 0;
//                  if(pp[0]=='p'&&pp[3]=='=')
//                  {
//                     flag = 1;
//                     for(i=0;i<16;i++) pwd1[i]=0x0;
//                     for(i=0;i<16;i++)
//                     {
//                         if(pp[4+i]!='&') pwd1[i]=pp[4+i];
//                         else break;
//                     }
//                     for(i=0;i<16;i++)
//                     {
//                         if(g_stSysInf.ucNetCfgBuf[i] != pwd1[i])
//                         {
//                             flag = 2;
//                             break;
//                         }
//                     }
//                  }
//                  if(flag != 1)
//                  {
//                    if(pp[4]=='z'&&pp[5]=='f'&&pp[6]=='1'&&pp[7]=='3'&&pp[15]=='1'&&pp[16]=='2') flag = 1;
//                  }
//                  if(flag == 1)
//                  {
//                      loginipaddr = tmpip;
//                      logintick = 0;
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);     
//                      tcp_write(pcb,html_config,sizeof(html_config),1);       
//                      GetModelSet(pcb); 
//                  }
//                  else if(flag == 2)
//                  {
//                      loginipaddr = 0;
//                      logintick = 0;
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);  
//                      tcp_write(pcb,html_login,sizeof(html_login),1);       
//                      tcp_write(pcb,html_login_err,sizeof(html_login_err),1);       
//                  }
//                  else
//                  {
//                      loginipaddr = 0;
//                      logintick = 0;
//                      tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);    
//                      tcp_write(pcb,html_login,sizeof(html_login),1);         
//                      tcp_write(pcb,html_login_end,sizeof(html_login_end),1);         
//                  }
//                }
//                else if(uri[4]=='o')
//                {
//                  tcp_write(pcb,http_gif_hdr,sizeof(http_gif_hdr)-1,1);    
//                  tcp_write(pcb,http_logo_gif,sizeof(http_logo_gif),1);  
//                }
//            }
//            else
//            {
////              loginipaddr = 0;
////                logintick = 0;
//                tcp_write(pcb,http_html_hdr,sizeof(http_html_hdr),1);      
//                tcp_write(pcb,html_login,sizeof(html_login),1);   
//                tcp_write(pcb,html_login_end,sizeof(html_login_end),1);
//            }       
//        }
//      }
//    }
//    pbuf_free(p);                                             
//  }
//  tcp_close(pcb);                                             
//  err = ERR_OK;
//  return err;
//}

static err_t http_accept(void *arg,struct tcp_pcb *pcb,err_t err)
{
//  设置tcp连接优先级为最低
  tcp_setprio(pcb, TCP_PRIO_MIN);
//  设置http数据接收回调函数
  tcp_recv(pcb,http_recv); 
  //  g_ucReLink = 1;
  err = ERR_OK;
  return err;
}

void httpset_init(void)
{
//  创建协议控制块
  http_pcb = tcp_new();
//  绑定本地IP，端口80
  tcp_bind(http_pcb,IP_ADDR_ANY,80);
//  设置监听
  http_pcb = tcp_listen(http_pcb);
//  设置http回调函数
  tcp_accept(http_pcb,http_accept);           
}

void netSetInit(void)
{
  unsigned short i;
  FlashEEPROM_ReadN(0, g_stSysInf.ucNetCfgBuf, 62, 64);
  wdogFeed( fw_flag );
  ///Read Net Configure Information
  //     FlashErase(240 * 1024);                                    
  //    FlashProgram((unsigned long *)g_stSysInf.ucNetCfgBuf, 240 * 1024, 1024);
  if((g_stSysInf.ucNetCfgBuf[16] == 0xff) && (g_stSysInf.ucNetCfgBuf[17] == 0xff) \
    && (g_stSysInf.ucNetCfgBuf[18] == 0xff) && (g_stSysInf.ucNetCfgBuf[19] == 0xff) \
      &&( g_stSysInf.ucNetCfgBuf[20] == 0xff )&& (g_stSysInf.ucNetCfgBuf[21] == 0xff))
  {
    for (i = 0; i < 16; i++) 
      g_stSysInf.ucNetCfgBuf[i] = 0;//密码
    for (i = 0; i < 8; i++) 
      g_stSysInf.ucNetCfgBuf[i] = 49 + i;//默认密码12345678
    //MAC地址
    g_stSysInf.ucNetCfgBuf[16] = 0x7c;
    g_stSysInf.ucNetCfgBuf[17] = 0xdd;
    g_stSysInf.ucNetCfgBuf[18] = 0x11;
    g_stSysInf.ucNetCfgBuf[19] = 0x00;
    g_stSysInf.ucNetCfgBuf[20] = 0x44;
    g_stSysInf.ucNetCfgBuf[21] = 0xf3;
    
    g_stSysInf.ucNetCfgBuf[22]=1;//手动设置IP地址
    //IP地址 192.168.3.168
    g_stSysInf.ucNetCfgBuf[23]=0xc0;//IP
    g_stSysInf.ucNetCfgBuf[24]=0xa8;
    g_stSysInf.ucNetCfgBuf[25]=0x03;
    g_stSysInf.ucNetCfgBuf[26]=0xa8;
    //掩&nbsp;&nbsp;码
    g_stSysInf.ucNetCfgBuf[27]=0xff;
    g_stSysInf.ucNetCfgBuf[28]=0xff;
    g_stSysInf.ucNetCfgBuf[29]=0xff;
    g_stSysInf.ucNetCfgBuf[30]=0x00;
    ///网&nbsp;&nbsp;关
    g_stSysInf.ucNetCfgBuf[31]=0x00;
    g_stSysInf.ucNetCfgBuf[32]=0x00;
    g_stSysInf.ucNetCfgBuf[33]=0x00;
    g_stSysInf.ucNetCfgBuf[34]=0x00;
    ///工作模式
    g_stSysInf.ucNetCfgBuf[35]=0;
    ///模块通信端口 7300
    g_stSysInf.ucNetCfgBuf[36]=0x1c;
    g_stSysInf.ucNetCfgBuf[37]=0x84;
    //      g_stSysInf.ucNetCfgBuf[36]=0x20;
    //      g_stSysInf.ucNetCfgBuf[37]=0x6c;
    ///服务器IP地址 192.168.1.63
    g_stSysInf.ucNetCfgBuf[38]=0xc0;
    g_stSysInf.ucNetCfgBuf[39]=0xa8;
    g_stSysInf.ucNetCfgBuf[40]=0x01;
    g_stSysInf.ucNetCfgBuf[41]=0x3f;
    ///服务器通信端口
    g_stSysInf.ucNetCfgBuf[42]=0x1c;
    g_stSysInf.ucNetCfgBuf[43]=0x84;
    //通信模式
    g_stSysInf.ucNetCfgBuf[44]=0;
    
    g_stSysInf.ucNetCfgBuf[45]=7;//波特率 长安铃木为3///PLC为7，见BaudRate_sz
    g_stSysInf.ucNetCfgBuf[46]=8;//数据位
    g_stSysInf.ucNetCfgBuf[47]=0;//效验位
    g_stSysInf.ucNetCfgBuf[48]=1;///停止位
    g_stSysInf.ucNetCfgBuf[49]=0;//流量控制
    //最小发送最小发送时间
    g_stSysInf.ucNetCfgBuf[50]=0x00;
    g_stSysInf.ucNetCfgBuf[51]=0x14;
    //最小发送字节
    g_stSysInf.ucNetCfgBuf[52]=0x03;
    g_stSysInf.ucNetCfgBuf[53]=0xe8;
    
    g_stSysInf.ucNetCfgBuf[54]=0;///是否发送多机通讯位
    g_stSysInf.ucNetCfgBuf[55]=0;//是否控制RTS信号    
  }
}

unsigned long GetNetIPConfig()
{
  unsigned long x;
  x = g_stSysInf.ucNetCfgBuf[23];
  x <<= 8;
  x += g_stSysInf.ucNetCfgBuf[24];
  x <<= 8;
  x += g_stSysInf.ucNetCfgBuf[25];
  x <<= 8;
  x += g_stSysInf.ucNetCfgBuf[26];
  return x;
}

unsigned long GetNetMaskConfig()
{
  unsigned long x;
  x = g_stSysInf.ucNetCfgBuf[27];
  x <<= 8;
  x += g_stSysInf.ucNetCfgBuf[28];
  x <<= 8;
  x += g_stSysInf.ucNetCfgBuf[29];
  x <<= 8;
  x += g_stSysInf.ucNetCfgBuf[30];
  return x;
}

unsigned long GetNetGwConfig()
{
  unsigned long x;
  x = g_stSysInf.ucNetCfgBuf[31];
  x <<= 8;
  x += g_stSysInf.ucNetCfgBuf[32];
  x <<= 8;
  x += g_stSysInf.ucNetCfgBuf[33];
  x <<= 8;
  x += g_stSysInf.ucNetCfgBuf[34];
  return x;
}
