#include "tt9980.h"
#include "i2c.h"
#include "poe_m.h"
#include "STC8xxxx.H"

//========================================================================
// function:		ttall_write
// description:	all tt9980 write a byte
// parameter: 	reg, dat
// return: 			result
// version: 		V2.0, 2018-6-18
//========================================================================
uint8_t ttall_write(uint8_t reg, uint8_t dat){
	uint8_t xdata ret = 0, dev = 0;
	for(dev=0; dev<MAX_DEVICE; dev++){
		ret <<= 1;
		ret |= tt_write(dev, reg, dat);
	}
	return ret;
}

//========================================================================
// function:		tt_write
// description:	tt9980 write a byte
// parameter: 	dev reg, dat
// return: 			result
// version: 		V2.0, 2018-6-18
//========================================================================
uint8_t tt_write(uint8_t dev, uint8_t reg, uint8_t dat){
	uint8_t xdata ret = 0, trys = 3;
	uint8_t state = dat;
	do{
		ret = i2c_write(i2c_salve[dev], reg, &state, 1);
	}while(ret && trys--);
	return ret;
}

//========================================================================
// function:		tt_read
// description:	tt9980 read a byte
// parameter: 	dev, reg, dat
// return: 			result
// version: 		V2.0, 2018-6-18
//========================================================================
uint8_t tt_read(uint8_t dev, uint8_t reg, uint8_t *dat){
	uint8_t xdata ret = 0, trys = 3;
	uint8_t state = *dat;
	do{
		ret = i2c_read(i2c_salve[dev], reg, &state, 1);
	}while(ret && trys--);
	return ret;
}
