#ifndef CO_H
#define CO_H

#include <Arduino.h>

class CO {
  public:
    int concentrationVolt = A3; //Analog pin, Pin #1 on sensor
    int referenceVolt = A2; //Analog pin, Pin #2 on
    int Vtemp = A1; //TODO: NEVER USED
    int power = A0; //Pin number for MOSFET providing power to the CO Sensor
    double voffset = -.008; //TODO: Investigate this value (default from old code)
    double sensitivity = 2.56; //TODO: Investigate this value (Default from old code)
    
    double measure();
};

#endif //CO_H
