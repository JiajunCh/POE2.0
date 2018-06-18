#include "tt9980.h"
#include "i2c.h"
#include "poe_m.h"
#include "STC8xxxx.H"

uint8_t tt_write(uint8_t reg, uint8_t dat){
	uint8_t xdata dev = 0, state = dat, trys = 3, ret = 0;
	for(dev=0; dev<MAX_DEVICE; dev++){	//set 2th class
		trys = 3;
		do{
			ret = i2c_write(i2c_salve[dev], reg, &dat, 1);
		}while(ret && trys--);
	}
	return ret;
}

uint8_t tt_read(uint8_t dev, uint8_t reg, uint8_t *dat){
	uint8_t xdata ret = 0, trys = 3;
	do{
		ret = i2c_read(i2c_salve[dev], reg, dat, 1);
	}while(ret && trys--);
	return ret;
}
