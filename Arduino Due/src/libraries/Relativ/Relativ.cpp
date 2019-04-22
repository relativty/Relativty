/*
  Copyright (c) 2017 Relativty
  Relativ.h - Library for Relativ VR Headset.
  https://github.com/relativty/Relativ
*/

#include "Arduino.h"
#include "Relativ.h"

Relativ::Relativ() {
}

void Relativ::startNative() {
  SerialUSB.begin(1); // BaudRate is irrevelant for Native USB.
}

void Relativ::start() {
  Serial.begin(250000);
}

void Relativ::updateOrientation(float x, float y, float z, float w, int accuracy) {
    Serial.print(x, accuracy);
    Serial.print(",");
    Serial.print(y, accuracy);
    Serial.print(",");
    Serial.print(z, accuracy);
    Serial.print(",");
    Serial.println(w, accuracy);
    Serial.flush();
}

void Relativ::updateOrientationNative(float x, float y, float z, float w, int accuracy) {
    SerialUSB.print(x, accuracy);
    SerialUSB.print(",");
    SerialUSB.print(y, accuracy);
    SerialUSB.print(",");
    SerialUSB.print(z, accuracy);
    SerialUSB.print(",");
    SerialUSB.println(w, accuracy);
    SerialUSB.flush();
}
