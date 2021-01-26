// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// PM.h
// This file contains pin number, variable, and function declarations for the PM sensor.

#ifndef PM_H
#define PM_H

#include <Arduino.h>

class PM {
  public:

    double measure25();
    double measure10();
};

#endif //PM_H
