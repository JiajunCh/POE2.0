#include "tt9980.h"
#include "i2c.h"
#include "poe_m.h"
#include "STC8xxxx.H"

//========================================================================
// function:		tt_write
// description:	tt9980 write a byte
// parameter: 	reg, dat
// return: 			result
// version: 		V2.0, 2018-6-18
//========================================================================
uint8_t tt_write(uint8_t reg, uint8_t dat){
	uint8_t xdata dev = 0, state = dat, trys = 3, ret = 0;
	for(dev=0; dev<MAX_DEVICE; dev++){	//set 2th class
		trys = 3;
		ret <<= 1;
		do{
			ret |= i2c_write(i2c_salve[dev], reg, &state, 1);
		}while((ret&0x01) && trys--);
	}
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
	do{
		ret = i2c_read(i2c_salve[dev], reg, dat, 1);
	}while(ret && trys--);
	return ret;
}
