// GEO Cookstove Sensor Code
// Los Encendidos 2021-2022
// Arduino code for the cookstove sensor.
// PM.h
// This file contains pin number, variable, and function declarations for the PM sensor.


#ifndef PM_H
#define PM_H

#include "Sensor.h"

class PM {
  public:
    bool my_status; 

    float f_PM1_0G; // PM1.0 GRIMM mass concentration 
    float f_PM2_5G; // PM1.0 GRIMM mass concentration 
    float f_PM10G;  // PM1.0 GRIMM mass concentration 
    float f_PM1_0T; // PM1.0 TSI mass concentration 
    float f_PM2_5T; // PM1.0 TSI mass concentration 
    float f_PM10T;  // PM1.0 GRIMM mass concentration 
    float f_PN0_3;  // Particles number >0.3um
    float f_PN0_5;  // Particles number >0.5um
    float f_PN1_0;  // Particles number >1.0um
    float f_PN2_5;  // Particles number >2.5um
    float f_PN5_0;  // Particles number >5.0um
    float f_PN10;   // Particles number >10um
    
    String pm2_5;   // String value of f_PN2_5
    String pm10;    // String value of f_PN10

    // Measures the PM values
    void measure();

    // The functions utlilized by measure()
    bool start_measurement(void); // Opens the communication with the sensor
    bool stop_measurement(void);  // Closes the communication with the sensor
    bool read_measurement(void);  // Reads the PM values and stores them in pm2_5 and pm10
    void reset_measurement(void); // Runs stop_measurement() followed by start_measurement()
    
};

#endif //PM_H
