///////////////////////////////////////////////////////////////////////////////
//   Malcolm Ma
//   UT Dallas
//   Dec 2015
//   Built with CCSv6.1.1
//
//	 Version: v0.1
//
//	Description:
//  Basic I2C driver for ISL29023
//  Currently ISL29023 is set to continously ambient light sampling mode,16bit.
//
//
//                                /|\  /|\
//                MSP430F5529     10k  10k      MSP430F5529
//                   slave         |    |         master
//             -----------------   |    |   -----------------
//           -|XIN  P3.0/UCB0SDA|<-|----+->|P3.0/UCB0SDA  XIN|-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |             XOUT|-
//            |     P3.1/UCB0SCL|<-+------>|P3.1/UCB0SCL     |
//            |                 |          |                 |
///////////////////////////////////////////////////////////////////////////////

#include <msp430.h>
#include "I2C.h"
#include "ISL29023.h"
//////////////////////////////////ISL29023//////////////////////////////////
//define address
#define ISL29023_ADDR 0x44
#define CMD1 0x00
#define CMD2 0x01
#define DATA_LSB 0x02
#define DATA_MSB 0x03

//define operation mode for CMD1
#define POWER_DOWN  0x00
#define ALS_ONCE  0x20
#define IR_ONCE   0x40
#define ALS_CONT  0xA0
#define IR_CONT   0xC0

//define operation mode for CMD2
//Lux range
#define RANGE1  0x00//1k
#define RANGE2  0x01//4k
#define RANGE3  0x02//16k
#define RANGE4  0x03//64k
//ADC resolution
#define ADC16   0x00
#define ADC12   0x01
#define ADC8  0x02
#define ADC4  0x03

static unsigned char TxData[] =              // Table of data to transmit
{
  CMD1,
  ALS_CONT,
};
unsigned int light;

void initISL29023()
{
	setI2CAddress(ISL29023_ADDR);

  TxData[0]=CMD1;
  TxData[1]=ALS_CONT;
  sendI2C(TxData,2,STOP);

  TxData[0]=CMD2;
  TxData[1]=RANGE4+ADC16;
  sendI2C(TxData,2,STOP);
}
unsigned int readISL29023()
{
  unsigned int temp;
  setI2CAddress(ISL29023_ADDR);
    //read data
  TxData[0]=DATA_MSB;
  sendI2C(TxData,1,NO_STOP);

  temp=readI2C();
  light=((unsigned int)temp)<<8;

  //read data
  TxData[0]=DATA_LSB;
  sendI2C(TxData,1,NO_STOP);

  temp=readI2C();
  light|=((unsigned int)temp);
  return light;
}
float getISL29023AMB()
{
	unsigned int temp;
	temp=readISL29023();
	return temp/65536.0*64000.0;//range4
}
