///////////////////////////////////////////////////////////////////////////////
//   Malcolm Ma
//   UT Dallas
//   Dec 2015
//   Built with CCSv6.1.1
//
//	 Version: v0.1
//
//  Description:
//  Basic I2C driver for SHT21
//
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
#include "SHT21.h"
//define address
#define SHT21_ADDR 0x40
#define SHT21_TEMP 0xE3
#define SHT21_HUM 0xE5

static unsigned char TxData[] =              // Table of data to transmit
{0};
static unsigned char *RxData; 

void initSHT21()
{

}

unsigned int readSHT21TEMP() 
{
  unsigned int temp=0;
  unsigned char temp1=0;
  setI2CAddress(SHT21_ADDR);
    //read data
  TxData[0]=SHT21_TEMP;
  sendI2C(TxData,1,NO_STOP);

  /*
  readI2CBytes(3,RxData);
  temp1=*RxData++;
  temp=((unsigned int)temp1)<<8;
  temp1=*RxData;
  temp|=((unsigned int)temp1);
  */
  temp=readI2CWord();
  //The resolution of temperature ADC is 14bit
  //The 2 LSB must be set to 0
  temp&=~0x0003;
  return temp;
}

unsigned int readSHT21HUM()
{
  unsigned int temp=0;
  unsigned char temp1=0;
  setI2CAddress(SHT21_ADDR);
    //read data
  TxData[0]=SHT21_HUM;
  sendI2C(TxData,1,NO_STOP);

  /*
    readI2CBytes(3,RxData);
    temp1=*RxData++;
    temp=((unsigned int)temp1)<<8;
    temp1=*RxData;
    temp|=((unsigned int)temp1);
    */
    temp=readI2CWord();
    //The resolution of humidity ADC is 12bit
    //The 2 LSB must be set to 0
    temp&=~0x0003;
    return temp;
}

float getSHT21TEMP()
{
  unsigned int temp=0;
  temp=readSHT21TEMP();
  return (-46.85+175.72/65536.0*temp);
}

float getSHT21HUM()
{
  unsigned int temp=0;
  temp=readSHT21HUM();
  return (-6+125/65536.0*temp);
}
