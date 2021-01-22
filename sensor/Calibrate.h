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
