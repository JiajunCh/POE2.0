#include "poe_m.h"
#include "uart.h"
#include "timer.h"
#include "i2c.h"
#include "STC8xxxx.h"

//========================================================================
// function:		any delay function
// description:	delay time in while
// parameter: 	void
// return: 			void
// version: 		V1.0, 2018-1-11
//========================================================================
void delay(uint8_t time){
	for(;time>0;time--);
}
void debug_delay(uint8_t time){
	for(;time>0;time--)
		delay(255);
}

//========================================================================
// function:		timeEvent_Process
// description:	cyclicity_event and deferred_event proccess
// parameter: 	a timerx tick
// return: 			void
// version: 		V2.0, 2018-4-24
//========================================================================
void timeEvent_Process(uint8_t new_tick){
	static uint8_t old_tick = 0;
	static uint8_t cnt_tick = 0;
	cnt_tick = new_tick-old_tick;
	if(0 == cnt_tick) return;
	old_tick = new_tick;

#if (DEBUG)
	timeEv_UARTRx1Rst(cnt_tick);
#endif
	timeEv_getGsta(cnt_tick);
	timeEv_pwrled(cnt_tick);
	timeEv_open_den(cnt_tick);
	timeEv_getIU(cnt_tick);
}

//========================================================================
// function:		main
// description:	main
// parameter: 	void
// return: 			void
// version: 		V2.0, 2018-4-24
//========================================================================
void main(void){
	EAXSFR();
	PORT_INIT();
	TIMER1_config();
	WDG_config();
	i2c_config();
#if (DEBUG)
	UART1_config();
#endif
	EA = 1;	//enable interuupt_
	while(1){
#if (DEBUG)
		UART1_RxProcess();
		UART1_TxProcess();
#endif
		timeEvent_Process(get_timer1tick());
		WDG_freed();
	}
}

