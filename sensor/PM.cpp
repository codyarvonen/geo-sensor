// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// PM.cpp
// This file contains function declarations for the PM sensor.

#include "PM.h"

double PM::measure25() {
      //This comes from Joseph's code from 2018-2019
    //Should return the concentration of particulate matter 2.5um and larger

    Serial3.begin(9600); //For communicating with PM sensor
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


double PM::measure10() {
      //This comes from Joseph's code from 2018-2019
    //Should return the concentration of particulate matter 2.5um and larger

    Serial3.begin(9600); //For communicating with PM sensor
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
