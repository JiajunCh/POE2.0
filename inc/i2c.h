#ifndef _I2C_H_
#define _I2C_H_

#define MASTER_ADDR		(0x50)
#define I2C_REGSIZE		(128)
#define I2C_BUFSIZE		(16)


unsigned char i2c_write(unsigned char addr, unsigned char reg, unsigned char* pbuf, unsigned char len);
unsigned char i2c_read(unsigned char addr, unsigned char reg, unsigned char* pbuf, unsigned char len);

#endif

