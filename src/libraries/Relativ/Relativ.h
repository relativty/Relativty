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
    Relativ(String controller);
    void start();
    void updateOrientation(float x, float y, float z, float w, int accuracy);
  private:
    String _controller;
};

#endif
