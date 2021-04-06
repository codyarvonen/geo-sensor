// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for cookstove sensor. The code is broken up into this sensor.ino file and several different classes, for the CO, CO2, and PM sensors, and for the calibration process and the SD card.
// sensor.ino
// This file contains pin numbers for the LEDs, the setup and loop functions, the sensorsOn and sensorsOff functions, and the custom average function.

#include <Wire.h>
#include <math.h> //Supports math for floating point numbers
#include "SoftwareSerial.h" //For serial communication
#include "CO.h"
#include "CO2.h"
#include "PM.h"
#include "SDCard.h"

//PIN NUMBERS
#define RED 24 //Pin number for Red in RGB LED
#define GREEN 26 //Pin number for Green in RGB LED
#define BLUE 22 //Pin number for Blue in RGB LED
#define power 13 //Pin number for MOSFET

//OBJECTS
CO co;
CO2 co2;
PM pm;
SDCard sd;
RTC_PCF8523 rtc;

//SETUP VALUES

void setup() {
    Serial.begin(9600); //Begin serial communication for printing to serial monitor
    Serial.println("CONDORS Cookstove Sensor");
    Serial3.begin(9600); //Serial communication port for PM sensor
    Wire.begin();
    Serial.println("LED on");
    //LED_ON();
    LED_BLUE();

    pinMode(power, OUTPUT); //declares MOSFET pin as output
    LED_GREEN();
    
    digitalWrite(power, HIGH);
    co2.scd30.begin();
    LED_BLUE();
    co2.scd30.setMeasurementInterval(1); //Fastest communication time with CO2 Sensor
    LED_RED();
    pm.my_status = pm.stop_autosend();
    LED_ON();
    rtc.begin();
    LED_GREEN();

    if (sd.printHeader() == 0) { //Check if SD card is connected and print header, if not LED will flash red three times
      for (int i = 0; i < 3; i++) {
        LED_OFF();
        LED_RED();
        delay(1000);
        LED_OFF();
        delay(1000);
      }
    }
    
    LED_OFF();
    digitalWrite(power, LOW);
}

void loop() {
    Serial.println("Measuring");
    double measurementInterval = 60.0; //will collect data every 60 seconds
    //int numTrials = 5; //Number of samples that will be taken for each measurement, TODO: Determine if we want to average multiple readings

    sensorsOn();
    DateTime now = rtc.now();
    DateTime end_loop (now + TimeSpan(0,0,0, measurementInterval));

    LED_GREEN();
    delay(2000);

    /*double coSum[numTrials]; //Arrays used for the sum of the samples in order to use custom average function that ignores values of -1
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
    double ppmPM10 = average(pm10Sum, numTrials);*/

    double ppmCO = co.measure();
    double ppmCO2 = co2.measure();
    String pm25 = pm.pm25;
    String pm10 = pm.pm10;

    Serial.print("CO = "); //TODO: These statements are for testing and can be removed
    Serial.println(ppmCO);
    Serial.print("CO2 = ");
    Serial.println(ppmCO2);
    Serial.print("PM25 = ");
    Serial.println(pm25);
    Serial.print("PM10 = ");
    Serial.println(pm10);

    if (!sd.isConnected) { //TODO: Repeated code from setup(); move to its own function?
        for (int i = 0; i < 3; i++) {
          LED_RED();
          delay(1000);
          LED_OFF();
          delay(1000);
      }
    } else {
      sd.writeToFile(now, ppmCO, ppmCO2, pm25, pm10);
    }
    
    sensorsOff();
    
    //TODO: Does 60 second delay includes measurement time or no?
}

void sensorsOn() {
    digitalWrite(power, HIGH);
}

void sensorsOff() {
    digitalWrite(power, LOW);
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
