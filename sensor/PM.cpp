// GEO Cookstove Sensor Code
// Los Encendidos 2021-2022
// Arduino code for the cookstove sensor.
// PM.cpp
// This file contains function declarations for the PM sensor.

#include "PM.h"

void PM::measure() {
  my_status = read_measurement();

  // Convert floats to strings in order to print to SD card
  String stringPM2_5 = String(f_PM2_5G);
  String stringPM10 = String(f_PM10G);

  //Currently only PM2.5 and PM10 are printed to the .csv file, the other values can be returned if desired
  pm2_5 = stringPM2_5;
  pm10 = stringPM10;
}

bool PM::start_measurement(void) {
  
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

bool PM::stop_measurement(void) {
  
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

bool PM::read_measurement (void) {
  
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

  // Iterate through the input until all data bytes are collected
  for (int i = 0; i < 52; i++) { 
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

  // NOTE: The calculation method for the checksum and data values 
  //       is outlined on the PM sensor datasheet. However, we had 
  //       to make updates to the way the checksum is calculated.
  //       If response_sum is larger than 256, we found that the 
  //       checksum will always fail because it will be negative. 
  //       This caused the a checksum fail every time we tested the 
  //       sensor in an environment with lots of particulate matter.
  //       So, for every factor over 256 response_sum ends up being, 
  //       we multiply it by 0x100 (256 in hex) before we subtract 
  //       response_sum. 
  
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
  my_status = stop_measurement();
  delay(1000);
  my_status = start_measurement();
  delay(1000);
}
