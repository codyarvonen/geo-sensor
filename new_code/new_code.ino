//Code for Everything
//Creator: Tanner and Kayci
//Start Date: 10/10/19

#include <SPI.h>
#include <SD.h> //For reading and writing to SD card
#include <Wire.h>
#include <SparkFun_SCD30_Arduino_Library.h>
#include <EEPROM.h>
#include <String.h>
#include <Wire.h>
#include <math.h>
#include <SoftwareSerial.h>
#include "RTClib.h"





File myFile; //File for writing to SD card
String file_name = "datos.csv";
RTC_PCF8523 rtc; // Real time clock. The time is already set to PERUVIAN TIME
double measurement_interval = 60.0; //will collect data every 60 seconds
bool SD_connect = false;

int RED = 45; //Pin number for Red in RGB LED
int GREEN = 46; //Pin number for Green in RGB LED
int BLUE = 44; //Pin number for Blue in RGB LED
int MODE_SWITCH = 6; //HIGH indicates calibration mode, LOW indicates measurement mode

int Fan_Power = 5; //Pin number for MOSFET providing power to the fan
int SD_Power = 48; //Pin number for MOSFET providing power to the  SD card reader

//CO Setup:
int CO_Vgas = A3; //Analog pin, Pin #1 on sensor
int CO_Vref = A2; //Analog pin, Pin #2 on sensor
int Vtemp = A1; 
int CO_Power = A0; //Pin number for MOSFET providing power to the CO Sensor

//CO2 Setup:
SCD30 co2AirSensor;
int CO2_Power = 13; //Pin number for MOSFET providing power to the CO2 Sensor

//PM Setup:
int PM_Power = 9; //Pin number for MOSFET providing power to the PM Sensor

//Addresses for each permanent variable to be stored for calibration
//Each calibration parameter will be stored as a double at the given address
//(untested)
int address_sensitivity = 0; //For CO sensor (addresses to store the calibration parameters)
int address_voffset = 8; //For CO sensor ('')
int address_m = 16; //For CO2 sensor
int address_b = 24; //For CO2 sensor

//Default value for each parameter
//To be used when the system is re-set or if the calibrated value cannot be read
//(default parameters for calibration that were set manually)
double default_sensitivity = 2.56; //For CO sensor
double default_voffset = -.008; //For CO sensor
double default_m = 11.5; //For CO2 sensor
double default_b = -4000; //For CO2 sensor

//Initial value for each parameter
double sensitivity = default_sensitivity; //For CO sensor
double voffset = default_voffset; //For CO sensor
double m = default_m; //For CO2 sensor
double b = default_b; //For CO2 sensor

//(called once when arduino is turned on)
void setup() {
  sensors_on();
  LED_ON(); 
  Wire.begin();
  Serial.begin(9600); //Begin serial communication for printing to Serial Monitor (when connected with laptop via USB)
  if (! rtc.begin()) { //Tries to connect with the clock
    Serial.println("Couldn't find RTC");
  }
  Serial3.begin(9600); //For communicating with PM sensor
  co2AirSensor.begin();
  co2AirSensor.setMeasurementInterval(1); //Fastest communication time with CO2 Sensor
  co2AirSensor.beginMeasuring();
  pinMode(CO_Power, OUTPUT); //(these are output pins)
  pinMode(CO2_Power, OUTPUT);
  pinMode(PM_Power, OUTPUT);
  pinMode(Fan_Power, OUTPUT);
  pinMode(SD_Power, OUTPUT);
  pinMode(53, OUTPUT);
  SD.begin(53);
  
  Serial.println("CO, CO2, PM 2.5, PM 10");
  myFile = SD.open(file_name, FILE_WRITE); //We will open the file and write the headers to it
  // if the file opened okay, write to it:
  if (myFile) {
    SD_connect = true;
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
    SD_connect = false;
  }
  for (int i = 0; i < 5; i++) {
    if (!SD_connect){ //(blinks white if connection to SD was successful; also what signals does SENCICO want?)
      LED_RED();
    }
    else{
      LED_ON();
    }
    delay(500);
    LED_OFF();
    delay(500);
  }
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

//(Found online but untested)
unsigned int EEPROM_write_double(int ee, const double& value){
//For writing to the EEPROM (non-volatile memory) of the arduino
  const byte* p = (const byte*)(const void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
  return i;
}

//(found online untested)
unsigned int EEPROM_read_double(int ee, double& value){
//For reading from the EEPROM (non-volatile memory) of the arduino
  byte* p = (byte*)(void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return i;
}

double get_Number_From_Serial(){
  //For receiving input from the serial communication
    String string = "";
    
    while(Serial.available() == 0){
      delay(100);
    }
    
    while(true)
    {
      char digit = ((char)Serial.read());
      if(digit == '\n'){
      }
      if(digit == 'q'){ //Stops accepting input when a q is written
        break;
      }
      else{
        string += digit;
      }
    }

    Serial.println(string);
    return string.toDouble();
}

void calibrate() {
  //This function will interract with the GUI and incorperate the calibration code. 
  //The current code was for testing the connection with a basic gui
  LED_BLUE(); //indicates calibration mode

    double n1 = 0;
    n1 = get_Number_From_Serial();
    double n2 = 0;
    n2 = get_Number_From_Serial();
    
    Serial.println("Number 1 = " + String(n1));
    for (int i = 0; i < 10; i++){
      LED_ON();
      delay(500);
      LED_BLUE();
      delay(500);
    }
    Serial.println("5");

}

void FACTORY_RESET() {
  //Reset all calibration values to default settings

}

//IS THIS WORKING (calibration needed)
double measure_CO() {
  //Return concentration of CO in ppm
  //https://www.spec-sensors.com/wp-content/uploads/2016/10/ULPSM-CO-968-001.pdf
  double Vgas = double(analogRead(CO_Vgas)) * double(5.0 / 1023.0); //Analog voltage indicating CO concentration
  double Vref = double(analogRead(CO_Vref)) * double(5.0 / 1023.0); //Analog reference voltage
  double Vgas0 = Vref + voffset;
  double TIA_Gain = 100;
  double M = sensitivity * TIA_Gain * pow(10, -6);
  double ppm = (Vgas - Vgas0) / M;
  return ppm;
  Serial.print(Vgas);
  Serial.print(" ");
  Serial.println(Vref);

}

double measure_CO2() {
  //Return Concentration of CO2 in ppm
  double co2Val;
  double m = default_m;
  double b = default_b;
  int count = 0;
  while (!co2AirSensor.dataAvailable()) //(CO2 data isn't available immediately, does it have an interrupt?)
  {
    count ++;
    if (count > 1000) {
      LED_RED();
      return -1;
    }
  }
  co2Val = co2AirSensor.getCO2();
  LED_GREEN();
  return (co2Val * m + b);
}

double measure_PM25() { 
  //This comes from Joseph's code from 2018-2019
  //Should return the concentration of particulate matter 2.5um and larger
  
  int incomingByte = 0;
  int count = 0; //Count to know how much data coming in
  int count2 = 0;
  int count3 = 0;
  double pm25 = 0; //PM of 2.5 microns or greater
  double pm10 = 0; //PM of 10 microns or greater
  int big = 0;
  int data[101];  //Used with PM sensor
  int store[11];  //Used with PM sensor
  
  while (count < 100) {
    incomingByte = Serial3.read(); //Collect available data
    data[count] = incomingByte; //Read incoming data into array
    count = count + 1; //Increase count
  }

  while (count2 < 100 && count3 < 10){
    if (data[count2] != 170){
      count2 = count2 + 1;
    }
    else{
      while (big < 11){
        store[count3] = data[count2 + big];
        big = big + 1;
        count3 = count3 + 1;
      }
    }
  }

  //Perform math on the data:
  //Remember:   PM 2.5 low bit-3rd element in vector, PM 2.5 high bit-4th, PM10 low bit-5th, PM10 high bit-6th

  pm25 = ((store[3] * 256) + store[2]) / 10.00; //These conversions to pm values come from the data sheet for SDS011 PM sensor page 6.
  pm10 =  ((store[5] * 256) + store[4]) / 10.00;
  
  return pm25;
}

double measure_PM10() { 
  //This comes from Joseph's code from 2018-2019
  //Should return the concentration of particulate matter 10um and larger
  
  int incomingByte = 0;
  int count = 0; //Count to know how much data coming in
  int count2 = 0;
  int count3 = 0;
  double pm25 = 0; //PM of 2.5 microns or greater
  double pm10 = 0; //PM of 10 microns or greater
  int big = 0;
  int data[101];  //Used with PM sensor
  int store[11];  //Used with PM sensor
  
  while (count < 100) {
    incomingByte = Serial3.read(); //Collect available data
    data[count] = incomingByte; //Read incoming data into array
    count = count + 1; //Increase count
  }

  while (count2 < 100 && count3 < 10){
    if (data[count2] != 170){
      count2 = count2 + 1;
    }
    else{
      while (big < 11){
        store[count3] = data[count2 + big];
        big = big + 1;
        count3 = count3 + 1;
      }
    }
  }

  //Perform math on the data:
  //Remember:   PM 2.5 low bit-3rd element in vector, PM 2.5 high bit-4th, PM10 low bit-5th, PM10 high bit-6th

  pm25 = ((store[3] * 256) + store[2]) / 10.00; //These conversions to pm values come from the data sheet for SDS011 PM sensor page 6.
  pm10 =  ((store[5] * 256) + store[4]) / 10.00;
  
  return pm10;
}

void write_to_file(DateTime now, double CO, double CO2, double pm25, double pm10) {
  //Writes most recent values to the excel file

  myFile = SD.open(file_name, FILE_WRITE); //(writes during the test)
  // if the file opened okay, write to it:
  if (myFile) {
    SD_connect = true;
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
    SD_connect = false;
    LED_RED();
  }

}

double average(double values[], int len) {
  //Averages values from a set and ignores values of -1 (This program uses -1 to indicate an error)
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
void sensors_on()
{
  //Most of the sensors need to be written LOW to 
  digitalWrite(CO_Power, LOW);
  digitalWrite(CO2_Power, LOW);
  digitalWrite(PM_Power, LOW);
  digitalWrite(Fan_Power, HIGH); //
  digitalWrite(SD_Power, LOW);
}

void sensors_off()
{
  digitalWrite(CO_Power, HIGH);
  digitalWrite(CO2_Power, HIGH);
  digitalWrite(PM_Power, HIGH);
  digitalWrite(Fan_Power, LOW);
  digitalWrite(SD_Power, HIGH);
}



//(run immediately after setup)
void loop() {
//they have an if statement that calls calibration at beginning, 
//calibration can only be called at beginning of loop function
  if ( digitalRead(MODE_SWITCH) == HIGH ) {
    //Check if calibration mode
    LED_BLUE();
    Serial.println("Calibrate");
    calibrate();
    LED_RED();
    
    while ( digitalRead(MODE_SWITCH) == HIGH ) {
      delay(500);
      //Wait until they switch it back to measurement mode
    }
  }
  
  Serial.println("Measuring");
  DateTime now = rtc.now();
  DateTime end_loop (now + TimeSpan(0,0,0, measurement_interval));
  
  LED_GREEN();
  sensors_on();
  delay(2000);
  

  int num_trials = 5; //The number of measurements that each sensor will take (can change, each takes about a second)
  double CO_sum[num_trials];
  double CO2_sum[num_trials];
  double PM25_sum[num_trials];
  double PM10_sum[num_trials];

  for (int i = 0; i < num_trials; i++) {
    CO_sum[i] = measure_CO();
    CO2_sum[i] = measure_CO2();
    PM25_sum[i] = measure_PM25();
    PM10_sum[i] = measure_PM10();
    delay(100);
  }
  double CO_value = average(CO_sum, num_trials); //(custom aveg func discards err results)
  double CO2_value = average(CO2_sum, num_trials);
  double PM25_value = average(PM25_sum, num_trials);
  double PM10_value = average(PM10_sum, num_trials);

  Serial.print(CO_value);
  Serial.print(", ");
  Serial.print(CO2_value);
  Serial.print(", ");
  Serial.print(PM25_value);
  Serial.print(", ");
  Serial.print(PM10_value);
  Serial.println("");

  write_to_file(now, CO_value, CO2_value, PM25_value, PM10_value);
  delay(1000);
  sensors_off(); //Turns the sensors off when they are not in use
  //TODO: Put the arduino to sleep as well, but it still must connect to the clock and LED

  int loop_num = 0; //This variable is a fail-safe to prevent an infinite loop, in case something goes wrong with the clock. 
  //(needs some work, waits too long??)
  while(rtc.now() < end_loop && loop_num < 60){ //Waits unitl exactly a minute has passed before measuring again.
    delay(900);
    if (!SD_connect){
      LED_RED(); //Blinks red if the sensor was unable to write to the SD card
    }
    else{
      LED_GREEN(); //Blinks green once every second
    }
    delay(100);
    LED_OFF();
    }
}
