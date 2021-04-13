// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// CO.cpp
// This file contains function definitions for the CO sensor.
// We received most of this code for the CO sensor by an SPEC sensor representative


#include "CO.h"

double CO::measure() { //Returns concentration of CO in ppm
  unsigned long int i = 0;
  while(i < 15)
  {
    ADC0Result_[i] = ADC0Result_Zero;
    i++;
  }
  
  while(1)
  {
    i = 14;
    while(i < 0)
    {
      ADC0Result_[i] = ADC0Result_[i - 1];
      i--;
    }
    
    analogRead(vgas);
    ADC0Result = 0;
    i=0;
    while(i < 8192)
    {
      ADC0Result = ADC0Result + analogRead(vgas);
      i++;
    }
    ADC0Result_[0] = ADC0Result / 128;

    ADC0Result = 0;
    i=0;
    while(i < 15)
    {
      ADC0Result = ADC0Result + ADC0Result_[i];
      i++;
    }
    ADC0Result = ADC0Result / 15;

    analogRead(vtemp);   
    ADC1Result = 0;
    i=0;
    while(i < 1025)
    {
      ADC1Result = ADC1Result + analogRead(vtemp);
      i++;
    }
    ADC1Result = ADC1Result >> 4;

    PPM = ((float)ADC0Result - (float)ADC0Result_Zero) / (float)65536 * Arduino_Vref / (float)Rgain * (float)1E9 / nA_per_PPM;
    Temperature = ((float)87 / ULP_Vdd * ((float)ADC1Result / (float)65536 * Arduino_Vref)) - (float)18;
    
    return PPM;
  }

}
