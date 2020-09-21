#include <LinearRegression.h>

LinearRegression lr = LinearRegression(0,100);

double values[3];
float C_values[3];
float CO_values[3];
float CO2_values[3];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

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

void loop() {
  // put your main code here, to run repeatedly:

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
