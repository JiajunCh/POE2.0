#ifndef _POE_M_H_
#define _POE_M_H_

#include "stc8xxxx.h"
//************************************************************************
#define DEBUG					(1)			//���Կ��� 1����0��
#define PWR_LED_MAX		(40)		//pwr led��������(ms)��s��ѹ����100%
#define PWR_LED_FAST	(150)		//pwr led��������(ms)��s��ѹ����95%
#define PWR_LED_SLOW	(500)		//pwr led��������(ms)��s��ѹ����75%
#define T_GET_G				(100) 	//�ռ������豸G״̬����(ms)
#define T_GET_IU			(500) 	//�����ɼ�ÿ���豸�ĵ�����ѹʱ����(ms)

#define IU_UP					(200000ul)	//�ɼ������ܺ���Чֵ���ޣ����ڴ�ֵ���Ա��βɼ�
#define CNT_IUMAX			(2)				//�����ɼ���n�δ���IU_MAX�Źر�һ���˿�
#define CNT_ERR				(10)			//�����ɼ�����n�ε�Ƭ��������������ֵ���ڵ���256�˹���ʧЧ
#define IU_MAX				(8000ul) //����ع���100%  //ȫ���˿ڵ�����ʽ IU_MAX = �˿ڵ����ܺ�(uA)/122.07
															//���� IU_MAX = 3A(3 000 000 uA) / 122.07 = 24576
#define IU_MID				(IU_MAX * 60 / 100) //����ع���95%
#define IU_NOR				(IU_MAX * 30 / 100) //����ع���75%
//************************************************************************

#define PRJ_NAME	"poe_led\n"
#define SW_VER		"Sw_Ver:V2.1\n"
#define HW_VER		"Hw_Ver:V2.0\n"

#define SYSCLK 			(24000000)
#define MAX_CH			(4)
#define MAX_DEVICE	(4)
#define ALL_CH			(MAX_CH*MAX_DEVICE)

//G : 1==on,0==off
#define G_ON  			(1)
#define G_OFF				(0)

//power max led flash time
#define PWR_LED_STOP	(0xffff)

//power max led ttl
#define PWR_LED_ON	(0)
#define PWR_LED_OFF	(1)
sbit PWR_LED	= P1^6;

//based output : 1==off,0==on
#define L_ON  			(0)
#define L_OFF				(1)
sbit L1		= P2^5;
sbit L2		= P2^4;
sbit L3		= P2^3;
sbit L4		= P2^2;

sbit L5		= P0^1;
sbit L6		= P0^0;
sbit L7		= P2^7;
sbit L8		= P2^6;

sbit L9		= P0^4;
sbit L10	= P0^3;
sbit L11	= P4^3;
sbit L12	= P0^2;

sbit L13	= P1^0;
sbit L14	= P0^7;
sbit L15	= P0^6;
sbit L16	= P0^5;

sbit L17	= P1^2;
sbit L18	= P1^1;
sbit L19	= P5^3;
sbit L20	= P5^2;

sbit L21	= P1^5;
sbit L22	= P4^4;
sbit L23	= P1^4;
sbit L24	= P1^3;

#define PORT_INIT()	\
P0M0=0xFF;	P0M1=0x00;\
P1M0=0x3F;	P1M1=0x00;\
P2M0=0xFC;	P2M1=0x00;\
P3M0=0x00;	P3M1=0xC0;\
P4M0=0x18;	P4M1=0x00;\
P5M0=0x0C;	P5M1=0x00;

extern uint8_t i2c_salve[];

void delay(uint8_t time);
void debug_delay(uint16_t time);

void WDG_config(void);
void WDG_freed(void);
void system_init(void);
void timeEv_pwrled(uint8_t tick);
void timeEv_getGsta(uint8_t tick);
void timeEv_getIU(uint8_t tick);

#endif

