///////////////////////////////////////////////////////////////////////////////
//   Malcolm Ma
//   UT Dallas
//   Dec 2015
//   Built with CCSv6.1.1
//
//	 Version: v0.1
//
//	Description:
//  Basic I2C driver for BMP180,modified from Sparkfun BMP180 library
//	See the original header below.
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

/*
	SFE_BMP180.cpp
	Bosch BMP180 pressure sensor library for the Arduino microcontroller
	Mike Grusin, SparkFun Electronics

	Uses floating-point equations from the Weather Station Data Logger project
	http://wmrx00.sourceforge.net/
	http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf

	Forked from BMP085 library by M.Grusin

	version 1.0 2013/09/20 initial version
	Verison 1.1.2 - Updated for Arduino 1.6.4 5/2015

	Our example code uses the "beerware" license. You can do anything
	you like with this code. No really, anything. If you find it useful,
	buy me a (root) beer someday.
*/

#ifndef BMP180_h
#define BMP180_h

float getBMP180Temp();//if the temperature is stable,don't need to call it
float getBMP180Prs(float temp);//need temperature input

char initBMP180();
  // call pressure.begin() to initialize BMP180 before use
  // returns 1 if success, 0 if failure (bad component or I2C bus shorted?)

char startTemperature(void);
  // command BMP180 to start a temperature measurement
  // returns (number of ms to wait) for success, 0 for fail

char readTemperature(float *T);
  // return temperature measurement from previous startTemperature command
  // places returned value in T variable (deg C)
  // returns 1 for success, 0 for fail

char startPressure(char oversampling);
  // command BMP180 to start a pressure measurement
  // oversampling: 0 - 3 for oversampling value
  // returns (number of ms to wait) for success, 0 for fail

char readPressure(float *P, float *T);
  // return absolute pressure measurement from previous startPressure command
  // note: requires previous temperature measurement in variable T
  // places returned value in P variable (mbar)
  // returns 1 for success, 0 for fail

float getSealevel();
  // convert absolute pressure to sea-level pressure (as used in weather data)
  // P: absolute pressure (mbar)
  // A: current altitude (meters)
  // returns sealevel pressure in mbar

float getAltitude();
  // convert absolute pressure to altitude (given baseline pressure; sea-level, runway, etc.)
  // P: absolute pressure (mbar)
  // P0: fixed baseline pressure (mbar)
  // returns signed altitude in meters

char getError(void);
  // If any library command fails, you can retrieve an extended
  // error code using this command. Errors are from the wire library: 
  // 0 = Success
  // 1 = Data too long to fit in transmit buffer
  // 2 = Received NACK on transmit of address
  // 3 = Received NACK on transmit of data
  // 4 = Other error

int readInt(char address);
  // read an signed int (16 bits) from a BMP180 register
  // address: BMP180 register address
  // value: external signed int for returned value (16 bits)
  // returns 1 for success, 0 for fail, with result in value

unsigned int readUInt(char address);
  // read an unsigned int (16 bits) from a BMP180 register
  // address: BMP180 register address
  // value: external unsigned int for returned value (16 bits)
  // returns 1 for success, 0 for fail, with result in value

char readBytes(unsigned char *values, char length);
  // read a number of bytes from a BMP180 register
  // values: array of char with register address in first location [0]
  // length: number of bytes to read back
  // returns 1 for success, 0 for fail, with read bytes in values[] array
  
char writeBytes(unsigned char *values, char length);
  // write a number of bytes to a BMP180 register (and consecutive subsequent registers)
  // values: array of char with register address in first location [0]
  // length: number of bytes to write
  // returns 1 for success, 0 for fail
  

#endif
