// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for cookstove sensor.
// SDCard.h
// This file contains pin number, variable, and function declarations for the SD card.

#ifndef SDCARD_H
#define SDCARD_H

#include "Sensor.h"

#define CS 53

class SDCard {
  public:
    File myFile; //File for writing to SD card
    String fileName = "datos.csv";
    bool isConnected = false;

    int printHeader();
    int writeToFile(DateTime now, double CO, double CO2, String pm25, String pm10);
};

#endif //SDCARD_H
