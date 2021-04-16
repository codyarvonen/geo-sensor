// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor
// sensor.ino
// This file contains setup() and loop(), as well as functions for the LED and SD card


#include <SD.h>   // Used to read/write files to the SD card
#include <SPI.h>  // Used to communicate with the SD card reader
#include <RTClib.h>   // Library for the real time clock
#include <String.h>   // Required to use string data type
#include "PM.h"   // Header file with PM functions/data
#include "CO2.h"  // Header file with CO2 functions/data
#include "CO.h"   // Header file with CO functions/data

// PIN NUMBERS
#define RED 24  // Pin number for Red in RGB LED
#define GREEN 26  // Pin number for Green in RGB LED
#define BLUE 22   // Pin number for Blue in RGB LED
#define POWER 13   // Pin number for MOSFET (controls power to the fan)
#define CS 53   // Pin number for SD card reader

// OBJECTS
RTC_PCF8523 rtc;  // Real time clock object
PM pm;
CO2 co2;
CO co;

// GLOBAL VARIABLES
File myFile;  // File variable for writing to SD card
String fileName = "datos.csv";

void setup() {
  
  Serial.begin(9600);   // Begin serial communication for printing to serial monitor
  Serial3.begin(9600);  // Serial3 will be used to communicate with the PM sensor
  Serial.println("CONDORS Cookstove Sensor");

  while (!SD.begin()) {   // Initializes SD card, LED will flash until SD card is present
    LED_RED();
    delay(1000);
    LED_OFF();
    delay(1000);
  }
  
  rtc.begin();  // Initializes real time clock, uses RTC library
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));   // Will set the real time clock to the time from the computer system
  printHeader();  // Prints header to the csv file on the SD card

  pm.my_status = pm.stop_autosend();  // Initializes PM sensor, returns status
  co2.scd30.begin();  // Intializes CO2 sensor
  co2.scd30.setMeasurementInterval(1);  //Fastest communication time with CO2 Sensor

  pinMode(POWER, OUTPUT);   // Configures MOSFET pin as OUTPUT
  digitalWrite(POWER, LOW);   // Sets MOSFET pin to LOW
  
  LED_GREEN();
  
}

void loop() {

  Serial.println("MOSFET HIGH");
  digitalWrite(POWER, HIGH);  // Turns on power to the fan
  delay(3000);
  
  pm.measure();   // PM sensor reads measurements, variables in PM library will contain the results
  String pm25 = pm.pm25;  // Concentration of PM that is 2.5micrometers
  String pm10 = pm.pm10;  // Concentration of PM that is 10micrometers
  double ppmCO = co.measure();  // Concentration of CO in parts per million
  double ppmCO2 = co2.measure();  // Concentration of CO2 in parts per million
  
  DateTime now = rtc.now();   // The current time as returned by the real time clock
  writeToFile(now, ppmCO, ppmCO2, pm25, pm10);

  Serial.println("MOSFET LOW");
  digitalWrite(POWER, LOW);   //Turns off power to the fan
  delay(3000);

}

void LED_ON() {
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);
    return;
}

void LED_OFF() {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
    return;
}

void LED_RED() {
    //Turns LED Red
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
    return;
}

void LED_GREEN() {
    //Turns LED Green
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
    return;
}

void LED_BLUE() {
    //Turns LED Blue
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
    return;
}

void printHeader() { //Prints column headers to csv file
    myFile = SD.open(fileName, FILE_WRITE);
    
    if (myFile) { // Write to the file if it opened correctly
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
        return;
    }
    else {
        Serial.println("Failed to connect to SD card");
        LED_RED();
        delay(5000);
        return;
    }
}

void writeToFile(DateTime now, double CO, double CO2, String pm25, String pm10) { //Writes most recent values to the csv file
    myFile = SD.open(fileName, FILE_WRITE);
    
    if (myFile) {  // Write to the file if it opened correctly
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
        return;
    }
    else {
        Serial.println("Failed to connect to SD card");
        LED_RED();
        delay(5000);
        return;
    }

}
