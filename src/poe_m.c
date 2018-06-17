#include "i2c.h"
#include "uart.h"
#include "poe_m.h"
#include "tt9980.h"
#include "STC8xxxx.h"

static uint16_t xdata pwrled_time = 0;	//led flash time

static uint8_t i2c_salve[MAX_DEVICE] = {TT9980_1_4, TT9980_5_8, TT9980_9_12, TT9980_13_16};

static uint8_t xdata den_start = 0;			//d enable gradually satrt

static void set_l(uint8_t dev, uint8_t ch, bit val);
static void g_disable(void);
static void g_enable(void);
static void set_den(uint8_t sta);

void delay(uint8_t time);
void debug_delay(uint16_t time);

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
// version: 		V1.0, 2018-5-28
//========================================================================
void system_init(void){
	uint8_t xdata dev=0, ch=0, state = 0;
	uint8_t xdata ret = 0, trys = 3;
	
	for(dev=0; dev<MAX_DEVICE; dev++)
		for(ch=0; ch<MAX_CH; ch++)
			set_l(dev, ch, L_ON);					//led on : all 
	
	PWR_LED = PWR_LED_ON;
	debug_delay(0x4FFF);
	for(dev=0; dev<MAX_DEVICE; dev++)
		for(ch=0; ch<MAX_CH; ch++)
			set_l(dev, ch, L_OFF);					//led off : all 
	
	PWR_LED = PWR_LED_OFF;
	pwrled_time = PWR_LED_STOP;			//pwrled off
	
	set_den(0xFF);
	
	state = 0x01;
	for(dev=0; dev<MAX_DEVICE; dev++){	//set 2th class
		for(ch=0; ch<MAX_CH; ch++){
			trys = 3;
			do{
				ret = i2c_write(i2c_salve[dev], GPDM(ch), &state, 1);
			}while(ret && trys--);
		}
	}
	
	state = 0x00;
	for(dev=0; dev<MAX_DEVICE; dev++){	//set pwr on
		trys = 3;
		do{
			ret = i2c_write(i2c_salve[dev], PWR_ON, &state, 1);
		}while(ret && trys--);
	}
	
	state = 0xFF;
	for(dev=0; dev<MAX_DEVICE; dev++){	//set work mode
		trys = 3;
		do{
			ret = i2c_write(i2c_salve[dev], WORK_MODE, &state, 1);
		}while(ret && trys--);
	}
}

//========================================================================
// function:		timeEv_open_den
// description:	d enable gradually
// parameter: 	void
// return: 			void
// version: 		V1.0, 2018-5-28
//========================================================================
void timeEv_open_den(uint8_t tick){
	static uint16_t xdata den_tick = 0;
	if(den_start >= ALL_CH)
		return;
	den_tick += tick;
	if(den_tick > T_EN_D){
		g_enable();
		den_start++;
	}
}

//========================================================================
// function:		timeEv_pwrled
// description:	pwr led flash
// parameter: 	tick
// return: 			void
// version: 		V2.0, 2018-4-25
//========================================================================
void timeEv_pwrled(uint8_t tick){
	static uint16_t xdata pwrled_tick = 0;
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
// version: 		V2.0, 2018-5-25
//========================================================================
void timeEv_getGsta(uint8_t tick){
	static uint16_t xdata getg_tick = 0;
	static uint8_t xdata g_slave = 0;
	uint8_t state = 0, ch = 0, ret = 0;
	getg_tick += tick;
	if(getg_tick > T_GET_G){
		getg_tick = 0;
		ret = i2c_read(i2c_salve[g_slave], PWR_STATE, &state, 1);
		if(ret)
			state = G_OFF<<0 | G_OFF<<1 | G_OFF<<2 | G_OFF<<3; //if i2c_err, then led_off
		for(ch=0; ch<MAX_CH; ch++){
			if(((state>>ch)&0x01) == G_ON)
				set_l(g_slave, ch, L_ON);
			else
				set_l(g_slave, ch, L_OFF);
		}
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
	uint8_t trys = 0, state = 0, ret = 0;
	getiu_tick += tick;
	if(getiu_tick > T_GET_IU){
		uint8_t dev = 0, ch = 0;
		uint32_t sum_iu = 0;
		getiu_tick = 0;
		for(dev=0; dev<MAX_DEVICE; dev++){
			uint8_t ret = 0;
			uint8_t pbuf[U4_H-U1_L+1] = {0};
			ret = i2c_read(i2c_salve[dev], I1_L, pbuf, U4_H-I1_L+1);
			if(!ret){
				for(ch=0; ch<MAX_CH; ch++)
					sum_iu += ((uint16_t)pbuf[ch<<2] | ((uint16_t)pbuf[(ch<<2)+1]<<8));
			}
		}
		if(sum_iu > IU_MAX){					// >100%
//			set_den(0x00);									// d_off : all
			g_disable();								// g_off + d_off : one
			state = 0xAA;
			for(dev=0; dev<MAX_DEVICE; dev++){	//set work mode
				trys = 3;
				do{
					ret = i2c_write(i2c_salve[dev], WORK_MODE, &state, 1);
				}while(ret && trys--);
			}
			den_start = ALL_CH;
			pwrled_time = PWR_LED_MAX;
		}
		else if(sum_iu > IU_MID){			// >95%
			//beta
			state = 0x00;
			for(dev=0; dev<MAX_DEVICE; dev++){
				trys = 3;
				do{
					ret = i2c_write(i2c_salve[dev], PWR_ON, &state, 1);
				}while(ret && trys--);
			}
//			set_den(0x00);									// d_off : all
			//beta
			state = 0xAA;
			for(dev=0; dev<MAX_DEVICE; dev++){	//set work mode
				trys = 3;
				do{
					ret = i2c_write(i2c_salve[dev], WORK_MODE, &state, 1);
				}while(ret && trys--);
			}
			den_start = ALL_CH;
			pwrled_time = PWR_LED_FAST;
		}
		else if(sum_iu > IU_NOR){			// >75%
			//beta
			state = 0x0F;
			for(dev=0; dev<MAX_DEVICE; dev++){
				trys = 3;
				do{
					ret = i2c_write(i2c_salve[dev], PWR_ON, &state, 1);
				}while(ret && trys--);
			}
//			set_den(0xFF);
			//beta
			state = 0xFF;
			for(dev=0; dev<MAX_DEVICE; dev++){	//set work mode
				trys = 3;
				do{
					ret = i2c_write(i2c_salve[dev], WORK_MODE, &state, 1);
				}while(ret && trys--);
			}
			den_start = ALL_CH;
			pwrled_time = PWR_LED_SLOW;
		}
		else{													// <=75%
			//beta
			state = 0x0F;
			for(dev=0; dev<MAX_DEVICE; dev++){
				trys = 3;
				do{
					ret = i2c_write(i2c_salve[dev], PWR_ON, &state, 1);
				}while(ret && trys--);
			}
//			set_den(0xFF);
			//beta
			state = 0xFF;
			for(dev=0; dev<MAX_DEVICE; dev++){	//set work mode
				trys = 3;
				do{
					ret = i2c_write(i2c_salve[dev], WORK_MODE, &state, 1);
				}while(ret && trys--);
			}
			pwrled_time = PWR_LED_STOP;
			PWR_LED = PWR_LED_OFF;
			if(den_start<ALL_CH) return;
//			g_enable();									// g_on + d_on : one
		}
	}
}

//========================================================================
// function:		g_disable
// description:	close gate witch is ON and lower previlige
// parameter: 	void
// return: 			closed flag
// version: 		V1.0, 2018-5-28
//========================================================================
static void g_disable(void){
	char xdata ret = 0, dev = 0, ch = 0;
	uint8_t g_state = 0, d_state = 0, close_state = 0;
	for(dev=MAX_DEVICE-1; dev>=0; dev--){
		uint8_t ret = 0, trys = 3;
		do{
			ret = i2c_read(i2c_salve[dev], PWR_STATE, &g_state, 1);
		}while(ret && trys--);
		if(ret) continue;
		for(ch=MAX_CH-1; ch>=0; ch--){
			if((g_state>>ch)&0x01 == G_ON){
				//test
				TX1_write2buff('d');
				TX1_write2buff('1'+dev);
				TX1_write2buff('1'+ch);
				TX1_write2buff('\n');
				//test
				close_state |= (0x10<<ch);
				trys = 3;
				do{
					ret = i2c_write(i2c_salve[dev], PWR_ON, &close_state, 1);
				}while(ret && trys--);
				return;
			}
		}
	}
}

//========================================================================
// function:		g_enable
// description:	open gate witch is OFF and higher previlige
// parameter: 	void
// return: 			closed flag
// version: 		V1.0, 2018-5-28
//========================================================================
static void g_enable(void){
	char xdata ret = 0, dev = 0, ch = 0;
	uint8_t d_state = 0, g_state = 0, pwr_state = 0;
	for(dev=0; dev<MAX_DEVICE; dev++){
		uint8_t ret = 0, trys = 3;
		do{
			ret = i2c_read(i2c_salve[dev], PWR_STATE, &pwr_state, 1);
		}while(ret && trys--);
		if(ret) continue;
		for(ch=0; ch<MAX_CH; ch++){
			if(((pwr_state>>ch)&0x01) == 0){
				//test
				TX1_write2buff('e');
				TX1_write2buff('1'+dev);
				TX1_write2buff('1'+ch);
				TX1_write2buff('\n');
				//test
				g_state = 0x0F;
				trys = 3;
				do{
					ret = i2c_write(i2c_salve[dev], PWR_ON, &g_state, 1);
				}while(ret && trys--);
				return;
			}
		}
	}
}

//========================================================================
// function:		set_den
// description:	set d enable / disable
// parameter: 	void
// return: 			void
// version: 		V1.0, 2018-5-28
//========================================================================
static void set_den(uint8_t sta){
	uint8_t xdata dev = 0, ret = 0, trys = 3;
//	static uint8_t xdata den_flag[MAX_DEVICE] = {0};		//d flag
	for(dev=0; dev<MAX_DEVICE; dev++){
//		if(den_flag[dev] == sta) continue;
//		den_flag[dev] = sta;
		trys = 3;
		do{
			ret = i2c_write(i2c_salve[dev], DET_EN, &sta, 1);
		}while(ret && trys--);
	}
}

//========================================================================
// function:		set_l
// description:	set led on/off
// parameter: 	device, channel, state
// return: 			void
// version: 		V2.0, 2018-5-25
//========================================================================
static void set_l(uint8_t dev, uint8_t ch, bit val){
	uint8_t l = 0xff;
	if(dev<MAX_DEVICE && ch<MAX_CH)
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

