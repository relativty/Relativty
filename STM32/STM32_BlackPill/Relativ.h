/*
  Copyright (c) 2019 Relativty
  Relativ.h - Library for Relativ VR Headset.
  https://github.com/relativty/Relativ

  STM32 Version
  Release: 1.1
  Date:    02/04/2019
*/

#ifndef Relativ_h
#define Relativ_h

#include "Arduino.h"

class Relativ
{
  public:
    Relativ();
    void start();
    void updateOrientation(float x, float y, float z, float w, int accuracy);
};

#endif
