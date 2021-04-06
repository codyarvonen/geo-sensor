// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// CO2.cpp
// This file contains function definitions for the CO2 sensor.

#include "CO2.h"

double CO2::measure() {
  double ppm;
  if (scd30.dataReady()){
    if (!scd30.read()) return 0.00;
    ppm = scd30.CO2;
  }
  return ppm;
}
