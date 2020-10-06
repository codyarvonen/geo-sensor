//Code for Everything
//Creator: Joseph Seamons
//Start Date: 3/7/19

///////////////////////////////////////////////////////////Libraries to include/////////////////////////////////////////////////////////////////////////////
#include <Wire.h>
#include <math.h> 
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <Adafruit_ADS1015.h> //NEED for CO sensor
#include <DS3231.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////Declare Variables////////////////////////////////////////////////////////////////////////////////////

//Declare serial communication for CO2 sensor
SoftwareSerial mySerial(12, 13); // RX, TX pins on Ardunio for CO2 sensor Communication

//Variables for CO2 Sensors
int store1 [3];
int i=0; //Counter
double value = 0;

int co2 =0;
double multiplier = 12.077; // 1 for 2% =20000 PPM, 10 for 20% = 200,000 PPM
uint8_t buffer[25];
uint8_t ind =0;
uint8_t index =0;

//Variables for PM Sensor
int incomingByte = 0;
int PM10 = 0; //PM of 10 microns or greater
int PM2_5 = 0; //PM of 2.5 microns or greater
int count = 0; //Count to know how much data coming in
int count1 = 0; //Set out counter for array
int count2 = 0;
int count3 = 0;
double pm25 = 0;
double pm10 = 0;
int big = 0;
int data[101];  //Used with PM sensor
int store[11];  //Used with PM sensor


//Period of wait for delay at end:
int period = 30000; //This will delay for 7.5 seconds.
unsigned long time_now = 0; //Time since it started


//Variables for CO Sensor
Adafruit_ADS1115 ads(0x48);  // construct an ads1115 at address 0x48 (CONFIGURATION)
float CO = 0.0;
double rawCO = 0.00;

//Variables for Clock
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
int z = 0;

//Declare files
File myFileCO2;
File myFilePM;
File myFileCO;
File myFileall;


//FUNCTIONS ARE INITALIZED 
int fill_buffer();  
int format_output();
int card_write();


//Start everything up for potentiometer:
int sensorPin = 0;    // The potentiometer is connected to
                      // analog pin 0
int sensorValue;

int sensorValueF;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////Initialize Everything////////////////////////////////////////////////////////////////////////////
void setup() {
  /////////////////////////////////////////////////////////////////////////Set-up for PM Sensor///////////////////////////////////////////////////////////
  Wire.begin();// join i2c bus
  //Serial.begin(9600); //Communication rate for the Arduino to write to serial monitor
  Serial1.begin(9600); //Communication to communicate with PM Sensor
 // while (!Serial)
 // {
 //   ; // wait for serial port to connect. Needed for native USB port only
 // } 
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////Set-up for CO2 Sensor///////////////////////////////////////////////////////
  mySerial.begin(9600); // Start serial communications with sensor
  //mySerial.println("K 0");  // Set Command mode
  mySerial.println("M 6"); // send Mode for Z and z outputs
  // "Z xxxxx z xxxxx" (CO2 filtered and unfiltered)
  mySerial.println("K 1");  // set streaming mode
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////Set-up for CO Sensor//////////////////////////////////////////////////////////////
  //ADC converter initalizing //NEED
  ads.begin();  // Initialize ads1115 

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  


  
  ///////////////////////////////////////////////////////////////////////////////Start initialization:///////////////////////////////////////////////////
 // Serial.println("Sensor Apperatus Start:");
  //Serial.println(" ");
  //Serial.println(" ");
 // Serial.println(" ");
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  
   ////////////////////////////////////////////////////////////////////////////Set-up the card reader://////////////////////////////////////////////////
  // Serial.print("Initializing SD card...");

    if (!SD.begin(53)) {
   //Serial.println("initialization failed!");
    while (1);
    }
   //Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////////Write Headings to SD Card//////////////////////////////////////
  myFileall = SD.open("AllData.csv", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFileall) {
   

    myFileall.print("Date,");
    myFileall.print("       ");

    myFileall.print("Time,");
    myFileall.print("        ");

    myFileall.print("Filt CO2 PPM,");
    myFileall.print("      ");

    myFileall.print("2.5 Micron PM,");
    myFileall.print("      ");

    myFileall.print("10 Micron PM,");
    myFileall.print("      ");

    myFileall.print("Raw CO,");
    myFileall.print("      ");

    myFileall.print("CO PPM,");
    myFileall.print("      ");

    myFileall.println("      ");

    
    // close the file:
    myFileall.close();
   Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
  }


  /////////////////////////////////////////////////////////////////////////////////////Set Up Clock/////////////////////////////////////////////////////////
  // Initialize the rtc object
  rtc.begin();

  /////////////////////////////////////////////////////////////////////////////////Set Up LED///////////////////////////////////////////////////////////////
  pinMode(7, OUTPUT);

}

  /////////////////////////////////////////////////////////////////////////////////////Run the Loop////////////////////////////////////////////////////////

void loop() {
  ////////////////////////////////////////////////////////////////////////Get CO2 Values,process, and print///////////////////////////////////////////////
  fill_buffer();  // function call that reads CO2 sensor and fills buffer    
  //Serial.print("Buffer contains: ");
  for(int j=0; j<ind; j++)//Serial.print(buffer[j],HEX);
  index = 0;
  format_output();

  i = i + 1;
 
  index = 8;  // In ASCII buffer, filtered value is offset from raw by 8 bytes
  format_output();
  
  i = 0;
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////Get CO values and print to card//////////////////////////////////////////////////
  //CO reading in Code NEED!
  int16_t adc1;  // we read from the ADC, we have a sixteen bit integer as a result
  int16_t adc2;
  
  // Reading in from CO Sensor 
  adc1 = ads.readADC_SingleEnded(0);
  adc2 = ads.readADC_SingleEnded(1);
  CO = ((((adc1-adc2) * (.0317)) - 16.8828)-520)+480;
  rawCO = adc1-adc2;
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////Get PM Values, process, and print///////////////////////////////////////////////
  // Start count
  count = 0;
  
  while (count < 100)
  {
    //Collect available data:
    incomingByte = Serial1.read();
   //Fill up array
   data[count] = incomingByte; //Read incoming data into array
   //Increase Count
   count = count + 1; //Increase count
   }

   //Store data in vector

    //Reset count1:
    count1 = 0;

    //Write array to the serial monitor:
    while (count1 < 100)
    {
      //Increase count:
      count1 = count1 + 1; //Increase count to get next data in array
    }
    
  //Reset count2 and count3:
    count2 = 0;
    count3 = 0;
    big = 0;
  ///////////////////////////////////////////////////////////////////////Get needed elements from the matrix///////////////////////////////////////////////////////////////
  while(count2 < 100 && count3 < 10)
  {

    if(data[count2] != 170)
    {
      count2 = count2 + 1;
    }
    else
    {
      while(big<11)
      {
        
        store[count3] = data[count2 + big];
        
        big = big +1;
        count3 = count3 + 1;
      }
    }
  }

  //Perform math on the data:
  //Remember:   PM2.5 low bit-3rd element in vector, PM2.5 high bit-4th, PM10 low bit-5th, PM10 high bit-6th

  pm25 = ((store[3]*256) + store[2])/10.00; //These conversions to pm values come from the data sheet for SDS011 PM sensor page 6.
  pm10 =  ((store[5]*256) + store[4])/10.00;
  ///////////////////////////////////////////////////////////////////////////////////////Write all Data to SD Card/////////////////////////////////////////////////////////////////////
  
  myFileall = SD.open("AllData.csv", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFileall) {
    
    //Turn on write indicator (LED)
    digitalWrite(7, HIGH);
    myFileall.print(rtc.getDateStr());  //Date
    myFileall.print(",      ");

    myFileall.print(rtc.getTimeStr());  //Time
    myFileall.print(",      ");

    myFileall.print(store1[1]);  //Filtered CO2
    myFileall.print(",      ");

    myFileall.print(pm25);    //2.5 Micron PPM
    myFileall.print(",      ");

    myFileall.print(pm10);    //10 Micron PPM
    myFileall.print(",      ");

    myFileall.print(rawCO);   //Raw CO
    myFileall.print(",      ");

    myFileall.print(CO);    //CO PPM
    myFileall.print(",      ");

    myFileall.println("      ");

    
    // close the file:
    myFileall.close();
  // Serial.println("done.");

   //Turn off write indicator (LED)
   digitalWrite(7, LOW);

   
  } else {
    // if the file didn't open, print an error:
   //Serial.println("error opening test.txt");
  }

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 //////////////////////////////////////////////////////////Get potentiometer data///////////////////////////////////////////////////////////////////
 //sensorValue = analogRead(sensorPin);  //Reading potentiometer value in Volts


 //sensorValueF = sensorValue*10;

 //period = sensorValueF;
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 //////////////////////////////////////////////////////////////Delay according to potentiometer///////////////////////////////////////////////////////////////////////
 //delay(1000);

 //Delay using millis

 time_now = millis();


for(z = 0; z<4; z++)  //With z=4, loop lasts about 30 seconds.
 while(millis() < time_now + period){
        //wait approx. [period] ms
  }


} // end of loop


 ///////////////////////////////////////////////////////////////////////////Define Functions//////////////////////////////////////////////////////////


 /////////////////////////////////////////////////////////Fill Buffer Function////////////////////////////////////////////////
 int fill_buffer(void){
  // Fill buffer with sensor ascii data
 //Serial.println("in loopb");
  
 ind = 0;
 while(buffer[ind-1] != 0x0A){  // Read sensor and fill buffer up to 0XA = CR
 
  //Serial.println("in loopbb");
  if(mySerial.available()){
    
    buffer[ind] = mySerial.read();
    ind++;
    } 
  }
  // buffer() now filled with sensor ascii data
  // ind contains the number of characters loaded into buffer up to 0xA =  CR
  ind = ind -2; // decrement buffer to exactly match last numerical character
  }
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  


 ////////////////////////////////////////////////////////Format Output Function//////////////////////////////////////////////
 int format_output(void){ // read buffer, extract 6 ASCII chars, convert to PPM and print
  co2 = buffer[15-index]-0x30;
  co2 = co2+((buffer[14-index]-0x30)*10);
  co2 +=(buffer[13-index]-0x30)*100;
  co2 +=(buffer[12-index]-0x30)*1000;
  co2 +=(buffer[11-index]-0x30)*10000;
  
  value = ((co2*multiplier)-457.4437);
 
  store1[i] = value;
  

  delay(200);
 }
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
