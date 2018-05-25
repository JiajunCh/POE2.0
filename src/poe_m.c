#include "i2c.h"
#include "poe_m.h"
#include "tt9980.h"
#include "STC8xxxx.h"

static uint16_t xdata pwrled_time = 0;	//led flash time

static void set_l(uint8_t dev, uint8_t ch, bit val);

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
// function:		timeEv_pwrled
// description:	pwr led flash
// parameter: 	tick
// return: 			void
// version: 		V2.0, 2018-4-25
//========================================================================
void timeEv_pwrled(uint8_t tick){
	static uint16_t pwrled_tick = 0;
	pwrled_tick += tick;
	if(PWR_LED_STOP == pwrled_time)
		return;
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
// version: 		V2.0, 2018-4-25
//========================================================================
void timeEv_getGsta(uint8_t tick){
	static uint16_t xdata getg_tick = 0;
	static uint8_t xdata g_slave = 0;
	uint8_t state = 0, ch = 0;
	getg_tick += tick;
	if(getg_tick > T_GET_G){
		getg_tick = 0;
		i2c_read(TT9980x_ADDR+g_slave, PWR_STATE, &state, 1);
		if(G_ON != L_ON) state = ~state;
		for(ch=0; ch<MAX_CH; ch++)
			set_l(g_slave, ch, (bit)(state>>ch));
		if(++g_slave >= MAX_DEVICE)
			g_slave = 0;
	}
}

//========================================================================
// function:		timeEv_getIU
// description:	get slave current and voltage
// parameter: 	tick
// return: 			void
// version: 		V2.0, 2018-5-25
//========================================================================
void timeEv_getIU(uint8_t tick){
	static uint16_t xdata getiu_tick = 0;
	uint8_t xdata iu_buf[(U4_H-U1_L+1)*MAX_DEVICE] = {0};
	getiu_tick += tick;
	if(getiu_tick > T_GET_IU){
		uint8_t dev = 0, ch = 0;
		uint32_t sum_iu = 0;
		uint8_t *pbuf = iu_buf;
		getiu_tick = 0;
		for(dev=0; dev<MAX_DEVICE; dev++){
			uint8_t ret = i2c_read(TT9980x_ADDR+dev, I1_L, iu_buf+dev*MAX_CH, U4_H-I1_L+1);
			if(ret) continue;
		}
		for(dev=0; dev<MAX_DEVICE; dev++){
			for(ch=0; ch<MAX_CH; ch++){
				sum_iu += (*(pbuf)|*(pbuf+1)) * (*(pbuf+2)|*(pbuf+3));
				pbuf += 4;
			}
		}
		sum_iu /= 1000000;
		if(sum_iu > IU_MAX100){
			pwrled_time = PWR_LED_MAX;
		}
		else if(sum_iu > IU_MAX95){
			pwrled_time = PWR_LED_FAST;
		}
		else if(sum_iu > IU_MAX75){
			pwrled_time = PWR_LED_SLOW;
		}
		else{
			pwrled_time = PWR_LED_STOP;
			PWR_LED = PWR_LED_OFF;
		}
	}
}

//========================================================================
// function:		set_l
// description:	set led on/off
// parameter: 	device, channel, state
// return: 			void
// version: 		V2.0, 2018-4-25
//========================================================================
static void set_l(uint8_t dev, uint8_t ch, bit val){
	uint8_t l = 0xff;
	if(dev<MAX_DEVICE && ch<MAX_CH)
		l = dev*MAX_CH+ch;
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



