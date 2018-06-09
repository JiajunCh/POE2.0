#include "tt9980.h"
#include "i2c.h"
#include "STC8xxxx.H"
/*
void set_DET(uint8_t dev, uint8_t ch, uint8_t state){
	uint8_t old_state = 0, ret = 0, trys = 3;
	do{
		ret = i2c_read(TT9980x_ADDR+dev, DET_EN, &old_state, 1);
	}while(ret && trys--);
	if(!ret){
		if(state)
			state = 0x11<<ch;
		else
			state = 0;
		old_state &= ~state;
		old_state |= state;
		trys = 3;
		do{
			ret = i2c_write(TT9980x_ADDR+dev, DET_EN, &old_state, 1);
		}while(ret && trys--);
	}
}
*/