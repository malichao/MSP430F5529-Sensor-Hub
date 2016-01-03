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

#include "filter.h"
#include "math.h"
float averageFilter(float data)
{
	static float temp[3]={0,0,0};
	temp[0]=temp[1];
	temp[1]=temp[2];
	temp[2]=data;
	return (temp[0]+temp[1]+temp[2])/3.0;
}

float medianFilter(float data)
{
	static float temp[5]={0,0,0,0,0};
	temp[0]=temp[1];
	temp[1]=temp[2];
	temp[2]=temp[3];
	temp[3]=temp[4];
	temp[4]=data;

	float temp1[5],t;
	int i,j;
	for(i=0;i<5;i++)
	{
		temp1[i]=temp[i];
	}
	for(i=0;i<5;i++)
		for(j=i+1;j<5;j++)
		{
			if(temp1[i]>temp1[j])
			{
				t=temp1[i];
				temp1[i]=temp1[j];
				temp1[j]=t;
			}
		}
	return temp1[2];
}

//LOW_PASS,Fs=200,HAMMING,wp=3,ws=9
#define FILTERTAPS 5
float coef[FILTERTAPS] = {0.001193,0.010720,0.026164,0.026164,0.010720};
float gain=13.340246;    
float values[FILTERTAPS] = {0}; 

float FIR(float in){
   static char k;
   char i = 0;
   float out = 0;
   values[k] = in; 
   for (i=0; i<FILTERTAPS; i++) {            
     out += coef[i] * values[(i + k) % FILTERTAPS];                          
    }
    out *= gain;                        
    k = (k+1) % FILTERTAPS;                  
    return out;                        
}


float weight=0.6;
#define PI 3.1415926
float AngleCalculate (float angle,float gyroZ,float accX,float accZ)
{
    float time=138;//integral interval time
    float angleFusion;
    float angleA,angleG;
    angleA=-atan2(accX,accZ)*180/PI;
    angleA=FIR(angleA);
    angleG=(gyroZ)*time/1000.0;
    angleFusion=angleA*(1-weight)+(angleG+angle)*weight;
    return angleFusion;
}
