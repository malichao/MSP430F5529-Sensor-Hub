///////////////////////////////////////////////////////////////////////////////
//   Malcolm Ma
//   UT Dallas
//   Dec 2015
//   Built with CCSv6.1.1
//
//	 Version: v0.1
//
//  Description:
//  Basic I2C driver for TMP006
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
#include "TEMP006.h"

#define TMP006_I2CADDR 0x41

//registers address
#define TMP006_OBJ 0x00
#define TMP006_AMB 0x01
#define TMP006_CONF 0x02
#define TMP006_MID 0xFE
#define TMP006_DID 0xFF

#define TMP006_POWER_DOWN 0x00
#define TMP006_CONV 0x70

//  CRX   Conversion Rate(Hz)
//  CR0       4
//  CR1       2
//  CR2       1
//  CR3       0.5
//  CR4       0.25
#define TMP006_CR0 0x00
#define TMP006_CR1 0x01
#define TMP006_CR2 0x02
#define TMP006_CR3 0x03
#define TMP006_CR4 0x04

static unsigned char TxData[] =              // Table of data to transmit
{0,0,0};

void initTEMP006()
{
  setI2CAddress(TMP006_I2CADDR);

  TxData[0]=TMP006_CONF;
  TxData[1]=TMP006_CONV+TMP006_CR0;
  TxData[2]=0x00;
  sendI2C(TxData,2,STOP);
}

unsigned int readTMP006AMB() 
{
  unsigned int temp;
  setI2CAddress(TMP006_I2CADDR);
    //read data
  TxData[0]=TMP006_AMB;
  sendI2C(TxData,1,NO_STOP);

  temp=readI2CWord();
  return temp;
}

float getTMP006AMB()
{
	unsigned int temperature=0;
	temperature=readTMP006AMB();
	return (temperature>>2)*0.03125;
}


