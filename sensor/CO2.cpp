// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// CO2.cpp
// This file contains function definitions for the CO2 sensor.

#include "CO2.h"

double CO2::measure() {
  //I TOOK OUT THE WHILE STATEMENT BECAUSE IT MADE NO SENSE. MAY NEED TO REPLACE
  double ppm = sensor.getCO2();
  ppm = sensor.getCO2();
  return ppm;
}
