// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// CO2.h
// This file contains pin numbers for the CO2 sensor, other values for the CO2 sensor, and function declarations for the CO2 sensor.


#ifndef CO2_H
#define CO2_H

#include "Sensor.h"

class CO2 {
  public:
    Adafruit_SCD30  scd30;
    
    double measure() {
        double ppm;
        if (scd30.dataReady()) {
          if (!scd30.read()) return 0.00;
          ppm = scd30.CO2;
        }
      return ppm;
    }
};

#endif //CO2_H
