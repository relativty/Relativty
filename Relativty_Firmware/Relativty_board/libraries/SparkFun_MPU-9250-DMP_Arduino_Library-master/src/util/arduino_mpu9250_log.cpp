/******************************************************************************
arduino_mpu9250_log.cpp - MPU-9250 Digital Motion Processor Arduino Library 
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

This library implements motion processing functions of Invensense's MPU-9250.
It is based on their Emedded MotionDriver 6.12 library.
	https://www.invensense.com/developers/software-downloads/

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
******************************************************************************/
#include "arduino_mpu9250_log.h"
#include <Arduino.h>
#include <stdarg.h>

// Based on log_stm32.c from Invensense motion_driver_6.12

#define BUF_SIZE        (256)
#define PACKET_LENGTH   (23)

#define PACKET_DEBUG    (1)
#define PACKET_QUAT     (2)
#define PACKET_DATA     (3)

void logString(char * string) 
{
}

int _MLPrintLog (int priority, const char* tag, const char* fmt, ...)
{
}

void eMPL_send_quat(long *quat)
{
}

void eMPL_send_data(unsigned char type, long *data)
{
}