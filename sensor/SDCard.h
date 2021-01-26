// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for cookstove sensor.
// SDCard.h
// This file contains pin number, variable, and function declarations for the SD card.

#ifndef SD_H
#define SD_H

#include <SD.h>
#include <RTClib.h>

class SDCard {
  public:
    int power = 48; //Pin number for MOSFET providing power to the  SD card reader
    File myFile; //File for writing to SD card
    String fileName = "datos.csv";
    bool isConnected = false;

    void printHeader();
    void writeToFile(DateTime now, double CO, double CO2, double pm25, double pm10);
};

#endif //PM_H
