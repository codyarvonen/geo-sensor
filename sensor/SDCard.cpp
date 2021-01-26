// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for cookstove sensor.
// SD_CARD.cpp
// This file contains function declarations for the SD card.

#include "SDCard.h"

void SDCard::printHeader() { //Prints column headers to csv file
    Serial.println("CO, CO2, PM 2.5, PM 10");
    SD.begin();
    myFile = SD.open(fileName, FILE_WRITE);
    // if the file opened okay, write to it:
    if (myFile) {
        isConnected = true;
        Serial.println("Successfully connected to SD card");
        //Prints the headers to the SD card file
        myFile.print("Fecha");  //Date
        myFile.print(",      ");

        myFile.print("Hora");  //Timestamp
        myFile.print(",      ");

        myFile.print("CO");  //CO concentration, ppm
        myFile.print(",      ");

        myFile.print("CO2");    //CO2 concentration, ppm
        myFile.print(",      ");

        myFile.print("Materia Particular 2.5um");    //PM concentration,
        myFile.print(",      ");

        myFile.print("Materia Particular 10um");    //PM concentration,
        myFile.print(",      ");

        myFile.println("      ");

        myFile.close();
    }
    else {
        Serial.println("Failed to connect to SD card");
        isConnected = false;
    }
}

void SDCard::writeToFile(DateTime now, double CO, double CO2, double pm25, double pm10) { //Writes most recent values to the csv file
    SD.begin();
    myFile = SD.open(fileName, FILE_WRITE);
    // if the file opened okay, write to it:
    if (myFile) {
        isConnected = true;
        Serial.println("Successfully connected to SD card");

        //Write Date Day/Month/Year (Custom in Peru)
        myFile.print(now.day(), DEC);
        myFile.print('/');
        myFile.print(now.month(), DEC);
        myFile.print('/');
        myFile.print(now.year(), DEC);
        myFile.print(",      ");

        //Write Time
        myFile.print(now.hour(), DEC);
        myFile.print(':');
        myFile.print(now.minute(), DEC);
        myFile.print(':');
        myFile.print(now.second(), DEC);
        myFile.print(",      ");

        //write Measured Concentrations
        myFile.print(CO);  //CO concentration, ppm
        myFile.print(",      ");
        myFile.print(CO2);    //CO2 concentration, ppm
        myFile.print(",      ");
        myFile.print(pm25);    //PM 2.5um concentration,
        myFile.print(",      ");
        myFile.print(pm10);    //PM 10um concentration,
        myFile.print(",      ");
        myFile.println("      ");
        myFile.close();
    }
    else {
        Serial.println("Failed to connect to SD card");
        isConnected = false;
    }

}
