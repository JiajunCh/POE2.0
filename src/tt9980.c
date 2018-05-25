#include "tt9980.h"
#include "i2c.h"
#include "STC8xxxx.H"

void set_WorkMode(uint8_t slave, uint8_t mode){
	i2c_write(TT9980x_ADDR+slave, WORK_MODE, &mode, 1);
}

void set_DetAuto(uint8_t slave, uint8_t det){
	i2c_write(TT9980x_ADDR+slave, DET_EN, &det, 1);
}

uint8_t get_PwrState(uint8_t slave, uint8_t* state){
	uint8_t ret=0;
	ret = i2c_read(TT9980x_ADDR+slave, PWR_STATE, state, 1);
	return ret;
}

uint8_t get_Current(uint8_t slave, uint8_t* pbuf){
	uint8_t ret=0;
	ret = i2c_read(TT9980x_ADDR+slave, I1_L, pbuf, U4_H-I1_L+1);
	return ret;
}

