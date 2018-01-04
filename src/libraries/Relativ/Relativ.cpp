/*
  Copyright (c) 2017 Relativty
  Relativ.h - Library for Relativ VR Headset.
  https://github.com/relativty/Relativ
*/

#include "Arduino.h"
#include "Relativ.h"

Relativ::Relativ(String controller) {
  _controller = controller;
}

void Relativ::start() {
  if (_controller == "NATIVE") {
    SerialUSB.begin(1); // BaudRate is irrevelant for Native USB.
  }
  else if (_controller == "OTHER"){
    Serial.begin(250000);
  }
}

void Relativ::updateOrientation(float x, float y, float z, float w, int accuracy) {
  if (_controller == "NATIVE") {
    SerialUSB.print(x, accuracy);
    SerialUSB.print(",");
    SerialUSB.print(y, accuracy);
    SerialUSB.print(",");
    SerialUSB.print(z, accuracy);
    SerialUSB.print(",");
    SerialUSB.println(w, accuracy);
  }
  else if (_controller == "OTHER"){
    Serial.print(x, accuracy);
    Serial.print(",");
    Serial.print(y, accuracy);
    Serial.print(",");
    Serial.print(z, accuracy);
    Serial.print(",");
    Serial.println(w, accuracy);
  }
}
