//GEO Sensor Code
//C.O.N.D.O.R.S. 2020-2021

#include <SPI.h>
#include <SD.h> //For reading and writing to SD card
#include <Wire.h>
#include <SparkFun_SCD30_Arduino_Library.h>
#include <EEPROM.h>
#include <String.h>
#include <Wire.h>
#include <math.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
#include "LinearRegression.h"

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
int CO_Vref = A2; //Analog pin, Pin #2 on
int Vtemp = A1;
int CO_Power = A0; //Pin number for MOSFET providing power to the CO Sensor

//CO2 Setup:
SCD30 co2AirSensor;
int CO2_Power = 13; //Pin number for MOSFET providing power to the CO2 Sensor

//PM Setup:
int PM_Power = 9; //Pin number for MOSFET providing power to the PM Sensor

//Addresses for each permanent variable to be stored for calibration
//Each calibration parameter will be stored as a double at the given address
int address_sensitivity = 0; //For CO sensor
int address_voffset = 8; //For CO sensor
int address_m = 16; //For CO2 sensor
int address_b = 24; //For CO2 sensor

//Default value for each parameter
//To be used when the system is re-set or if the calibrated value cannot be read
double default_sensitivity = 2.56; //For CO sensor
double default_voffset = -.008; //For CO sensor
double default_m = 11.5; //For CO2 sensor
double default_b = -4000; //For CO2 sensor

//Initial value for each parameter
double sensitivity = default_sensitivity; //For CO sensor
double voffset = default_voffset; //For CO sensor
double m = default_m; //For CO2 sensor
double b = default_b; //For CO2 sensor

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

void loop() {

    Serial.println(digitalRead(MODE_SWITCH));

    if ( digitalRead(MODE_SWITCH) == LOW ) {
        //Check if calibration mode
        LED_BLUE();
        Serial.println("Calibrate");
        calibrate();
        LED_RED();

        while ( digitalRead(MODE_SWITCH) == LOW ) {
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


    int num_trials = 5; //The number of measurements that each sensor will take
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
    double CO_value = average(CO_sum, num_trials);
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

void setup() {
    sensors_on();
    LED_ON();
    Wire.begin();
    Serial.begin(9600); //Begin serial communication for printing to Serial Monitor
    if (! rtc.begin()) { //Tries to connect with the clock
        Serial.println("Couldn't find RTC");
    }
    Serial3.begin(9600); //For communicating with PM sensor
    co2AirSensor.begin();
    co2AirSensor.setMeasurementInterval(1); //Fastest communication time with CO2 Sensor
    co2AirSensor.beginMeasuring();
    pinMode(CO_Power, OUTPUT);
    pinMode(CO2_Power, OUTPUT);
    pinMode(PM_Power, OUTPUT);
    pinMode(Fan_Power, OUTPUT);
    pinMode(SD_Power, OUTPUT);
    pinMode(53, OUTPUT);
    pinMode(MODE_SWITCH, INPUT);
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
        if (!SD_connect){
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

//Regression variables
LinearRegression lr = LinearRegression(0,100);

double values[3];
float C_values[3];
float CO_values[3];
float CO2_values[3];

//Measured Variables:
float(CO_Tank); //PPM of CO in the gas tank - should be input by user in GUI as well
float(CO2_Tank);  //PPM of CO2 in the gas tank - should be input by user in GUI as well

//Variables from First Run
float(var11); //Height of CO reported by User in GUI
float(var12); //Height of both flows reported by User in GUI
float(var13); //Height of air reported by User in GUI
float(COvolt1); //Voltage read from test 1 - This should come from the variable "vgas"
float(CO2volt1); // Whatever the reaw reading is from the CO2 Sensor

//Variables from Second Run
float(var21); //Height of CO reported by User in GUI
float(var22); //Height of both flows reported by User in GUI
float(var23); //Height of air reported by User in GUI
float(COvolt2); //Voltage read from test 2 - This should come from the variable "vgas"
float(CO2volt2); // Whatever the reaw reading is from the CO2 Sensor

//Variables from Third Run
float(var31); //Height of CO reported by User in GUI
float(var32); //Height of both flows reported by User in GUI
float(var33); //Height of air reported by User in GUI
float(COvolt3); //Voltage read from test 3 - This should come from the variable "vgas"
float(CO2volt3); // Whatever the reaw reading is from the CO2 Sensor


//Calculated Variables:
//Variables from First Run
float(flow11); //Flow of CO calculated from height reported in GUI
float(flow12); //Flow of Both gasses calculated from height reported in GUI
float(flow13); //Flow of air calculated from height reported in GUI
float(CO_conc1); //Concentration of CO calculated from flows
float(CO2_conc1); //Concentration of CO2 calculated from flows

//Variables from Second Run
float(flow21); //Flow of CO calculated from height reported in GUI
float(flow22); //Flow of Both gasses calculated from height reported in GUI
float(flow23); //Flow of air calculated from height reported in GUI
float(CO_conc2); //Concentration of CO calculated from flows
float(CO2_conc2); //Concentration of CO2 calculated from flows

//Variables from Third Run
float(flow31); //Flow of CO calculated from height reported in GUI
float(flow32); //Flow of Both gasses calculated from height reported in GUI
float(flow33); //Flow of air calculated from height reported in GUI
float(CO_conc3); //Concentration of CO calculated from flows
float(CO2_conc3); //Concentration of CO2 calculated from flows

float(slope_CO); //final regressed slope of CO curve
float(slope_CO2); // final regressed slope of CO2 curve
float(inter_CO); //final regressed intercept of CO curve
float(inter_CO2); //final regressed intercept of C2 curve

//These functions are for reading and writing to EEPROM
float readFloat(unsigned int addr)
{
    union
    {
        byte b[4];
        float f;
    } data;
    for(int i = 0; i < 4; i++)
    {
        data.b[i] = EEPROM.read(addr+i);
    }
    return data.f;
}

void writeFloat(unsigned int addr, float x)
{
    union
    {
        byte b[4];
        float f;
    } data;
    data.f = x;
    for(int i = 0; i < 4; i++)
    {
        EEPROM.write(addr+i, data.b[i]);
    }
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

void regressionLoop () {
    int steady_state_time(0);
    int dummy(0);
    // put your main code here, to run repeatedly:

    Serial.println("\n\n\n\n\n press the ENTER key to start calibration or switch the calibration setting to off and restart the deivice to exit calibration mode \n");
    while(!Serial.available())delay(10);
    dummy = Serial.parseFloat(); //Read the data the user has input
    Serial.println(dummy);
    delay(1000);

    //get initial CO and CO2 values
    Serial.println("Enter PPM of CO in the gas tank and press enter\n");
    while(!Serial.available())delay(10);
    CO_Tank = Serial.parseFloat(); //Read the data the user has input
    Serial.println(CO_Tank);
    delay(1000);

    Serial.println("Enter PPM of CO2 in the gas tank and press enter\n");
    while(!Serial.available())delay(10);
    CO2_Tank = Serial.parseFloat(); //Read the data the user has input
    Serial.println(CO2_Tank);
    delay(1000);

    //steady state time entry only for prototype
    Serial.println("Enter time to reach steady state (seconds)\n");
    while(!Serial.available())delay(10);
    steady_state_time = 1000*Serial.parseFloat(); //Read the data the user has input
    Serial.println(dummy);
    delay(1000);

    //MEASUREMENT 1
    Serial.println("Turn gas on, record heights and press ENTER once heights are recorded\n");
    while(!Serial.available())delay(10);
    dummy = Serial.parseFloat(); //Read the data the user has input
    delay(steady_state_time);

    //float(var11); //Height of CO reported by User in GUI
    //float(var12); //Height of both flows reported by User in GUI
    //float(var13); //Height of air reported by User in GUI
    Serial.println("Type Height of CO\n");
    while(!Serial.available())delay(10);
    var11 = Serial.parseInt(); //Read the data the user has input

    Serial.println("Type Height of both flows\n");
    while(!Serial.available())delay(10);
    var12 = Serial.parseInt(); //Read the data the user has input

    Serial.println("Type Height of air\n");
    while(!Serial.available())delay(10);
    var13 = Serial.parseInt(); //Read the data the user has inpu

    //measure C02
    CO2volt1 = measure_CO2();
    //measure C0
    COvolt1 = measure_CO();

    //wait for device to reach steady state
    delay(steady_state_time);

    //MEASUREMENT 2
    Serial.println("Change gas levels, record heights and press ENTER once heights are recorded\n");
    //User for Input
    while (Serial.available() == 0) {
        // Wait for User to Input Data
    }
    dummy = Serial.parseInt(); //Read the data the user has input
    //float(var21); //Height of CO reported by User in GUI
    //float(var22); //Height of both flows reported by User in GUI
    //float(var23); //Height of air reported by User in GUI
    Serial.println("Type Height of CO\n");
    //User for Input
    while (Serial.available() == 0) {
        // Wait for User to Input Data
    }
    var21 = Serial.parseInt(); //Read the data the user has input
    Serial.println("Type Height of both flows\n");
    //User for Input
    while (Serial.available() == 0) {
        // Wait for User to Input Data
    }
    var22 = Serial.parseInt(); //Read the data the user has input
    Serial.println("Type Height of air\n");
    //User for Input
    while (Serial.available() == 0) {
        // Wait for User to Input Data
    }
    var23 = Serial.parseInt(); //Read the data the user has input
    //measure C02
    CO2volt2 = measure_CO2();
    //measure C0
    COvolt2 = measure_CO2();

    //wait for device to reach steady state
    delay(steady_state_time);

    //MEASUREMENT 3
    Serial.println("Change gas levels, record heights and press ENTER once heights are recorded\n");
    //User for Input
    while (Serial.available() == 0) {
        // Wait for User to Input Data
    }
    dummy = Serial.parseInt(); //Read the data the user has input
    //float(var31); //Height of CO reported by User in GUI
    //float(var32); //Height of both flows reported by User in GUI
    //float(var33); //Height of air reported by User in GUI
    Serial.println("Type Height of CO\n");
    //User for Input
    while (Serial.available() == 0) {
        // Wait for User to Input Data
    }
    var31 = Serial.parseInt(); //Read the data the user has input
    Serial.println("Type Height of both flows\n");
    //User for Input
    while (Serial.available() == 0) {
        // Wait for User to Input Data
    }
    var32 = Serial.parseInt(); //Read the data the user has input
    Serial.println("Type Height of air\n");
    //User for Input
    while (Serial.available() == 0) {
        // Wait for User to Input Data
    }
    var33 = Serial.parseInt(); //Read the data the user has input
    //measure C02
    CO2volt3 = measure_CO2();
    //measure C0
    COvolt3 = measure_CO2();

    //wait for device to reach steady state
    delay(steady_state_time);

    // Pull variables var11, var12, var13,volt1 etc... for all three runs from GUI

    //Calculate the flows
    flow11 = find_flow(var11);
    flow12 = find_flow(var12);
    flow13 = find_flow(var13);
    flow21 = find_flow(var21);
    flow22 = find_flow(var22);
    flow23 = find_flow(var23);
    flow31 = find_flow(var31);
    flow32 = find_flow(var32);
    flow33 = find_flow(var33);

    //Calculate the compositions
    CO_conc1 = C_Comp(flow11,flow12,flow13,CO_Tank);
    CO2_conc1 = C_Comp(flow11,flow12,flow13,CO2_Tank);

    CO_conc2 = C_Comp(flow21,flow22,flow23,CO_Tank);
    CO2_conc2 = C_Comp(flow21,flow22,flow23,CO2_Tank);

    CO_conc3 = C_Comp(flow31,flow32,flow33,CO_Tank);
    CO2_conc3 = C_Comp(flow31,flow32,flow33,CO2_Tank);



//THESE FINAL FOUR PARAMTERS ARE WHAT NEEDS TO BE IMPLEMENTED INTO THE FINAL CODE

    slope_CO = line_regress_slope(CO_conc1,CO_conc2,CO_conc3,COvolt1,COvolt2,COvolt3);
    slope_CO2 = line_regress_slope(CO2_conc1,CO2_conc2,CO2_conc3,CO2volt1,CO2volt2,CO2volt3);

    inter_CO = line_regress_inter(CO_conc1,CO_conc2,CO_conc3,COvolt1,COvolt2,COvolt3);
    inter_CO2 = line_regress_inter(CO2_conc1,CO2_conc2,CO2_conc3,CO2volt1,CO2volt2,CO2volt3);

    writeFloat(0, slope_CO);
    writeFloat(4, slope_CO2);
    writeFloat(8, inter_CO);
    writeFloat(12, inter_CO2);

    testFunction();
}

void testFunction() {
    Serial.print("value of slope_CO: ");
    Serial.println(readFloat(0));
    Serial.print("value of slope_CO2: ");
    Serial.println(readFloat(4));
    Serial.print("value of inter_CO: ");
    Serial.println(readFloat(8));
    Serial.print("value of inter_CO2: ");
    Serial.println(readFloat(12));
}

//This function calculates the composition of CO or CO2 in the gas mix

float C_Comp(float(C),float(B),float(A),float(ppm)){ //Calculate composition of CO or CO2 in the gas mix given flows
    //C represents the flow of the stream with CO and CO2, B represents the flow of both stream, A represents the flow of the Air stream, ppm represents the tank concentration of CO or CO2
    float comp;
    comp = C/B*ppm;  //composition calculated in ppm  **I NEED TO RUN A FEW MORE ANALYSIS TO MAKE CERTAIN THIS IS RIGHT EQUATION
    return comp; //return composition
}

//This function linearly regresses the desired line slope

float line_regress_slope(float(C1),float(C2),float(C3),float(V1),float(V2),float(V3)){
    // C1,C2,C3 are three compositions and V1,V2,V3 are the voltages associated with those compositions

    lr.learn(C1,V1);
    lr.learn(C2,V2);
    lr.learn(C3,V3);

    lr.calculate(3);
    lr.correlation();
    lr.getValues(values);

    return values[0];
}

//This function linearly regresses the desired line intercept

float line_regress_inter(float(C1),float(C2),float(C3),float(V1),float(V2),float(V3)){
    // C1,C2,C3 are three compositions and V1,V2,V3 are the voltages associated with those compositions

    lr.learn(C1,V1);
    lr.learn(C2,V2);
    lr.learn(C3,V3);

    lr.calculate(3);
    lr.correlation();
    lr.getValues(values);

    return values[1];
}

//this function linearly regresses flow in mL/min of gas

float find_flow(float(x)){ //This function will find the flow of the gas mixing based off of the paramters in the rotary flow meter.
    // It will preform a linear regression between the flows given on the data sheet for the flow meter to estimate the flows of the actual system.
    float result;
    // These if else statements tell the code which to flows to regress between from the data sheet

    if (x>=0 && x<10){
        result = float(0) + ((x-float(0))/(float(10)-float(0)))*(float(269)-float(0));
    }
    else if (x>=10 && x<20){
        result = float(269) + ((x-float(10))/(float(20)-float(10)))*(float(522)-float(269));
    }
    else if (x>=20 && x<30){
        result = float(522) + ((x-float(20))/(float(30)-float(20)))*(float(761)-float(522));
    }
    else if (x>=30 && x<40){
        result = float(761) + ((x-float(30))/(float(40)-float(30)))*(float(986)-float(761));
    }
    else if (x>=40 && x<50){
        result = float(986) + ((x-float(40))/(float(50)-float(40)))*(float(1197)-float(986));
    }
    else if (x>=50 && x<60){
        result = float(1197) + ((x-float(50))/(float(60)-float(50)))*(float(1411)-float(1197));
    }
    else if (x>=60 && x<70){
        result = float(1411) + ((x-float(60))/(float(70)-float(60)))*(float(1622)-float(1411));
    }
    else if (x>=70 && x<80){
        result = float(1622) + ((x-float(70))/(float(80)-float(70)))*(float(1805)-float(1622));
    }
    else if (x>=80 && x<90){
        result = float(1805) + ((x-float(80))/(float(90)-float(80)))*(float(1994)-float(1805));
    }
    else if (x>=90 && x<100){
        result = float(1994) + ((x-float(90))/(float(100)-float(90)))*(float(2191)-float(1994));
    }
    else if (x>=100 && x<110){
        result = float(2191) + ((x-float(100))/(float(110)-float(100)))*(float(2346)-float(2191));
    }
    else if (x>=110 && x<120){
        result = float(2346) + ((x-float(110))/(float(120)-float(110)))*(float(2493)-float(2346));
    }
    else if (x>=120 && x<130){
        result = float(2493) + ((x-float(120))/(float(130)-float(120)))*(float(2658)-float(2493));
    }
    else if (x>=130 && x<140){
        result = float(2658) + ((x-float(130))/(float(140)-float(130)))*(float(2831)-float(2658));
    }
    else if (x>=140 && x<150){
        result = float(2831) + ((x-float(140))/(float(150)-float(140)))*(float(2975)-float(2831));
    }
    else {
        //I need this to print an error screen to the GUI because we cannot have a reading higher than 150 or lower than 0
    }
    return result;

}

void calibrate() {
    //This function will interract with the GUI and incorperate the calibration code.
    //The current code was for testing the connection with a basic gui
    LED_BLUE(); //indicates calibration mode

    regressionLoop();

    /*double n1 = 0;
    n1 = get_Number_From_Serial();
    double n2 = 0;
    n2 = get_Number_From_Serial();

    Serial.println("Number 1 = " + String(n1));
    for (int i = 0; i < 10; i++) {
        LED_ON();
        delay(500);
        LED_BLUE();
        delay(500);

    Serial.println("5");*/

}

void FACTORY_RESET() {
    //Reset all calibration values to default settings

}

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
    while (!co2AirSensor.dataAvailable())
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

/*****************************************
*This function writes to SD card
//in example (https://youtu.be/oTXi6kYg0D4)
    //using pins 11, 12, 13 as SPI protocol (MOSI, MISO, SCLK)
    //CS on pin 4 (this is the pin that sends the serial communication)
    //pwr an gnd
//on the mega
    //using  d50 (miso), d51 (mosi), d52 (sclk), d53 (cs), d48 (pwr), 

void setup() {
    //open serial communication and wait for port to open:
    Serial.begin(9600);

    while(!Serial){
        ; //wait for serial port to connect.  Needed for native USB port only
    }

Serial.print("Initializing SD card...");

if (!SD.begin(4)){
    Serial.println("initialization failed");
    while (1);
}
Serial.println("initialization done");

if (SD.exists("example.txt")){
    Serial.println("example.txt exists");
}
else{
    Serial.println("example.txt doesn't exist");
}

//open new file and immediately close it:
Serial.println("creating example.txt...");
myFile = SD.open("example.txt", FILE_WRITE);
myFile.write("Hello");
Serial.println("Writing to file successful");
myfile.close();
}

void loop(){
    //do nothing
}

******************************************/

void write_to_file(DateTime now, double CO, double CO2, double pm25, double pm10) {
    //Writes most recent values to the excel file

    myFile = SD.open(file_name, FILE_WRITE);
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
