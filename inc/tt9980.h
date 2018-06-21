#ifndef _TT9980_H_
#define _TT9980_H_

#define TT9980_1_4		(0x27)
#define TT9980_5_8		(0x2B)
#define TT9980_9_12		(0x2D)
#define TT9980_13_16	(0x2E)

/* TT9980 REG address */
#define PWR_STATE		(0x10)	//r  Bit(0~3) == PWR_EN(1~4), Bit(4~7) == PGOOD(1~4)
#define WORK_MODE		(0x12)	//rw Px_M[1:0]  00:Close  01:Manual  10:Half-Auto  11:Auto
#define DET_EN			(0x14)	//rw Bit(0~3) == DET_EN(1~4), Bit(4~7) == CLASS_EN(1~4)
#define PWR_ON			(0x19)	//rw Bit(0~3) == PWR_ON(1~4), Bit(4~7) == PWR_OFF(1~4)
#define I1_L				(0x30)	//r  low byte of current of port 1
#define I1_H				(0x31)	//r  hight byte of current of port 1
#define U1_L				(0x32)	//r  low byte of voltage of port 1
#define U1_H				(0x33)	//r  hight byte of voltage of port 1
#define I2_L				(0x34)	//r  low byte of current of port 2
#define I2_H				(0x35)	//r  hight byte of current of port 2
#define U2_L				(0x36)	//r  low byte of voltage of port 2
#define U2_H				(0x37)	//r  hight byte of voltage of port 2
#define I3_L				(0x38)	//r  low byte of current of port 3
#define I3_H				(0x39)	//r  hight byte of current of port 3
#define U3_L				(0x3A)	//r  low byte of voltage of port 3
#define U3_H				(0x3B)	//r  hight byte of voltage of port 3
#define I4_L				(0x3C)	//r  low byte of current of port 4
#define I4_H				(0x3D)	//r  hight byte of current of port 4
#define U4_L				(0x3E)	//r  low byte of voltage of port 4
#define U4_H				(0x3F)	//r  hight byte of voltage of port 4
#define GPDM(x)			(0x46+x*5)	//rw

#define MODE_AUTO		(0xFF)
#define MODE_HFAUTO	(0xAA)


unsigned char ttall_write(unsigned char reg, unsigned char dat);
unsigned char tt_write(unsigned char dev, unsigned char reg, unsigned char dat);
unsigned char tt_read(unsigned char dev, unsigned char reg, unsigned char *dat);

#endif

