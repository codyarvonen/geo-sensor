// GEO Cookstove Sensor Code
// C.O.N.D.O.R.S. 2020-2021
// Arduino code for the cookstove sensor.
// Calibrate.cpp
// This file contains function declarations for the CO and CO2 calibration process.

//sensor interaction with user (we are thinking we want most of this to be in the instruction manual rather than in the calibration. that would make this process simpler for SENCICO)
//1. When calibration mode is turned on, gives prompt, "Press ENTER to calibrate CO & CO2 sensors"
//2. "Note that the calibration process will run several times. Connect calibration device to sensor box and then press ENTER"
//3. "Is the concentration of CO Xppm & CO2 Xppm? If yes, press ENTER, if no (ask concentrations of CO & CO2)."
//4. "Is the concentration of the N2 in the air tank Xppm? If yes, press ENTER, if no (ask concentrations N2)."
//3. "Close all valves (rotometer & gas tanks) and then press ENTER"
//4. "Open air tank and then press ENTER"
//5. "Open rotometer until flow rate reaches Xmm^3/sec and then press ENTER"
//6. "Open CO & CO2 tank until flow rate reaches Xmm^3/sec and then press ENTER"
//7. "Run calibration? Press ENTER."
// - waiting point for X amount of min. - records measurement at this concentration 3X and then the average of those three is the value shown to the user
//8. "Equilibrium reached. CO concentrations measured at Xppm, CO2 concentrations measured at Xppm."
//9. Repeats steps 3-8 (make sure rotometer is closed first) for different flow rates provided
//10. "Close all valves, disconnect box. Sensor box has now been calibrated."

#include "Calibrate.h"

void Calibrate::calibrate() {
  CO co;
  CO2 co2;
  
  int dummy(0);

  Serial.begin(9600);
  Serial.println("Cookstove Sensor CO and CO2 Calibration Process");
  Serial.println("Enter 'yes' and press the ENTER key to begin the calibration process or switch off calibration mode and restart the device to return to measurement mode.");
  while(!Serial.available()) delay(10);
  dummy = Serial.parseFloat();
  delay(1000);

  //Read initial CO and CO2 values
  Serial.println("Enter PPM of CO in the gas tank and press ENTER.");
  while(!Serial.available()) delay(10);
  float coTankPpm = Serial.parseFloat();
  delay(1000);

  Serial.println("Enter PPM of CO2 in the gas tank and press ENTER.");
  while(!Serial.available()) delay(10);
  float co2TankPpm = Serial.parseFloat();
  delay(1000);

  //Read time to reach steady state
  Serial.println("Enter time to reach steady state in seconds."); //This is something we need to specify to them in our manual
  while(!Serial.available()) delay(10);
  int steadyStateTime = 1000*Serial.parseFloat();
  delay(1000);

  float coValues[3];
  float co2Values[3];

  //Sample 1
  float sample1Heights[3];
  Serial.println("Sample 1");
  Serial.println("Turn on tank containing CO and CO2 and enter the height."); //these are instructions that should be provided in the manual
  while(!Serial.available()) delay(10);
  sample1Heights[0] = Serial.parseFloat();
  delay(1000);

  Serial.println("Turn on air tank and record the height of the combined flows after time to reach steady state has passed.");
  delay(steadyStateTime);
  coValues[0] = co.measure();
  co2Values[0] = co2.measure();
  while(!Serial.available()) delay(10);
  sample1Heights[1] = Serial.parseFloat();
  delay(1000);

  Serial.println("Turn off tank containing CO and CO2 and enter the height of the air flow.");
  while(!Serial.available()) delay(10);
  sample1Heights[2] = Serial.parseFloat();
  delay(1000);

  //Sample 2
  float sample2Heights[3];
  Serial.println("Sample 2");
  Serial.println("Turn on tank containing CO and CO2 and enter the height.");
  while(!Serial.available()) delay(10);
  sample2Heights[0] = Serial.parseFloat();
  delay(1000);

  Serial.println("Turn on air tank and record the height of the combined flows after time to reach steady state has passed.");
  delay(steadyStateTime);
  coValues[1] = co.measure();
  co2Values[1] = co2.measure();
  while(!Serial.available()) delay(10);
  sample2Heights[1] = Serial.parseFloat();
  delay(1000);

  Serial.println("Turn off tank containing CO and CO2 and enter the height of the air flow.");
  while(!Serial.available()) delay(10);
  sample2Heights[2] = Serial.parseFloat();
  delay(1000);

  //Sample 3
  float sample3Heights[3];
  Serial.println("Turn on tank containing CO and CO2 and enter the height.");
  while(!Serial.available()) delay(10);
  sample3Heights[0] = Serial.parseFloat();
  delay(1000);

  Serial.println("Turn on air tank and record the height of the combined flows after time to reach steady state has passed.");
  delay(steadyStateTime);
  coValues[2] = co.measure();
  co2Values[2] = co2.measure();
  while(!Serial.available()) delay(10);
  sample3Heights[1] = Serial.parseFloat();
  delay(1000);

  Serial.println("Turn off tank containing CO and CO2 and enter the height of the air flow.");
  while(!Serial.available()) delay(10);
  sample3Heights[2] = Serial.parseFloat();
  delay(1000);

  Serial.println("THESE ARE THE RAW VALUES"); //TODO: these statements are for testing and can be removed
  Serial.print("coValues[0] = ");
  Serial.println(coValues[0]);
  Serial.print("co2Values[0] = ");
  Serial.println(co2Values[0]);
  Serial.print("coValues[1] = ");
  Serial.println(coValues[1]);
  Serial.print("co2Values[1] = ");
  Serial.println(co2Values[1]);
  Serial.print("coValues[2] = ");
  Serial.println(coValues[2]);
  Serial.print("co2Values[2] = ");
  Serial.println(co2Values[2]);
  
  float sample1Flows[3];
  float sample2Flows[3];
  float sample3Flows[3];
  sample1Flows[0] = findFlow(sample1Heights[0]);
  sample1Flows[1] = findFlow(sample1Heights[1]);
  sample1Flows[2] = findFlow(sample1Heights[2]);
  sample2Flows[0] = findFlow(sample2Heights[0]);
  sample2Flows[1] = findFlow(sample2Heights[1]);
  sample2Flows[2] = findFlow(sample2Heights[2]);
  sample3Flows[0] = findFlow(sample3Heights[0]);
  sample3Flows[1] = findFlow(sample3Heights[1]);
  sample3Flows[2] = findFlow(sample3Heights[2]);

  float coConcs[3];
  float co2Concs[3];
  coConcs[0] = calcConc(sample1Flows, coTankPpm);
  co2Concs[0] = calcConc(sample1Flows, co2TankPpm);
  coConcs[1] = calcConc(sample2Flows, coTankPpm);
  co2Concs[1] = calcConc(sample2Flows, co2TankPpm);
  coConcs[2] = calcConc(sample3Flows, coTankPpm);
  co2Concs[2] = calcConc(sample3Flows, co2TankPpm);

  float coSlope = lineRegressSlope(coConcs, coValues);
  float co2Slope = lineRegressSlope(coConcs, coValues);

  float coInter = lineRegressInter(coConcs, coValues);
  float co2Inter = lineRegressInter(coConcs, coValues);

  writeFloat(0, coSlope);
  writeFloat(4, co2Slope);
  writeFloat(8, coInter);
  writeFloat(12, co2Inter);
}

//TODO: Fix me
float Calibrate::findFlow(float(x)) {//This function will find the flow of the gas mixing based off of the paramters in the rotary flow meter.
  // It will perform a linear regression between the flows given on the data sheet for the flow meter to estimate the flows of the actual system.
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

//TODO: Fix me
float Calibrate::calcConc(float flows[3], float(ppm)) {//Calculate composition of CO or CO2 in the gas mix given flows
    //C represents the flow of the stream with CO and CO2, B represents the flow of both stream, A represents the flow of the Air stream, ppm represents the tank concentration of CO or CO2
    float comp = flows[0]/flows[1]*ppm;  //composition calculated in ppm  **I NEED TO RUN A FEW MORE ANALYSIS TO MAKE CERTAIN THIS IS RIGHT EQUATION
    return comp; //return composition
}

//TODO: These functions don't seem to be using the LinearRegression library correctly, investigate further
float Calibrate::lineRegressSlope(float concs[3], float voltages[3]) {
    // C1,C2,C3 are three compositions and V1,V2,V3 are the voltages associated with those compositions
    LinearRegression lr;
    double values[3];
    lr.learn(concs[0],voltages[0]);
    lr.learn(concs[1],voltages[1]);
    lr.learn(concs[2],voltages[2]);

    lr.calculate(3);
    lr.correlation();
    lr.getValues(values);

    return values[0];
}

float Calibrate::lineRegressInter(float concs[3], float voltages[3]) {
    // C1,C2,C3 are three compositions and V1,V2,V3 are the voltages associated with those compositions
    LinearRegression lr;
    double values[3];
    lr.learn(concs[0],voltages[0]);
    lr.learn(concs[1],voltages[1]);
    lr.learn(concs[2],voltages[2]);

    lr.calculate(3);
    lr.correlation();
    lr.getValues(values);

    return values[1];
}

float Calibrate::readFloat(unsigned int addr) {
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

void Calibrate::writeFloat(unsigned int addr, float x) {
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
