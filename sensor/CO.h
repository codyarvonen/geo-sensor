// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// CO.h
// This file contains the CO sensor library, including the pin numbers and other necessary variables
// We received most of this code for the CO sensor by an SPEC sensor representative


#ifndef CO_H
#define CO_H

#include "Sensor.h"

class CO {
  public:
    int vgas = 0; //Analog pin, Pin #1 on sensor
    int vtemp = 1; //Analog pin, temperature sensor for CO

    unsigned int ADC0Result_[15];
    unsigned long int ADC0Result; //Vgas   
    unsigned long int ADC0Result_Zero = 21465;  //Adjust this manually
  
    unsigned long int ADC1Result; //Vtemp
    unsigned long int ADC1Result_Zero = 23062;  //Adjust this manually
    
    unsigned long int Rgain = 100E3;
    double PPM;
    float Temperature;
    float Relative_Humidity = 50;
    
    float Arduino_Vref = 5.0;
    float ULP_Vdd = 3.3;
  
    char Serial_Number[13] = {'0', '4', '1', '1', '1', '7', '0', '1', '1', '2', '4', '2', '\0'};  //User QR reader to get this
    float nA_per_PPM = 3.86;  //Printed on back of sensor
    
    double measure();
};

#endif //CO_H
