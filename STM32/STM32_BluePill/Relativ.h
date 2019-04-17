/*
  Copyright (c) 2017 Relativty
  Relativ.h - Library for Relativ VR Headset.
  https://github.com/relativty/Relativ
*/

#ifndef Relativ_h
#define Relativ_h

#include "Arduino.h"

class Relativ
{
  public:
    Relativ();
    void start();
    void startNative();
    void updateOrientation(float x, float y, float z, float w, int accuracy);
    void updateOrientationNative(float x, float y, float z, float w, int accuracy);
};

#endif
