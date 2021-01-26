// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// Calibrate.h
// This file contains variable and function declarations for the CO and CO2 calibration process.

#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "CO.h"
#include "CO2.h"
#include "LinearRegression.h"
#include <EEPROM.h>

class Calibrate {
  public:
    void calibrate();
    float findFlow(float(x));
    float calcConc(float flows[3], float(ppm));
    float lineRegressSlope(float concs[3], float voltages[3]);
    float lineRegressInter(float concs[3], float voltages[3]);
    float readFloat(unsigned int addr);
    void writeFloat(unsigned int addr, float x);
};

#endif //CALIBRATE_H
