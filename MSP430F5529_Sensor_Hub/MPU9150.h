///////////////////////////////////////////////////////////////////////////////
//   Malcolm Ma
//   UT Dallas
//   Dec 2015
//   Built with CCSv6.1.1
//
//	 Version: v0.1
//
//  Description:
//  Basic I2C driver for MPU9150
//	MPU9150 is basically a MPU6050 integrated with a magnetometer,that's why
//	so many MPU6050 namings are used here.However,namings can be done in a
//	better way.This is due to the tight schedule.
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

#ifndef MPU9150_H
#define MPU9150_H

void initMPU9150(void);
void readMPU6050ID(void);
float readMPU6050Temp( );
void readMPU6050Gyro(int *gyroData);
void readMPU6050Acc(int *accData);
void readMPU6050Mgn(int* cmpsData);

void getMPU6050Acc(float *ax,float *ay,float *az);
void getMPU6050Gyro(float *gx,float *gy,float *gz);
void getMPU6050Mgn(float *mx,float *my,float *mz);

//private funcions
void MPU6050_WriteReg(unsigned char addr,unsigned char data);
void MPU9150_WriteReg(unsigned char addr,unsigned char data);
void MPU6050_ReadData(unsigned char reg_add,unsigned char*Read,unsigned char num);
void MPU9150_ReadData(unsigned char reg_add,unsigned char*Read,unsigned char num);
void MPU6050_ReturnTemp(int *Temperature);
#endif
