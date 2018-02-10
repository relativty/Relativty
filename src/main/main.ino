// Copyright (c) 2017 Relativty

//=============================================================================
// i2cdevlib and MPU6050 are needed: https://github.com/jrowberg/i2cdevlib
// Big thanks to the amazing Jeff Rowberg <3, go check his repo to learn more MPU6050.
//=============================================================================

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project

#include "Relativ.h"

#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;
Relativ relativ; 

#define INTERRUPT_PIN 2

// IMU status and control:
bool dmpReady = false;  // true if DMP init was successful
uint8_t mpuIntStatus;   
uint8_t devStatus;      // 0 = success, !0 = error
uint16_t packetSize;    
uint16_t fifoCount;     
uint8_t fifoBuffer[64]; 


Quaternion q;           // [w, x, y, z]

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

void setup() {

    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    /**
    * Relativ::startNative() is for 32-bit ARM microconrollers with Native USB like Arduino DUE (recommended) 
    * Relativ::start() is for NON-NATIVE USB microcontrollers, like Arduino MEGA, Arduino UNO, etc. (slower)
    */
    relativ.startNative(); //Compile error? change this to relativ.start() if you have an UNO, MEGA, etc... 

    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
   
    //If you are getting "SerialUSB was not declared" when compiling, change
    //the following line to Serial.println(.......)
    SerialUSB.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // configure the DMP
    devStatus = mpu.dmpInitialize();

    // ==================================
    // supply your own gyro offsets here:
    // ==================================

    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788);

    // devStatus if everything worked properly
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
    }
}


void loop() {
    // Do nothing if DMP doesn't initialize correctly
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) to be available
    while (!mpuInterrupt && fifoCount < packetSize) {
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        mpu.resetFIFO();
    }

    // check for interrupt
    else if (mpuIntStatus & 0x02) {
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        fifoCount -= packetSize;

        mpu.dmpGetQuaternion(&q, fifoBuffer);


/**
* Relativ::updateOrientationNative() is for 32-bit ARM core microcontrollers
* with Native USB like Arduino DUE (recommended)
* Relativ::updateOrientation() is for NON-NATIVE USB microcontrollers
* like Arduino Mega, Arduino Uno, etc. (slower)
*/

        relativ.updateOrientationNative(q.x, q.y, q.z, q.w, 4); 

        //If your microcontroller is non-native usb, change the above line to:
        //relativ.updateOrientation(q.x, q.y, q.z, q.w, 4); 
    }
}

