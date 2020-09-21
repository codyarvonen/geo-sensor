const int temperaturePin = 0;
const int LEDpin = 2;

void setup()
{
  Serial.begin(9600);
}


void loop()
{
  /*
  float voltage, degreesC;
  //digitalWrite(LEDpin, HIGH);
  voltage = analogRead(temperaturePin) * 5.0/1024.0;
  degreesC = (voltage - 0.4) * 10.0;
  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print("  deg C: ");
  Serial.print(degreesC);
  Serial.println();
  */
  digitalWrite(LEDpin, LOW);
  delay(1000); // repeat once per second (change as you wish!)
  digitalWrite(LEDpin, HIGH);
  delay(1000); // repeat once per second (change as you wish!)
}
