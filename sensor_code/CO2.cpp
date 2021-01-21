#include "CO2.h"

double CO2::measure() {
  //I TOOK OUT THE WHILE STATEMENT BECAUSE IT MADE NO SENSE. MAY NEED TO REPLACE
  double ppm = sensor.getCO2();
  ppm = sensor.getCO2();
  return ppm;
}
