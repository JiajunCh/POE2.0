#include "i2c.h"
#include "STC8xxxx.H"
#include "intrins.h"

#define I2C_OVERTIME	(0xff)

#define ERR_OT				0x01

sbit SDA = P3^3;
sbit SCL = P3^2;

static bit isda;				//设备地址标志
static bit isma;				//存储地址标志

static uint8_t xdata I2C_Reg[I2C_REGSIZE] = {0};
static uint8_t I2C_err = 0;

//========================================================================
// function: 		Wait
// description: i2c wait
// parameter: 	void
// return:	 		void
// version: 		V2.0, 2018-4-24
//========================================================================
void Wait(void){
	uint8_t time = I2C_OVERTIME;
	while (!(I2CMSST & 0x40) && time--);
	I2CMSST &= ~0x40;
	if(time) I2C_err |= ERR_OT;
}

//========================================================================
// function: 		Start
// description: i2c general start signal
// parameter: 	void
// return:	 		void
// version: 		V2.0, 2018-4-24
//========================================================================
void Start(void){
//	if(I2C_err) return;
	EAXSFR();
	I2CMSCR = 0x01;     //set master cmd to general start signal
	Wait();
	EAXRAM();
}

//========================================================================
// function: 		SendData
// description: i2c sent _data
// parameter: 	void
// return:	 		void
// version: 		V2.0, 2018-4-24
//========================================================================
void SendData(char dat){
//	if(I2C_err) return;
	EAXSFR();
	I2CTXD = dat;        //write _data to buf
	I2CMSCR = 0x02;      //set master cmd to sent _data
	Wait();
	EAXRAM();
}

//========================================================================
// function: 		RecvACK
// description: i2c receive ack
// parameter: 	void
// return:	 		void
// version: 		V2.0, 2018-4-24
//========================================================================
void RecvACK(void){
//	if(I2C_err) return;
	EAXSFR();
	I2CMSCR = 0x03;      //set master cmd to sent ack
	Wait();
	EAXRAM();
}

//========================================================================
// function: 		RecvData
// description: i2c receive _data
// parameter: 	void
// return:	 		void
// version: 		V2.0, 2018-4-24
//========================================================================
char RecvData(void){
	char rxd = 0;
//	if(I2C_err) return 0;
	EAXSFR();
	I2CMSCR = 0x04;      //set master cmd to receive _data
	Wait();
	rxd = I2CRXD;
	EAXRAM();
	return rxd;
}

//========================================================================
// function: 		SendACK
// description: i2c sent ack
// parameter: 	void
// return:	 		void
// version: 		V2.0, 2018-4-24
//========================================================================
void SendACK(void){
//	if(I2C_err) return;
	EAXSFR();
	I2CMSST = 0x00;       //set ack
	I2CMSCR = 0x05;     	//set master cmd to sent
	Wait();
	EAXRAM();
}

//========================================================================
// function: 		SendNAK
// description: i2c sent nak
// parameter: 	void
// return:	 		void
// version: 		V2.0, 2018-4-24
//========================================================================
void SendNAK(void){
//	if(I2C_err) return;
	EAXSFR();
	I2CMSST = 0x01;       	//set nak
	I2CMSCR = 0x05;         //set master cmd to sent
	Wait();
	EAXRAM();
}

//========================================================================
// function: 		Stop
// description: i2c general start signal
// parameter: 	void
// return:	 		void
// version: 		V2.0, 2018-4-24
//========================================================================
void Stop(void){
	EAXSFR();
	I2CMSCR = 0x06;          //set master cmd to general stop signal
	Wait();
	EAXRAM();
}

//========================================================================
// function: 		i2c_config
// description: i2c config
// parameter: 	void
// return:	 		void
// version: 		V2.0, 2018-4-24
//========================================================================
void i2c_config(void){
	EAXSFR();
	P3PU |= 0x0C;
	P_SW2 |=  0x30;
	I2CCFG = 0xe0;            //enable i2c master
	I2CMSST = 0x00;
	EAXRAM();
}

//========================================================================
// function: 		i2c_write
// description: i2c write a byte to slave
// parameter: 	addr, reg, pbuf, len
// return:	 		ret flag
// version: 		V2.0, 2018-5-24
//========================================================================
uint8_t i2c_write(uint8_t addr, uint8_t reg, uint8_t* pbuf, uint8_t len){
	I2C_err = 0;
	Start();           //start signal
	SendData(addr<<1); //dev_addr<<1 + w
	RecvACK();
	SendData(reg);     //mem_addr
	RecvACK();
	for(; len>0; len--){
		SendData(*pbuf++);     //byte_data
		RecvACK();
	}
	Stop();            //stop signal
	return I2C_err;
}

//========================================================================
// function: 		i2c_read
// description: i2c read a byte from slave
// parameter: 	addr, reg, pbuf, len
// return:	 		ret flag
// version: 		V2.0, 2018-5-24
//========================================================================
uint8_t i2c_read(uint8_t addr, uint8_t reg, uint8_t* pbuf, uint8_t len){
	I2C_err = 0;
	Start();                 	//start signal
	SendData((addr<<1)+1);   	//dev_addr<<1 + r
	RecvACK();
	SendData(reg);            //mem_addr
	RecvACK();
	for(; len>1; len--){
		*pbuf++ = RecvData();   //read _data
		SendACK();
	}
	*pbuf = RecvData();    		//read _data
	SendNAK();
	Stop();                   //stop signal
	return I2C_err;
}

void test(void){
	Start();                                    //发送起始命令
	SendData(0x5a);                             //发送设备地址+写命令
	RecvACK();
	SendData(0x00);                             //发送存储地址
	RecvACK();
	SendData(0x12);                             //写测试数据1
	RecvACK();
	SendData(0x78);                             //写测试数据2
	RecvACK();
	Stop();                                     //发送停止命令

	Start();                                    //发送起始命令
	SendData(0x5a);                             //发送设备地址+写命令
	RecvACK();
	SendData(0x00);                             //发送存储地址高字节
	RecvACK();
	Start();                                    //发送起始命令
	SendData(0x5b);                             //发送设备地址+读命令
	RecvACK();
	P0 = RecvData();                            //读取数据1
	SendACK();
	P2 = RecvData();                            //读取数据2
	SendNAK();
	Stop();                                     //发送停止命令
	while (1);
}


