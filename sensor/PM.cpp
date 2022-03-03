// GEO Cookstove Sensor Code
// Los Encendidos 2021-2022
// Arduino code for the cookstove sensor.
// PM.cpp
// This file contains function declarations for the PM sensor.
// Much of this code is from https://medium.com/electronza/arduino-measuring-pm2-5-and-pm10-with-honeywell-hpma115s0-703f384c485a


#include "PM.h"

void PM::measure() {
//  my_status = start_measurement(); 
  my_status = read_measurement();  // The read_measurement function stops and then starts the sensor measurement 

  // Convert floats to strings in order to print to SD card
  String stringPM2_5 = String(f_PN2_5);
  String stringPM10 = String(f_PN10);

  // Print statements commented; can be uncommented for testing purposes
  /*Serial.print("Read measurement status is ");
    Serial.print("PM1.0 GRIMM value is ");
    Serial.println(f_PM1_0G);
    Serial.print("PM2.5 GRIMM value is ");
    Serial.println(f_PM2_5G);
    Serial.print("PM10 GRIMM value is ");
    Serial.println(f_PM10G);
    Serial.print("PM1.0 TSI value is ");
    Serial.println(f_PM1_0T);
    Serial.print("PM2.5 TSI value is ");
    Serial.println(f_PM2_5T);
    Serial.print("PM10 TSI value is ");
    Serial.println(f_PM10T);
    Serial.print("Particles number >0.3um is ");
    Serial.println(f_PN0_3);
    Serial.print("Particles number >0.5um is ");
    Serial.println(f_PN0_5);
    Serial.print("Particles number >1.0um is ");
    Serial.println(f_PN1_0);
    Serial.print("Particles number >2.5um is ");
    Serial.println(f_PN2_5);
    Serial.print("Particles number >5.0um is ");
    Serial.println(f_PN5_0);
    Serial.print("Particles number >10um is ");
    Serial.println(f_PN10);
    Serial.println(" ");*/

  //Currently only PM2.5 and PM10 are printed to the .csv file, the other values can be returned if desired
  pm2_5 = stringPM2_5;
  pm10 = stringPM10;
  
//  my_status = stop_measurement();
}

bool PM::start_measurement(void)
{
  // First, we send the command
  byte start_measurement[] = {0x11, 0x03, 0x0C, 0x02, 0x1E, 0xC0};
  Serial3.write(start_measurement, sizeof(start_measurement));
  // Then we wait for the responce
  while(Serial3.available() < 1);
  byte HEAD = Serial3.read();
  while(Serial3.available() < 1);
  byte LEN = Serial3.read();
  while(Serial3.available() < 1);
  byte CMD = Serial3.read();
  while(Serial3.available() < 1);
  byte DF1 = Serial3.read();
  while(Serial3.available() < 1);
  byte CS = Serial3.read();
  // Test the response
  if ((0x100 - HEAD - LEN - CMD - DF1) != CS) { 
    Serial.println("Start measurement checksum fail");
    return false;
  }
  else {
    Serial.println("Start measurement checksum success!");
    return true;
  }
}

bool PM::stop_measurement(void)
{
  // First, we send the command
  byte stop_measurement[] = {0x11, 0x03, 0x0C, 0x01, 0x1E, 0xC1};
  Serial3.write(stop_measurement, sizeof(stop_measurement));
  // Then we wait for the responce
  while(Serial3.available() < 1);
  byte HEAD = Serial3.read();
  while(Serial3.available() < 1);
  byte LEN = Serial3.read();
  while(Serial3.available() < 1);
  byte CMD = Serial3.read();
  while(Serial3.available() < 1);
  byte DF1 = Serial3.read();
  while(Serial3.available() < 1);
  byte CS = Serial3.read();
  // Test the response
  if ((0x100 - HEAD - LEN - CMD - DF1) != CS) { 
    Serial.println("Stop measurement checksum fail");
    return false;
  }
  else {
    return true;
  }
}

bool PM::read_measurement (void)
{
  // First, we send the command
  byte read_measurement[] = {0x11, 0x02, 0x0B, 0x07, 0xDB};
  Serial3.write(read_measurement, sizeof(read_measurement));
  // Then we wait for the responce
  while(Serial3.available() < 1);
  byte HEAD = Serial3.read();
  while(Serial3.available() < 1);
  byte LEN = Serial3.read();
  while(Serial3.available() < 1);
  byte CMD = Serial3.read();
  byte DF[52];
  for (int i = 0; i < 52; i++) { // Iterate through the input until all data bytes are collected
    while(Serial3.available() < 1);
    DF[i] = Serial3.read();
  }
  while(Serial3.available() < 1);
  byte CS = Serial3.read();
  // Then we add all the data bytes together and store it as an int
  int DATA = 0;
  for (int i = 0; i < 52; i++) {
    DATA += DF[i];
  }

  int response_sum = HEAD + LEN + CMD + DATA;
  int factor = (response_sum/256) + 1;

  // Uncomment for debbugging
  /*Serial.println("HEAD = ");
  Serial.println(HEAD);
  Serial.println("LEN = ");
  Serial.println(LEN);
  Serial.println("CMD = ");
  Serial.println(CMD);
  Serial.println("DATA = ");
  Serial.println(DATA);
  Serial.println("CS = ");
  Serial.println(CS);
  Serial.println("Response Sum = ");
  Serial.println(response_sum);
  Serial.println("Factor = ");
  Serial.println(factor);
  Serial.println("((0x100 * factor) - response_sum) = ");
  Serial.println(((0x100 * factor) - response_sum));*/
  
  // Test the response
  if (((0x100 * factor) - response_sum) != CS) { 
    Serial.println("Read measurement checksum FAIL");
    reset_measurement();
    return false;
  }
  else {
    Serial.println("Computing values");
    // Compute PM values
    f_PM1_0G = DF[0]*0x1000000 + DF[1]*0x10000 + DF[2]*0x100 + DF[3];
    f_PM2_5G = DF[4]*0x1000000 + DF[5]*0x10000 + DF[6]*0x100 + DF[7];
    f_PM10G = DF[8]*0x1000000 + DF[9]*0x10000 + DF[10]*0x100 + DF[11];
    f_PM1_0T = DF[12]*0x1000000 + DF[13]*0x10000 + DF[14]*0x100 + DF[15];
    f_PM2_5T = DF[16]*0x1000000 + DF[17]*0x10000 + DF[18]*0x100 + DF[19];
    f_PM10T = DF[20]*0x1000000 + DF[21]*0x10000 + DF[22]*0x100 + DF[23];
    f_PN0_3 = DF[24]*0x1000000 + DF[25]*0x10000 + DF[26]*0x100 + DF[27];
    f_PN0_5 = DF[28]*0x1000000 + DF[29]*0x10000 + DF[30]*0x100 + DF[31];
    f_PN1_0 = DF[32]*0x1000000 + DF[33]*0x10000 + DF[34]*0x100 + DF[35];
    f_PN2_5 = DF[36]*0x1000000 + DF[37]*0x10000 + DF[38]*0x100 + DF[39];
    f_PN5_0 = DF[40]*0x1000000 + DF[41]*0x10000 + DF[42]*0x100 + DF[43];
    f_PN10 = DF[44]*0x1000000 + DF[45]*0x10000 + DF[46]*0x100 + DF[47];
    
    return true;
  }
}

void PM::reset_measurement(void) {
//  Serial.println("RESET MEASUREMENT");
//  Serial.println("Closing and opening measurment ");
//  delay(1000);
  
  my_status = stop_measurement();
//  Serial.println("Stop measurement status is ");
//  Serial.println(my_status);
  delay(1000);

  my_status = start_measurement();
//  Serial.println("Start measurement status is ");
//  Serial.println(my_status);
  delay(1000);
}
