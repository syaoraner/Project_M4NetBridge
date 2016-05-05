#ifndef __HTTPWEB_H__
#define __HTTPWEB_H__

struct realdata{
  long adval[16];//0-9V1 1-5V1 2-A/D 
  long wd_cpu;   //�ڲ�cpu�¶�
  long wd_18b20; //����18B20�¶�
  unsigned char io_i[16];//0-����1 1-����2 2-S4 3-S5 
  unsigned char io_o[16];//0-�̵���1 1-�̵���2 2-���� 3-ON 4-OFF
  unsigned char io_s[16];//�ֶ��������
};

void httpset_init(void);//��ʼ��WEB������
void netSetInit(void);//��ʼ���������û�����
unsigned long GetNetIPConfig();//��ȡIP��ַ
unsigned long GetNetMaskConfig();//��ȡ����
unsigned long GetNetGwConfig();//��ȡ����

#endif