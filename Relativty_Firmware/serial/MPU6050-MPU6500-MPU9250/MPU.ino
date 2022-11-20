#include "mpu.hpp"
#include "I2Cdev.hpp"

struct quaternion {
    float w;
    float x;
    float y;
    float z;
} quaternion;

void setup() {
  Fastwire::setup(400, 0);
  mympu_open(200);
  //serial rs232 is slow so i try to get the highest baud rate possible
  //this value need to match the one of the driver
  //8b + even parity + 1 stop bit
  Serial.begin(230400, SERIAL_8E1);
}



void loop() {

  mympu_update();

  quaternion = { mympu.qW, mympu.qX, mympu.qY, mympu.qZ };

  Serial.write((const char *)&quaternion, sizeof(quaternion));
}
