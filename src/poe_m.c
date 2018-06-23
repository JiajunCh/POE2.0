#include "i2c.h"
#include "uart.h"
#include "poe_m.h"
#include "tt9980.h"
#include "STC8xxxx.h"

static uint16_t xdata pwrled_time = 0;	//led flash time

uint8_t i2c_salve[MAX_DEVICE] = {TT9980_1_4, TT9980_5_8, TT9980_9_12, TT9980_13_16};

static void lowprio_off(void);
static void highprio_on(void);
static void set_led(uint8_t dev, uint8_t ch, bit val);
static void set_allled(bit val);

//========================================================================
// function:		any delay function
// description:	delay time in while
// parameter: 	tick
// return: 			void
// version: 		V2.0, 2018-6-18
//========================================================================
void delay(uint8_t time){
	for(;time>0;time--);
}
void debug_delay(uint16_t time){
	for(;time>0;time--)
		delay(0xFF);
}

//========================================================================
// function:		WDG_config
// description:	start wdg
// parameter: 	void
// return: 			void
// version: 		V1.0, 2018-1-11
//========================================================================
void WDG_config(void){
	WDT_CONTR = D_EN_WDT | D_WDT_SCALE_128;
}

//========================================================================
// function:		WDG_freed
// description:	wdg clear
// parameter: 	void
// return: 			void
// version: 		V1.0, 2018-1-11
//========================================================================
void WDG_freed(void){
	WDT_CONTR |= D_CLR_WDT;
}

//========================================================================
// function:		system_init
// description:	system init state
// parameter: 	void
// return: 			void
// version: 		V2.0, 2018-6-18
//========================================================================
void system_init(void){
	uint8_t xdata ch=0;
	
	set_allled(L_ON);	//led on : all 
	PWR_LED = PWR_LED_ON;	//pwrled on
	debug_delay(0x2FFF);
	set_allled(L_OFF);	//led off : all 
	PWR_LED = PWR_LED_OFF;	//pwrled off
	pwrled_time = PWR_LED_STOP;			//pwrled off
	
	ttall_write(WORK_MODE, MODE_HFAUTO);	//set work mode
	ttall_write(DET_EN, 0xFF);	//open det/class
	ttall_write(PWR_ON, 0x0F);	//power on
	for(ch=0; ch<MAX_CH; ch++)
		ttall_write(GPDM(ch), 0x01);	//set class level_2
	
	PrintString1(PRJ_NAME);	//test
}

//========================================================================
// function:		timeEv_pwrled
// description:	pwr led flash
// parameter: 	tick
// return: 			void
// version: 		V2.0, 2018-6-18
//========================================================================
void timeEv_pwrled(uint8_t tick){
	static uint16_t xdata pwrled_tick = 0;
	if(PWR_LED_STOP == pwrled_time)
		return;
	pwrled_tick += tick;
	if(pwrled_tick > pwrled_time){
		pwrled_tick = 0;
		PWR_LED = !PWR_LED;
	}
}

//========================================================================
// function:		timeEv_getGsta
// description:	uart1 interrupt_ service handle
// parameter: 	tick
// return: 			void
// version: 		V2.0, 2018-6-18
//========================================================================
void timeEv_getGsta(uint8_t tick){
	static uint16_t xdata getg_tick = 0;
	static uint8_t dev = 0;
	uint8_t state = 0, ch = 0, ret = 0;
	getg_tick += tick;
	if(getg_tick > T_GET_G){
		getg_tick = 0;
//		TX1_write2buff('g');	//test
		ret = tt_read(dev, PWR_STATE, &state);
		if(ret){
//			TX1_write2buff('-');	//test
			state = (G_OFF ? 0xFF : 0x00);	//if i2c_err, then led_off
		}
//		else
//			TX1_write2buff('|');	//test
//		TX1_write2buff('0'+(state&0x0F));	//test
		for(ch=0; ch<MAX_CH; ch++){
			if(((state>>ch)&0x01) == G_ON)
				set_led(dev, ch, L_ON);
			else
				set_led(dev, ch, L_OFF);
		}
		if((++dev) >= MAX_DEVICE)
			dev = 0;
//		TX1_write2buff('\n');	//test
	}
}

//========================================================================
// function:		timeEv_getIU
// description:	get slave current and voltage
// parameter: 	tick
// return: 			void
// version: 		V2.0, 2018-6-18
//========================================================================
void timeEv_getIU(uint8_t tick){
	static uint16_t xdata getiu_tick = 0;
	uint8_t xdata ret = 0;
	getiu_tick += tick;
	if(getiu_tick > T_GET_IU){
		uint8_t dev = 0, ch = 0;
		uint32_t sum_iu = 0;
		getiu_tick = 0;
//		TX1_write2buff('i');//test
		for(dev=0; dev<MAX_DEVICE; dev++){			//calculate sum of i*u
			uint8_t pbuf[U4_H-U1_L+1] = {0};
			ret = i2c_read(i2c_salve[dev], I1_L, pbuf, U4_H-I1_L+1);
			if(!ret){
//				TX1_write2buff('|');//test
				for(ch=0; ch<MAX_CH; ch++){
					sum_iu += (uint16_t)pbuf[ch<<2] | ((uint16_t)pbuf[(ch<<2)+1]<<8);
				}
			}
//			else	//test
//				TX1_write2buff('-');//test
		}
#if (DEBUG)
	{
		uint32_t test_sum = sum_iu;
		TX1_write2buff('i');
		TX1_write2buff('=');
		while(test_sum){
			TX1_write2buff('0'+(test_sum%10));//test
			test_sum /= 10;
		}
	}
#endif
		
		if(sum_iu > IU_MAX){			// >100%
			pwrled_time = PWR_LED_MAX;
			lowprio_off();
//			TX1_write2buff('M');//test
		}
		else if(sum_iu > IU_MID){	// >95%
			pwrled_time = PWR_LED_FAST;
//			TX1_write2buff('m');//test
		}
		else if(sum_iu > IU_NOR){	// >75%
			pwrled_time = PWR_LED_SLOW;
			ttall_write(DET_EN, 0xFF);
//			highprio_on();
			ttall_write(PWR_ON, 0x0F);
//			TX1_write2buff('S');//test
		}
		else{											// <=75%
			pwrled_time = PWR_LED_STOP;
			PWR_LED = PWR_LED_OFF;
			ttall_write(DET_EN, 0xFF);
//			highprio_on();
			ttall_write(PWR_ON, 0x0F);
//			TX1_write2buff('s');//test
		}
//		TX1_write2buff('\n');//test
	}
}

//========================================================================
// function:		lowprio_off
// description:	close gate witch is ON and lower previlige
// parameter: 	void
// return: 			closed flag
// version: 		V1.0, 2018-6-18
//========================================================================
static void lowprio_off(void){
	char xdata ret = 0, dev = 0, ch = 0;
	uint8_t pwr_state = 0, g_state = 0;
	for(dev=MAX_DEVICE-1; dev>=0; dev--){
		ret = tt_read(dev, PWR_STATE, &pwr_state);	//read pwr state
		if(ret) continue;
		for(ch=MAX_CH-1; ch>=0; ch--){
			if((pwr_state>>ch)&0x01 == G_ON){		//get pwr_on channal
				g_state |= (0x10<<ch);
				tt_write(dev, PWR_ON, g_state);	//set pwr on/off
				return;
			}
		}
	}
}

//========================================================================
// function:		highprio_on
// description:	open gate witch is OFF and higher previlige
// parameter: 	void
// return: 			closed flag
// version: 		V1.0, 2018-6-18
//========================================================================
static void highprio_on(void){
	uint8_t xdata ret = 0, dev = 0, ch = 0;
	uint8_t pwr_state = 0, g_state = 0;
	for(dev=0; dev<MAX_DEVICE; dev++){
		ret = tt_read(dev, PWR_STATE, &pwr_state);	//read pwr state
		if(ret)
			continue;
		for(ch=0; ch<MAX_CH; ch++){
			if(((pwr_state>>ch)&0x01) == G_OFF){
				g_state |= (0x01<<ch);
				tt_write(dev, PWR_ON, g_state);	//set pwr on/off
				return;
			}
		}
	}
}

//========================================================================
// function:		set_led
// description:	set led on/off
// parameter: 	device, channel, state
// return: 			void
// version: 		V2.0, 2018-6-18
//========================================================================
static void set_led(uint8_t dev, uint8_t ch, bit val){
	uint8_t l = 0xff;
	if(dev<6 && ch<MAX_CH)
		l = dev*MAX_CH+ch;
	l += 8;
	switch(l){
		case 0:  L1 = val; break;
		case 1:  L2 = val; break;
		case 2:  L3 = val; break;
		case 3:  L4 = val; break;
		case 4:  L5 = val; break;
		case 5:  L6 = val; break;
		case 6:  L7 = val; break;
		case 7:  L8 = val; break;
		case 8:  L9 = val; break;
		case 9:  L10 = val; break;
		case 10: L11 = val; break;
		case 11: L12 = val; break;
		case 12: L13 = val; break;
		case 13: L14 = val; break;
		case 14: L15 = val; break;
		case 15: L16 = val; break;
		case 16: L17 = val; break;
		case 17: L18 = val; break;
		case 18: L19 = val; break;
		case 19: L20 = val; break;
		case 20: L21 = val; break;
		case 21: L22 = val; break;
		case 22: L23 = val; break;
		case 23: L24 = val; break;
		default:break;
	}
}

//========================================================================
// function:		set_lalled
// description:	set led on/off
// parameter: 	state
// return: 			void
// version: 		V2.0, 2018-6-18
//========================================================================
static void set_allled(bit val){
	uint8_t dev, ch;
	for(dev=0; dev<MAX_DEVICE; dev++)
		for(ch=0; ch<MAX_CH; ch++)
			set_led(dev, ch, val);					//set led all 
}

