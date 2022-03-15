// GEO Cookstove Sensor Code
// Los Encendidos 2021-2022
// Arduino code for the cookstove sensor
// sensor.ino
// This file contains setup() and loop(), as well as functions for the LED and SD card

#include <SD.h>       // Used to read/write files to the SD card
#include <SPI.h>      // Used to communicate with the SD card reader
#include <RTClib.h>   // Library for the real time clock
#include <String.h>   // Required to use string data type
#include "PM.h"       // Header file with PM functions/data
#include "CO2.h"      // Header file with CO2 functions/data
#include "CO.h"       // Header file with CO functions/data

#include <LowPower.h> // Library to enable sleep mode

// PIN NUMBERS
#define RED 24    // Pin number for Red in RGB LED
#define GREEN 26  // Pin number for Green in RGB LED
#define BLUE 22   // Pin number for Blue in RGB LED
#define FAN 13    // Pin number for MOSFET (controls power to the fan)
#define CS 53     // Pin number for SD card reader

#define INTERRUPT_PIN 18

// OBJECTS
RTC_PCF8523 rtc;  // Real time clock object
PM pm;
CO2 co2;
CO co;

// GLOBAL VARIABLES
File myFile;      // File variable for writing to SD card
String fileName = "datos.csv";

String pm2_5;
String pm10;
double ppmCO = 0;
double ppmCO2 = 0;

enum state {sleep, fanOn, fanOff, measure, writeData, errorSD, errorMeasure};
enum state currentState;

void setup() {

  pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Initialize the interrupt pin with the internal pull up resistor in the Arduino
//  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(FAN, OUTPUT);       // Configures MOSFET pin as OUTPUT
  pinMode(RED, OUTPUT);       // Configures RED pin as OUTPUT
  pinMode(BLUE, OUTPUT);      // Configures BLUE pin as OUTPUT
  pinMode(GREEN, OUTPUT);     // Configures GREEN pin as OUTPUT
  digitalWrite(FAN, LOW);     // Sets MOSFET pin to LOW
  LED_OFF();                  // Sets color pins to LOW
  delay(500);
  
  LED_ON();
  delay(2000);
  
  Serial.begin(9600);   // Begin serial communication for printing to serial monitor
  Serial3.begin(9600);  // Serial3 will be used to communicate with the PM sensor
  Serial.println("Los Encendidos Cookstove Sensor");

  initSensors(true, true, true, true);

  // https://adafruit.github.io/RTClib/html/class_r_t_c___p_c_f8523.html
  // https://brainwagon.org/2019/04/03/template-program-that-uses-a-ds3231-rtc-to-wake-up-an-arduino/

  currentState = fanOff;

  SD.begin();

  LED_OFF();
  delay(2000);
}

void loop() {

  static int fanTimer = 0;
  static bool writeSuccess = false;
  static bool SDCardSuccess = false;

  // Perform state update first.
  switch(currentState) { 
    case sleep:
      digitalWrite(FAN, HIGH);
      currentState = fanOn;
      break;
    case fanOn:
      if (fanTimer == 5) {
        fanTimer = 0;
        digitalWrite(FAN, LOW);
        currentState = fanOff;
      }
      else {
        currentState = fanOn;
      }
      break;
    case fanOff:
      if (fanTimer == 5) {
        fanTimer = 0;
        currentState = measure;
      }
      else {
        currentState = fanOff;
      }
      break;
    case measure:
      currentState = writeData;
      break;
    case writeData:
      if (writeSuccess) {
        currentState = sleep;
      }
      else {
        currentState = errorSD;
      }
      break;
    case errorSD:
      if (SDCardSuccess && printHeader()) {
        digitalWrite(FAN, HIGH);
        currentState = fanOn;
      }
      else {
        currentState = errorSD;
      }
      break;
    case errorMeasure:
      // IMPLEMENT THIS STATE (CHECKSUM FAIL, ETC.)
      break;
    default:
      Serial.println("ERROR: No state has been specified");
      break;
  }

  // Perform state action next.
  switch(currentState) { 
    case sleep:
      Serial.println("STATE: sleep");
      LED_OFF();

      Serial.println("MCU going to sleep...");

      rtc.enableCountdownTimer(PCF8523_FrequencyMinute, 1);

      delay(2000);
      attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), isr, FALLING);
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
      
      detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN));
      rtc.disableCountdownTimer();
      delay(2000);
      
      break;
    case fanOn:
      Serial.println("STATE: fanOn");
      LED_GREEN();
      fanTimer++;
      break;
    case fanOff:
      Serial.println("STATE: fanOff");
      LED_OFF();
      fanTimer++;
      break;
    case measure:
      Serial.println("STATE: measure");
      LED_BLUE();
      pm.measure();             // PM sensor reads measurements, variables in PM library will contain the results
      pm2_5 = pm.pm2_5;         // Concentration of PM that is 2.5micrometers
      pm10 = pm.pm10;           // Concentration of PM that is 10micrometers
      ppmCO = co.measure();     // Concentration of CO in parts per million
      ppmCO2 = co2.measure();   // Concentration of CO2 in parts per million
      break;
    case writeData:
      Serial.println("STATE: writeData");
      writeSuccess = writeToFile(rtc.now(), ppmCO, ppmCO2, pm2_5, pm10);
      break;
    case errorSD:
      Serial.println("STATE: errorSD");
      LED_RED();
      SDCardSuccess = SD.begin();
      break;
    case errorMeasure:
      // IMPLEMENT THIS STATE (CHECKSUM FAIL, ETC.)
      Serial.println("STATE: errorMeasure");
      break;
    default:
      Serial.println("ERROR: No state has been specified");
      LED_OFF();
      break;
  }
  
  delay(1000);
  
}

/*
 * Interrupt service routine
 */
 
void isr() {
  Serial.println("MCU is now awake");
}


/*
 * Helper functions
 */
 
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

void LED_YELLOW() {
    //Turns LED Yellow
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
    return;
}

void LED_CYAN() {
    //Turns LED Cyan
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);
    return;
}

void LED_MAGENTA() {
    //Turns LED Magenta
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, HIGH);
    return;
}

bool printHeader() { //Prints column headers to csv file
    myFile = SD.open(fileName, FILE_WRITE);
    
    if (myFile) { // Write to the file if it opened correctly
        Serial.println("Successfully opened file on SD card!");
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
        Serial.println("Successfully printed header to file on SD card!");
        return true;
    }
    else {
        Serial.println("Failed to open file on SD card");
        return false;
    }
}

// Writes most recent values to the csv file
bool writeToFile(DateTime now, double CO, double CO2, String pm2_5, String pm10) {
    myFile = SD.open(fileName, FILE_WRITE);
    
    if (myFile) {  // Write to the file if it opened correctly
        Serial.println("Successfully opened file on SD card!");

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
        myFile.print(pm2_5);    //PM 2.5um concentration,
        myFile.print(",      ");
        myFile.print(pm10);    //PM 10um concentration,
        myFile.print(",      ");
        myFile.println("      ");
        myFile.close();
        
        Serial.println("Successfully written data to file on SD card!");
        
        return true;
    }
    else {
        Serial.println("Failed to open file on SD card");
        return false;
    }
}

void initSensors(bool pmInit, bool coInit, bool co2Init, bool rtcInit) {
  if (pmInit) {
    pm.reset_measurement();
  }
  if (co2Init) {
    co2.scd30.begin();  // Intializes CO2 sensor
    co2.scd30.setMeasurementInterval(1);  // Fastest communication time with CO2 Sensor
  }
  if (coInit) {
    
  }
  if (rtcInit) {
    rtc.begin();  // Initializes real time clock, uses RTC library
    
    if (!rtc.isrunning()) {
      Serial.println("RTC is not running! Setting __DATE__ and __TIME__ to the date and time of last compile.");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));   // Will set the real time clock to the time from the computer system

  }
}
