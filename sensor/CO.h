// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// CO.h
// This file contains pin numbers for the CO sensor, other values for the CO sensor, and any function declarations for the CO sensor.

#ifndef CO_H
#define CO_H

#include <Arduino.h>

class CO {
  public:
    int concentrationVolt = A0; //Analog pin, Pin #1 on sensor
    int referenceVolt = A1; //Analog pin, Pin #2 on
    int Vtemp = A2; //Analog pin, temperature sensor for CO
    double voffset = -.008; //TODO: Investigate this value (default from old code)
    double sensitivity = 2.56; //TODO: Investigate this value (Default from old code)
    
    double measure();
};

#endif //CO_H
