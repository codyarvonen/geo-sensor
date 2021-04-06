// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// PM.cpp
// This file contains function declarations for the PM sensor.

#include "PM.h"

void PM::measure() {
  my_status = start_measurement();
  my_status = read_measurement(); 

  //Convert floats to strings in order to print to SD card
  String stringPM01 = String(f_PM01);
  String stringPM25 = String(f_PM25);
  String stringPM4 = String(f_PM4);
  String stringPM10 = String(f_PM10);

  //TODO: Print statements commented; can bu uncommented for testing purposes
  /*Serial.print("Read measurement status is ");
  Serial.println(my_status, BIN);
  Serial.print("PM1.0 value is ");
  Serial.println(f_PM01);
  Serial.print("PM2.5 value is ");
  Serial.println(f_PM25);
  Serial.print("PM4.0 value is ");
  Serial.println(f_PM4);
  Serial.print("PM10 value is ");
  Serial.println(f_PM10);
  Serial.println(" ");*/

  //Currently only PM2.5 and PM10 are printed to the .csv file, the other values can be returned if desired
  pm25 = stringPM25;
  pm10 = stringPM10;
  
  my_status = stop_measurement();
}

bool PM::start_measurement(void)
{
  // First, we send the command
  byte start_measurement[] = {0x68, 0x01, 0x01, 0x96 };
  Serial3.write(start_measurement, sizeof(start_measurement));
  //Then we wait for the response
  while(Serial3.available() < 2);
  byte read1 = Serial3.read();
  byte read2 = Serial3.read();
  // Test the response
  if ((read1 == 0xA5) && (read2 == 0xA5)){
    // ACK
    return 1;
  }
  else if ((read1 == 0x96) && (read2 == 0x96))
  {
    // NACK
    return 0;
  }
  else return 0;
}

bool PM::stop_measurement(void)
{
  // First, we send the command
  byte stop_measurement[] = {0x68, 0x01, 0x02, 0x95 };
  Serial3.write(stop_measurement, sizeof(stop_measurement));
  //Then we wait for the response
  while(Serial3.available() < 2);
  byte read1 = Serial3.read();
  byte read2 = Serial3.read();
  // Test the response
  if ((read1 == 0xA5) && (read2 == 0xA5)){
    // ACK
    return 1;
  }
  else if ((read1 == 0x96) && (read2 == 0x96))
  {
    // NACK
    return 0;
  }
  else return 0;
}

bool PM::read_measurement (void)
{
  // Send the command 0x68 0x01 0x04 0x93
  byte read_particle[] = {0x68, 0x01, 0x04, 0x93 };
  Serial3.write(read_particle, sizeof(read_particle));
  // A measurement can return 0X9696 for NACK
  // Or can return eight bytes if successful
  // We wait for the first two bytes
  while(Serial3.available() < 1);
  delay(1000); 
  byte HEAD = Serial3.read();
  //Serial.println(HEAD, BIN);
  //Serial.println(0x40, BIN);
  while(Serial3.available() < 1);
  delay(1000);
  byte LEN = Serial3.read();
  //Serial.println(LEN, BIN);
  //Serial.println(0xd, BIN); //was 0x05 but now 0xd
  // Test the response
  if ((HEAD == 0x96) && (LEN == 0x96)){
    // NACK
    Serial.println("NACK");
    return 0;
  }
  else if ((HEAD == 0x40) && (LEN == 0xd))//was 0x05 but now 0xd
  {
    // The measurement is valid, read the rest of the data 
    // wait for the next byte
    // TODO: test print statements commented out; can un-comment for testing purposes 
    while(Serial3.available() < 1);
    byte COMD = Serial3.read();
    //Serial.println(COMD, HEX);
    
    while(Serial3.available() < 1);
    byte DF1 = Serial3.read();
    //Serial.println(DF1, HEX);
    
    while(Serial3.available() < 1);
    byte DF2 = Serial3.read();
    //Serial.println(DF2, HEX);     
    
    while(Serial3.available() < 1);
    byte DF3 = Serial3.read();
    //Serial.println(DF3, HEX); 
      
    while(Serial3.available() < 1);
    byte DF4 = Serial3.read();
    //Serial.println(DF4, HEX);         
    while(Serial3.available() < 1);
    byte DF5 = Serial3.read();
    //Serial.println(DF5, HEX);
    while(Serial3.available() < 1);
    byte DF6 = Serial3.read();
    //Serial.println(DF6, HEX);     
    while(Serial3.available() < 1);
    byte DF7 = Serial3.read();
    //Serial.println(DF7, HEX);   
    while(Serial3.available() < 1);
    byte DF8 = Serial3.read();
    //Serial.println(DF8, HEX);     
    while(Serial3.available() < 1);
    byte DF9 = Serial3.read();
    //Serial.println(DF9, HEX);
    while(Serial3.available() < 1);
    byte DF10 = Serial3.read();
    //Serial.println(DF10, HEX);     
    while(Serial3.available() < 1);
    byte DF11 = Serial3.read();
    //Serial.println(DF11, HEX);   
    while(Serial3.available() < 1);
    byte DF12 = Serial3.read();
    //Serial.println(DF12, HEX);     
    while(Serial3.available() < 1);
    byte CS = Serial3.read();
    //Serial.println(CS, HEX);
  
       
    // Now we shall verify the checksum
    if (((0x10000 - HEAD - LEN - COMD - DF1 - DF2 - DF3 - DF4 - DF5 - DF6 - DF7 - DF8 - DF9 - DF10 - DF11 - DF12) % 0x100) != CS){ 
      Serial.println("Checksum fail");
      return 0;
    }
    else
    {
      // Checksum OK, we compute PM values
      byte PM01 = (DF1 * 0x100) + DF2;
      byte PM25 = (DF3 * 0x100) + DF4;
      byte PM4 = (DF5 * 0x100) + DF6;
      byte PM10 = (DF7 * 0x100) + DF8;  
      f_PM01 = float(PM01);
      f_PM25 = float(PM25);
      f_PM4 = float(PM4);
      f_PM10 = float(PM10);
      return 1;
    }
  }
}

bool PM::stop_autosend(void)
{
 // Stop auto send
  byte stop_autosend[] = {0x68, 0x01, 0x20, 0x77};
  Serial3.write(stop_autosend, sizeof(stop_autosend));
  //Then we wait for the response
  Serial.print("stop autosend sent \n");
  while(Serial3.available() < 2);
  byte read1 = Serial3.read();
  byte read2 = Serial3.read();
  // Test the response
  if ((read1 == 0xA5) && (read2 == 0xA5)){
    // ACK
    Serial.print(read1, HEX);
    Serial.print(read2, HEX);
    return 1;
  }
  else if ((read1 == 0x96) && (read2 == 0x96))
  {
    // NACK
    Serial.print(read1,HEX);
    Serial.print(read2,HEX);    return 0;
  }
  else{Serial.print(read1, HEX);Serial.print(read2, HEX);; return 0;}
}

bool PM::start_autosend(void)
{
 // Start auto send
  byte start_autosend[] = {0x68, 0x01, 0x40, 0x57 };
  Serial3.write(start_autosend, sizeof(start_autosend));
  //Then we wait for the response
  while(Serial3.available() < 2);
  byte read1 = Serial3.read();
  byte read2 = Serial3.read();
  // Test the response
  if ((read1 == 0xA5) && (read2 == 0xA5)){
    // ACK
    return 1;
  }
  else if ((read1 == 0x96) && (read2 == 0x96))
  {
    // NACK
    return 0;
  }
  else return 0;
}
