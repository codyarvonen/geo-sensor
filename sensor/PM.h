// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// PM.h
// This file contains pin number, variable, and function declarations for the PM sensor.


#ifndef PM_H
#define PM_H

#include "Sensor.h"

class PM {
  public:
    bool my_status;
    float f_PM01;
    float f_PM25;
    float f_PM4;
    float f_PM10;
    String pm01;
    String pm25;
    String pm4;
    String pm10;

    bool start_measurement(void);
    bool stop_measurement(void);
    bool read_measurement (void);
    bool stop_autosend(void);
    bool start_autosend(void);
    void measure();
};

#endif //PM_H
