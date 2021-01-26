// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for cookstove sensor. The code is broken up into this sensor.ino file and several different classes, for the CO, CO2, and PM sensors, and for the calibration process and the SD card.
// sensor.ino
// This file contains pin numbers for the LEDs, the setup and loop functions, the sensorsOn and sensorsOff functions, and the custom average function.

//TODO: Add headers/descriptions to each file

#include <SPI.h>
//#include <SD.h> //For reading and writing to SD card
#include <Wire.h>
//#include <SparkFun_SCD30_Arduino_Library.h> //Library for the CO2 sensor
//#include <EEPROM.h> //For writing to non-volatile memory
#include <String.h> //Supports use of strings
#include <math.h> //Supports math for floating point numbers
#include <SoftwareSerial.h> //For serial communication
#include <RTClib.h> //For the real time clock
//#include "LinearRegression.h" //Used for calibration
#include <Arduino.h>
#include "CO.h"
#include "CO2.h"
#include "PM.h"
#include "SDCard.h"
#include "Calibrate.h"

//PIN NUMBERS
int RED = 45; //Pin number for Red in RGB LED
int GREEN = 46; //Pin number for Green in RGB LED
int BLUE = 44; //Pin number for Blue in RGB LED
int MODE_SWITCH = 6; //HIGH indicates calibration mode, LOW indicates measurement mode
int sensorPower = D5; //Pin number for MOSFET providing power to the sensors	
int fanPower = D6; //Pin number for MOSFET providing power to the fan

//OBJECTS
CO co;
CO2 co2;
PM pm;
SDCard sd;
Calibrate calibrate;

//SETUP VALUES

void setup() {
    Serial.begin(9600); //Begin serial communication for printing to serial monitor
    Wire.begin();
    LED_ON();

    co2.sensor.begin();
    co2.sensor.setMeasurementInterval(1); //Fastest communication time with CO2 Sensor
    pinMode(sensorPower, OUTPUT); //turns on power for MOSFET of all 3 sensors	
    pinMode(fanPower, OUTPUT);	
    pinMode(53, OUTPUT); //this is the wake-up call for the SS to get working	
    pinMode(MODE_SWITCH, INPUT);

    if (!sd.isConnected) { //Check if SD card is connected, if not LED will flash red three times
      for (int i = 0; i < 3; i++) {
        LED_RED();
        delay(1000);
        LED_OFF();
        delay(1000);
      }
    }
    else {
      sd.printHeader();
    }

    Serial.println(digitalRead(MODE_SWITCH)); //TODO: remove, only to test current mode value
    
    //TODO: Currently the switch is broken, always reads 0, must change values in code
    if (digitalRead(MODE_SWITCH) == 0) { //If in calibration mode
        Serial.println("Calibrating");
        LED_BLUE();
        calibrate.calibrate();
        while ( digitalRead(MODE_SWITCH) == 0) { //Wait until switched back to measurement mode
            delay(500);
        }
    }
}

void loop() {
    Serial.println("Measuring");
    double measurementInterval = 60.0; //will collect data every 60 seconds
    int numTrials = 5; //Number of samples that will be taken for each measurement
    
    RTC_PCF8523 rtc; // Real time clock. The time is already set to PERUVIAN TIME
    if (!rtc.begin()) { //Tries to connect with the clock
        Serial.println("Couldn't find RTC"); //TODO: Add a signal with the LED?
    }
    DateTime now = rtc.now();
    DateTime end_loop (now + TimeSpan(0,0,0, measurementInterval)); //TODO: Investigate further, what is this supposed to do?

    LED_GREEN();
    sensorsOn();
    delay(2000); //TODO: Why this delay??

    double coSum[numTrials]; //Arrays used for the sum of the samples in order to use custom average function that ignores values of -1
    double co2Sum[numTrials];
    double pm25Sum[numTrials];
    double pm10Sum[numTrials];

    for (int i = 0; i < numTrials; i++) {
        coSum[i] = co.measure();
        co2Sum[i] = co2.measure();
        pm25Sum[i] = pm.measure25();
        pm10Sum[i] = pm.measure10();
        delay(100);
    }
    double ppmCO = average(coSum, numTrials);
    double ppmCO2 = average(co2Sum, numTrials);
    double ppmPM25 = average(pm25Sum, numTrials);
    double ppmPM10 = average(pm10Sum, numTrials);

    Serial.print("CO = "); //TODO: These statements are for testing and can be removed
    Serial.println(ppmCO);
    Serial.print("CO2 = ");
    Serial.println(ppmCO2);
    Serial.print("PM25 = ");
    Serial.println(ppmPM25);
    Serial.print("PM10 = ");
    Serial.println(ppmPM10);

    if (!sd.isConnected) { //TODO: Repeated code from setup(); move to its own function?
        for (int i = 0; i < 3; i++) {
          LED_RED();
          delay(1000);
          LED_OFF();
          delay(1000);
      }
    } else {
      sd.writeToFile(now, ppmCO, ppmCO2, ppmPM25, ppmPM10);
    }
    
    sensorsOff();
    
    //TODO: I didn't include a while loop and a delay statement from the original code here; pending investigation
}

void sensorsOn() {
    //Most of the sensors need to be written LOW to
    //TODO: Change to HIGH with new MOSFAT??
    digitalWrite(sensorPower, HIGH);	
    digitalWrite(fanPower, HIGH);
}

void sensorsOff() {
    digitalWrite(sensorPower, LOW);	
    digitalWrite(fanPower, LOW);
}


void LED_ON() {
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);
}

void LED_OFF() {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
}

void LED_RED() {
    //Turns LED Red
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
}

void LED_GREEN() {
    //Turns LED Green
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
}

void LED_BLUE() {
    //Turns LED Blue
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
}

double average(double values[], int len) {
    //Averages values from an array and ignores values of -1
    double sum = 0.0;
    int count = 0;
    for (int i = 0; i < len; i++) {
        if (values[i] != -1 ) {
            sum += values[i];
            count++;
        }
    }
    if (count == 0) {
        return -1;
    }
    double avg = sum / double(count);
    return avg;
}
