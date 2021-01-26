// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// CO2.h
// This file contains pin numbers for the CO2 sensor, other values for the CO2 sensor, and function declarations for the CO2 sensor.

#ifndef CO2_H
#define CO2_H

#include <SparkFun_SCD30_Arduino_Library.h>

class CO2 {
  public:
    SCD30 sensor;
    int power = 13; //Pin number for MOSFET providing power to the CO2 Sensor
    
    //Addresses for each permanent variable to be stored for calibration
    //Each calibration parameter will be stored as a double at the given address
    int address_m = 16; //For CO2 sensor (TODO: NOT USED ANYWHERE ELSE)
    int address_b = 24; //For CO2 sensor (TODO: NOT USED ANYWHERE ELSE)

    
    //Default value for each parameter
    //To be used when the system is reset or if the calibrated value cannot be read
    double default_m = 11.5; //For CO2 sensor (TODO: NOT USED ANYWHERE ELSE)
    double default_b = -4000; //For CO2 sensor (TODO: NOT USED ANYWHERE ELSE)

    
    //Initial value for each parameter
    double m = default_m; //For CO2 sensor (TODO: NOT USED ANYWHERE ELSE)
    double b = default_b; //For CO2 sensor (TODO: NOT USED ANYWHERE ELSE)

    double measure();
};

#endif //CO_H
