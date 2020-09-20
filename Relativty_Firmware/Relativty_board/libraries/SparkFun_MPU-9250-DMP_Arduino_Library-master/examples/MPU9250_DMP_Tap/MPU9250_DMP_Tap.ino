/************************************************************
MPU9250_DMP_Tap
 Tap-detection example for MPU-9250 DMP Arduino Library 
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

The MPU-9250's digital motion processor (DMP) can monitor for
single or double tap events on any of the three accelerometer
axes.

This example turns tap-detection on in the z-axis. Try to
tap it to the max count of 8!

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
*************************************************************/
#include <SparkFunMPU9250-DMP.h>

#define SerialPort SerialUSB

MPU9250_DMP imu;

void setup() 
{
  SerialPort.begin(115200);

  // Call imu.begin() to verify communication and initialize
  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
      SerialPort.println("Unable to communicate with MPU-9250");
      SerialPort.println("Check connections, and try again.");
      SerialPort.println();
      delay(5000);
    }
  }

  // Enable tap detection in the DMP. Set FIFO sample rate to 10Hz.
  imu.dmpBegin(DMP_FEATURE_TAP, 10);
  // dmpSetTap parameters, in order, are:
  // x threshold: 1-1600 (0 to disable)
  // y threshold: 1-1600 (0 to disable)
  // z threshold: 1-1600 (0 to disable)
  // (Threshold units are mg/ms)
  // taps: Minimum number of taps needed for interrupt (1-4)
  // tap time: milliseconds between valid taps
  // tap time multi: max milliseconds between multi-taps
  unsigned short xThresh = 0;   // Disable x-axis tap
  unsigned short yThresh = 0;   // Disable y-axis tap
  unsigned short zThresh = 100; // Set z-axis tap thresh to 100 mg/ms
  unsigned char taps = 1;       // Set minimum taps to 1
  unsigned short tapTime = 100; // Set tap time to 100ms
  unsigned short tapMulti = 1000;// Set multi-tap time to 1s
  imu.dmpSetTap(xThresh, yThresh, zThresh, taps, tapTime, tapMulti);
}

void loop() 
{
  // Check for new data in the FIFO
  if ( imu.fifoAvailable() )
  {
    // DMP FIFO must be updated in order to update tap data
    imu.dmpUpdateFifo();
    // Check for new tap data by polling tapAvailable
    if ( imu.tapAvailable() )
    {
      // If a new tap happened, get the direction and count
      // by reading getTapDir and getTapCount
      unsigned char tapDir = imu.getTapDir();
      unsigned char tapCnt = imu.getTapCount();
      switch (tapDir)
      {
      case TAP_X_UP:
          SerialPort.print("Tap X+ ");
          break;
      case TAP_X_DOWN:
          SerialPort.print("Tap X- ");
          break;
      case TAP_Y_UP:
          SerialPort.print("Tap Y+ ");
          break;
      case TAP_Y_DOWN:
          SerialPort.print("Tap Y- ");
          break;
      case TAP_Z_UP:
          SerialPort.print("Tap Z+ ");
          break;
      case TAP_Z_DOWN:
          SerialPort.print("Tap Z- ");
          break;
      }
      SerialPort.println(tapCnt);
    }
  }
}

