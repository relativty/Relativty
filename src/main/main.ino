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
//Relativ relativ("NATIVE"); // "NATIVE" For arduino Due and other 32-bit ARM core with native USB

//Relativ relativ("OTHER"); // "OTHER" For arduino UNO, mega, ... (Significatively slower than NATIVE)


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

    //Relativ Relativ("NATIVE");
    // Relativ Relativ("OTHER");

    SerialUSB.begin(250000);
    while (!SerialUSB); // wait for Leonardo enumeration, others continue immediately

    int i = 0;
    while (i < 1000) {
      SerialUSB.println("13092001MCPC");
      SerialUSB.flush();
      i++;
    }
    
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    // configure the DMP
    devStatus = mpu.dmpInitialize();

    // ==================================
    // supply your own gyro offsets here:
    // ==================================
    
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788);

    // devSTatus if everything worked properly
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
    // Do nothing if DMP isn't initialize correctly
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
      //...
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for interrupt
    if (mpuIntStatus & 0x02) {
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      fifoCount -= packetSize;
        
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      SerialUSB.print(q.x, 4);
      SerialUSB.print(",");
      SerialUSB.print(q.y, 4);
      SerialUSB.print(",");
      SerialUSB.print(q.z, 4);
      SerialUSB.print(",");
      SerialUSB.println(q.w, 4);
      //SerialUSB.println(String(q.x) +"," +String(q.y) +"," +String(q.z)+","+String(q.w));
    
    }
}

