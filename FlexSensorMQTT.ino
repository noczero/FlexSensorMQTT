/**
Bismillah
---------
Test Flex Sensor
- Wemos D1 Mini
- Resistor 37K between A0 and Ground
*/

#define flexPIN A0

const float VCC = 4.98; // Wemos D1 Voltage
const float Resistor_Divider = 36500.0; // ohm resistance
const float STRAIGHT_RES = 11555.99; // got from flexR when flex is straight
const float BEND_RES = 60234.45; // got from flexR when flex is bend

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("initialised");
  pinMode(flexPIN , INPUT);
}

int flexADC = 0;
float flexV = 0.0 , flexR = 0.0 , angle = 0.0;

float readingFlex(){
  flexADC = analogRead(flexPIN);
  flexV = flexADC * VCC / 1023.0;
  flexR = Resistor_Divider * (VCC / flexV - 1.0);
  //Serial.println("Flex Res : " + String(flexR));
  // map value to degrees
  angle = map(flexR, STRAIGHT_RES , BEND_RES,0 , 90.0);

  return abs(angle);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Degree - " + String(readingFlex()));
  delay(100);
}
