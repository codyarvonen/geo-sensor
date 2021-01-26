// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// CO.cpp
// This file contains function definitions for the CO sensor.

#include "CO.h"

double CO::measure() { //Returns concentration of CO in ppm
  double concVolt = double(analogRead(concentrationVolt)) * double(5.0 / 1023.0); //Analog voltage indicating concentration
  double refVolt = double(analogRead(referenceVolt)) * double(5.0 / 1023.0); //Analog reference voltage
  double vgas = refVolt + voffset;
  double tiaGain = 100;
  double mwa = sensitivity * tiaGain * pow(10, -6);
  double ppm = (concVolt - vgas) / mwa;
  return ppm;
}
