///////////////////////////////////////////////////////////////////////////////
//   Malcolm Ma
//   UT Dallas
//   Dec 2015
//   Built with CCSv6.1.1
//
//	 Version: v0.1
//
//  Description: 
//  Basic I2C driver for TI's MSP430F5529
//  ACLK = n/a, MCLK = SMCLK = BRCLK = default DCO = ~1.045MHz
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

#ifndef I2C_H
#define I2C_H

#define STOP  1
#define NO_STOP 0
void initI2C();
void clearI2CPort();
void setI2CAddress(unsigned char addr);
void sendI2C(unsigned char* data,unsigned char length,unsigned char _stop);
void sendI2CBytes(unsigned char* data,unsigned char length,unsigned char _stop);
unsigned char readI2C();
unsigned int readI2CWord();
void readI2CBytes(unsigned char length,unsigned char* pointer);

#endif
