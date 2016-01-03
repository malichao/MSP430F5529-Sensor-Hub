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

#include "BMP180.h"
#include <msp430.h>
#include "I2C.h"
#include <math.h>

#define BMP180_ADDR 0x77 // 7-bit address

#define BMP180_REG_CONTROL 0xF4
#define BMP180_REG_RESULT 0xF6

#define BMP180_COMMAND_TEMPERATURE 0x2E
#define BMP180_COMMAND_PRESSURE0 0x34
#define BMP180_COMMAND_PRESSURE1 0x74
#define BMP180_COMMAND_PRESSURE2 0xB4
#define BMP180_COMMAND_PRESSURE3 0xF4

int AC1,AC2,AC3,VB1,VB2,MB,MC,MD;
unsigned int AC4,AC5,AC6;
float c5,c6,mc,md,x0,x1,x2,y0,y1,y2,p0,p1,p2;
char _error;
#define ALTITUDE 131.0//the altitude of Dallas,meter
static float _absolutePressure;
static float _sealvlPressure;
static float _altitude;

char initBMP180()
// Initialize library for subsequent pressure measurements
{
  float c3,c4,b1;


  // The BMP180 includes factory calibration data stored on the device.
  // Each device has different numbers, these must be retrieved and
  // used in the calculations when taking pressure measurements.

  // Retrieve calibration data from device:

    AC1=readInt(0xAA);
    AC2=readInt(0xAC);
    AC3=readInt(0xAE);
    AC4=readUInt(0xB0);
    AC5=readUInt(0xB2);
    AC6=readUInt(0xB4);
    VB1=readInt(0xB6);
    VB2=readInt(0xB8);
    MB=readInt(0xBA);
    MC=readInt(0xBC);
    MD=readInt(0xBE);
    // All reads completed successfully!

    // If you need to check your math using known numbers,
    // you can uncomment one of these examples.
    // (The correct results are commented in the below functions.)

    // Example from Bosch datasheet
    // AC1 = 408; AC2 = -72; AC3 = -14383; AC4 = 32741; AC5 = 32757; AC6 = 23153;
    // VB1 = 6190; VB2 = 4; MB = -32768; MC = -8711; MD = 2868;

    // Example from http://wmrx00.sourceforge.net/Arduino/BMP180-Calcs.pdf
     //AC1 = 7911; AC2 = -934; AC3 = -14306; AC4 = 31567; AC5 = 25671; AC6 = 18974;
     //VB1 = 5498; VB2 = 46; MB = -32768; MC = -11075; MD = 2432;

    // Compute floating-point polynominals:

    c3 = 160.0 * pow(2,-15) * AC3;
    c4 = pow(10,-3) * pow(2,-15) * AC4;
    b1 = pow(160,2) * pow(2,-30) * VB1;
    c5 = (pow(2,-15) / 160) * AC5;
    c6 = AC6;
    mc = (pow(2,11) / pow(160,2)) * MC;
    md = MD / 160.0;
    x0 = AC1;
    x1 = 160.0 * pow(2,-13) * AC2;
    x2 = pow(160,2) * pow(2,-25) * VB2;
    y0 = c4 * pow(2,15);
    y1 = c4 * c3;
    y2 = c4 * b1;
    p0 = (3791.0 - 8.0) / 1600.0;
    p1 = 1.0 - 7357.0 * pow(2,-20);
    p2 = 3038.0 * 100.0 * pow(2,-36);

    // Success!
    return(1);
}

void bmp180_delay()//4.5ms delay
{
	int i;
	for(i=0;i<5;i++)
		__delay_cycles(1000);//1ms at 1Mhz
}
int readInt(char address)
// Read a signed integer (two bytes) from device
// address: register to start reading (plus subsequent register)
// value: external variable to store data (function modifies value)
{
  unsigned char data[2];
  int value=0;

  data[0] = address;
  if (readBytes(data,2))
  {
    value = (int)(data[0])<<8;
    value |= (int)data[1];
    //if (*value & 0x8000) *value |= 0xFFFF0000; // sign extend if negative
    return(value);
  }
  value = 0;
  return(0);
}


unsigned int readUInt(char address)
// Read an unsigned integer (two bytes) from device
// address: register to start reading (plus subsequent register)
// value: external variable to store data (function modifies value)
{
  unsigned char data[2];
  unsigned int value=0;

  data[0] = address;
  if (readBytes(data,2))
  {
    value = (unsigned int)(data[0])<<8;
    value |= (unsigned int)data[1];
    return(value);
  }
  value = 0;
  return(0);
}


char readBytes(unsigned char *values, char length)
// Read an array of bytes from device
// values: external array to hold data. Put starting register in values[0].
// length: number of bytes to read
{
	setI2CAddress(BMP180_ADDR);
	sendI2C(values,1,NO_STOP);
	readI2CBytes(length,values);
	return(1);
}


char writeBytes(unsigned char *values, char length)
// Write an array of bytes to device
// values: external array of data to write. Put starting register in values[0].
// length: number of bytes to write
{
	setI2CAddress(BMP180_ADDR);
	sendI2CBytes(values,length,STOP);
	return(1);
}


char startTemperature(void)
// Begin a temperature reading.
// Will return delay in ms to wait, or 0 if I2C error
{
	unsigned char data[2], result;
	
	data[0] = BMP180_REG_CONTROL;
	data[1] = BMP180_COMMAND_TEMPERATURE;
	result = writeBytes(data, 2);
	if (result) // good write?
		return(5); // return the delay in ms (rounded up) to wait before retrieving data
	else
		return(0); // or return 0 if there was a problem communicating with the BMP
}


char readTemperature(float *T)
// Retrieve a previously-started temperature reading.
// Requires begin() to be called once prior to retrieve calibration parameters.
// Requires startTemperature() to have been called prior and sufficient time elapsed.
// T: external variable to hold result.
// Returns 1 if successful, 0 if I2C error.
{
	unsigned char data[2];
	char result;
	float tu, a;
	
	data[0] = BMP180_REG_RESULT;

	result = readBytes(data, 2);
	if (result) // good read, calculate temperature
	{
		tu = (data[0] * 256.0) + data[1];

		//example from Bosch datasheet
		//tu = 27898;

		//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf
		//tu = 0x69EC;
		
		a = c5 * (tu - c6);
		*T = a + (mc / (a + md));

		/*		
		Serial.println();
		Serial.print("tu: "); Serial.println(tu);
		Serial.print("a: "); Serial.println(a);
		Serial.print("T: "); Serial.println(*T);
		*/
	}
	return(result);
}

float getBMP180Temp()//if the temperature is stable,don't need to call it
{
	float pointerTemp[1];
	startTemperature();//BMP180,must wait 4.5ms to call getTemp() function
	bmp180_delay();
	readTemperature(pointerTemp);//BMP180,must wait 4.5ms after calling start function
	return pointerTemp[0];
}

float getBMP180Prs(float temp)//need temperature input
{
	float pointerTemp[1];
	float pointerPres[1];
	pointerTemp[0]=temp;
	startPressure(3);//BMP180,must wait 4.5ms to call getPre() function
	bmp180_delay();
	readPressure(pointerPres,pointerTemp);//BMP180,must wait 4.5ms after calling start function
	_absolutePressure=pointerPres[0];
	return pointerPres[0];
}

char startPressure(char oversampling)
// Begin a pressure reading.
// Oversampling: 0 to 3, higher numbers are slower, higher-res outputs.
// Will return delay in ms to wait, or 0 if I2C error.
{
	unsigned char data[2], result, delay;
	
	data[0] = BMP180_REG_CONTROL;

	switch (oversampling)
	{
		case 0:
			data[1] = BMP180_COMMAND_PRESSURE0;
			delay = 5;
		break;
		case 1:
			data[1] = BMP180_COMMAND_PRESSURE1;
			delay = 8;
		break;
		case 2:
			data[1] = BMP180_COMMAND_PRESSURE2;
			delay = 14;
		break;
		case 3:
			data[1] = BMP180_COMMAND_PRESSURE3;
			delay = 26;
		break;
		default:
			data[1] = BMP180_COMMAND_PRESSURE0;
			delay = 5;
		break;
	}
	result = writeBytes(data, 2);
	if (result) // good write?
		return(delay); // return the delay in ms (rounded up) to wait before retrieving data
	else
		return(0); // or return 0 if there was a problem communicating with the BMP
}


char readPressure(float *P, float *T)
// Retrieve a previously started pressure reading, calculate abolute pressure in mbars.
// Requires begin() to be called once prior to retrieve calibration parameters.
// Requires startPressure() to have been called prior and sufficient time elapsed.
// Requires recent temperature reading to accurately calculate pressure.

// P: external variable to hold pressure.
// T: previously-calculated temperature.
// Returns 1 for success, 0 for I2C error.

// Note that calculated pressure value is absolute mbars, to compensate for altitude call sealevel().
{
	unsigned char data[3];
	char result;
	float pu,s,x,y,z;
	
	data[0] = BMP180_REG_RESULT;

	result = readBytes(data, 3);
	if (result) // good read, calculate pressure
	{
		pu = (data[0] * 256.0) + data[1] + (data[2]/256.0);

		//example from Bosch datasheet
		//pu = 23843;

		//example from http://wmrx00.sourceforge.net/Arduino/BMP085-Calcs.pdf, pu = 0x982FC0;	
		//pu = (0x98 * 256.0) + 0x2F + (0xC0/256.0);
		
		s = *T - 25.0;
		x = (x2 * pow(s,2)) + (x1 * s) + x0;
		y = (y2 * pow(s,2)) + (y1 * s) + y0;
		z = (pu - x) / y;
		*P = (p2 * pow(z,2)) + (p1 * z) + p0;

	}
	return(result);
}


float getSealevel()
// Given a pressure P (mb) taken at a specific altitude (meters),
// return the equivalent pressure (mb) at sea level.
// This produces pressure readings that can be used for weather measurements.
{
	_sealvlPressure=(_absolutePressure/pow(1-(ALTITUDE/44330.0),5.255));
	return _sealvlPressure;
}


float getAltitude()
// Given a pressure measurement P (mb) and the pressure at a baseline P0 (mb),
// return altitude (meters) above baseline.
{
	_altitude=(44330.0*(1-pow(_absolutePressure/_sealvlPressure,1/5.255)));
	return _altitude;
}


char getError(void)
	// If any library command fails, you can retrieve an extended
	// error code using this command. Errors are from the wire library: 
	// 0 = Success
	// 1 = Data too long to fit in transmit buffer
	// 2 = Received NACK on transmit of address
	// 3 = Received NACK on transmit of data
	// 4 = Other error
{
	return(_error);
}

