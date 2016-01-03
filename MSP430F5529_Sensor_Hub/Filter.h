///////////////////////////////////////////////////////////////////////////////
//   Malcolm Ma
//   UT Dallas
//   Dec 2015
//   Built with CCSv6.1.1
//
//	 Version: v0.1
//
//  Description:
//	Average Filter:calculate the mean value of pass 3 data,this function
//	contains a static buffer,so it shouldn't be used for different sensors
//
//	Median Filter:returns the median value of pass 5 data,a bubble sort is used
//	because there are only 5 data.This function contains a static buffer,so it
//	shouldn't be used for different sensors
//
//	FIR Filter:A low pass FIR filter.corner freq = 3Hz,cutoff freq = 6Hz
//	Used for Accelerometer.This function contains a static variable,so it
//	shouldn't be used for different sensors
///////////////////////////////////////////////////////////////////////////////

#ifndef FILTER_H
#define FILTER_H

float FIR(float in);
float medianFilter(float data);
float averageFilter(float data);
float AngleCalculate (float angle,float gyroZ,float accX,float accZ);

#endif
