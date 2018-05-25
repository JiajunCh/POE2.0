#ifndef _POE_M_H_
#define _POE_M_H_

#include "stc8xxxx.h"
//************************************************************************
#define DEBUG					(1)			//调试开关 1开，0关
#define PWR_LED_MAX		(100)		//pwr led快闪周期(ms)：s电压大于100%
#define PWR_LED_FAST	(250)		//pwr led快闪周期(ms)：s电压大于90%
#define PWR_LED_SLOW	(750)		//pwr led慢闪周期(ms)：s电压大于80%
#define T_GET_G				(500) 	//收集单个设备G状态周期(ms)
#define SUM_ADC				(MAX_DEVICE * 1200) 	//D极ADC总和极限(ms)
#define T_GET_IU			(500) 	//轮流采集每个设备的电流电压时间间隔
#define IU_MAX100			(133*24) //最大负载功率100%  //单个端口功率公式 IU_MAX100 = 最大功率(毫瓦)/122.07/5.835,最后乘设备个数
#define IU_MAX95			(IU_MAX100*95/100) //最大负载功率95%
#define IU_MAX75			(IU_MAX100*75/100) //最大负载功率75%
//************************************************************************

#define SUM_ADC90			(SUM_ADC*100/90)			//90% SUM_ADC
#define SUM_ADC80			(SUM_ADC*100/80)			//80% SUM_ADC

#define PRJ_NAME	"poe_master\n"
#define SW_VER		"Sw_Ver:V2.0\n"
#define HW_VER		"Hw_Ver:V2.0\n"

#define SYSCLK 			(24000000)
#define MAX_CH			(4)
#define MAX_DEVICE	(4)

#define CAST_8(addr)	(*(volatile unsigned char* addr))
#define CAST_16(addr)	(*(volatile unsigned int* addr))

//slave I2C_Reg based address	in single device
#define RO_ADDR			(0)
#define RW_ADDR			(20)
//read only
#define G_STA8			(RO_ADDR)		//G pin state based address in I2C_Reg
#define D_V16				(G_STA8 + MAX_CH)	//D pin adc
#define RO_END			(D_V16 + MAX_CH*2)	//RO_END < RW_ADDR
//read write
#define D_EN8				(RW_ADDR)	//d pin detect enable
#define G_EN8				(D_EN8+1)	//g pin on enable
#define RW_END			(G_EN8 + MAX_CH)	//RW_END < REG_SIZE

#define DG_EN				(1)
#define DG_DIS			(0)

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
sbit L1		= P2^2;
sbit L2		= P2^3;
sbit L3		= P2^4;
sbit L4		= P2^5;
sbit L5		= P2^6;
sbit L6		= P2^7;
sbit L7		= P0^0;
sbit L8		= P0^1;
sbit L9		= P0^2;
sbit L10	= P4^3;
sbit L11	= P0^3;
sbit L12	= P0^4;
sbit L13	= P0^5;
sbit L14	= P0^6;
sbit L15	= P0^7;
sbit L16	= P1^0;
sbit L17	= P5^2;
sbit L18	= P5^3;
sbit L19	= P1^1;
sbit L20	= P1^2;
sbit L21	= P1^3;
sbit L22	= P1^4;
sbit L23	= P4^4;
sbit L24	= P1^5;

#define PORT_INIT()	\
P0M0=0xFF;	P0M1=0x00;\
P1M0=0x3F;	P1M1=0x00;\
P2M0=0xFC;	P2M1=0x00;\
P3M0=0x00;	P3M1=0xC0;\
P4M0=0x18;	P4M1=0x00;\
P5M0=0x0C;	P5M1=0x00;

void WDG_config(void);
void WDG_freed(void);
void timeEv_pwrled(uint8_t tick);
void timeEv_getGsta(uint8_t tick);

#endif

