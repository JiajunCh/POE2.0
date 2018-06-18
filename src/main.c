#include "poe_m.h"
#include "uart.h"
#include "timer.h"
#include "i2c.h"
#include "STC8xxxx.h"

//========================================================================
// function:		timeEvent_Process
// description:	cyclicity_event and deferred_event proccess
// parameter: 	a timerx tick
// return: 			void
// version: 		V2.0, 2018-6-18
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
	timeEv_getIU(cnt_tick);
}

//========================================================================
// function:		main
// description:	main
// parameter: 	void
// return: 			void
// version: 		V2.0, 2018-6-18
//========================================================================
void main(void){
	PORT_INIT();
	TIMER1_config();
	WDG_config();
	i2c_config();
#if (DEBUG)
	UART1_config();
#endif
	system_init();
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

