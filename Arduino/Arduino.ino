#include <Adafruit_NeoPixel.h>
#include <MPU6050_tockn.h>
#include <Arduino.h>
#include<Wire.h>

//pins:
const int pinLight1 = 0;
const int pinLight2 = 1;
const int pinLight3 = 2;
const int batteryPin = 3;
const int pinNeoPixels = 6;

const int numberOfNeoPixels = 31;
const float lightSensitive = 0.05f;

int index = 0;
int sensorUpdate = 0;
String msgContent = "";

int maxPower = 80;

int lightVoltage1[] = {0,0};
int lightVoltage2[] = {0,0};
int lightVoltage3[] = {0,0};  


MPU6050 mpu6050(Wire);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(numberOfNeoPixels, pinNeoPixels, NEO_GRB + NEO_KHZ800);

void setup() {
 
  pixels.begin();  
  Serial.begin(9600);
  
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets();

  CalibrateLight();

  delay(50);
  SetAllLights(0, 255, 0);
}

void loop() {
  mpu6050.update();

  sensorUpdate ++;

  index ++;
  String json = CreateJson(index);
  Serial.println(json);

  ReadSerial();

  delay(10);
}

String CreateJson(int messageIndex){
  int light1 = analogRead(pinLight1);
  int light2 = analogRead(pinLight2);
  int light3 = analogRead(pinLight3);
  
  String lightValue1 = String(analogRead(pinLight1));;//(light1 < lightVoltage1[0] || light1 > lightVoltage1[1]) ? "false" : "true";//String(analogRead(pinLight1));
  String lightValue2 = String(analogRead(pinLight2));//(light2 < lightVoltage2[0] || light2 > lightVoltage2[1]) ? "false" : "true";//String(analogRead(pinLight2));
  String lightValue3 = String(analogRead(pinLight3));//(light3 < lightVoltage3[0] || light3 > lightVoltage3[1]) ? "false" : "true";//String(analogRead(pinLight3));

  String batteryValue = String(analogRead(batteryPin));

  String tempValue1 = String(mpu6050.getTemp());
  
  return "{" +
    CreateJsonLine("index", String(messageIndex)) + "," +
    CreateJosnArrayLine("gyroscoop", String(mpu6050.getGyroAngleX()) + "," + String(mpu6050.getGyroAngleY()) + "," + String(mpu6050.getGyroAngleZ())) + "," +
    CreateJosnArrayLine("accelerator", String(mpu6050.getAccX()) + "," + String(mpu6050.getAccY()) + "," + String(mpu6050.getAccZ())) + "," +
    CreateJsonLine("temperature", tempValue1) + "," +
    CreateJsonLine("battery", batteryValue) + "," +
    CreateJosnArrayLine("light", lightValue1 + "," + lightValue2 + "," + lightValue3) +
  "}";
}

String CreateJsonLine(String key, String value){
  return "\"" + key + "\":" + value;
}

String CreateJosnArrayLine(String key, String arr){
   return "\"" + key + "\":" + "[" + arr + "]";
}

void CalibrateLight() {
  delay(100);

  int precision = 10;

  lightVoltage1[0] = analogRead(pinLight1);
  lightVoltage2[0] = analogRead(pinLight2);
  lightVoltage3[0] = analogRead(pinLight3);  

  for(int i = 1; i < precision; i++){
    lightVoltage1[0] += analogRead(pinLight1);
    lightVoltage2[0] += analogRead(pinLight2);
    lightVoltage3[0] += analogRead(pinLight3);
    delay(50);
  }

  lightVoltage1[1] = (lightVoltage1[0] / precision) * ( 1 + lightSensitive);
  lightVoltage2[1] = (lightVoltage2[0] / precision) * ( 1 + lightSensitive);
  lightVoltage3[1] = (lightVoltage3[0] / precision) * ( 1 + lightSensitive);

  lightVoltage1[0] = (lightVoltage1[0] / precision) * ( 1 - lightSensitive);
  lightVoltage2[0] = (lightVoltage2[0] / precision) * ( 1 - lightSensitive);
  lightVoltage3[0] = (lightVoltage3[0] / precision) * ( 1 - lightSensitive);
}

void ReadSerial(){
  while (Serial.available() > 0) {
    
    byte character = Serial.read();

    if(character == 49){
      SetAllLights(maxPower, maxPower, 0);
    }

    if(character == 50) {
      SetAllLights(maxPower, 0, maxPower);
    }

    if(character == 51) {
      SetAllLights(0, maxPower, maxPower);
    }

    if(character == 52) {
      SetAllLights(maxPower, 0, 0);
    }

    if(character == 53) {
      SetAllLights(0, maxPower, 0);
    }

    if(character == 54) {
      SetAllLights(0, 0, maxPower);
    }

    if(character == 55) {
      SetAllLights(maxPower, maxPower/2, 0);
    }

    if(character == 56) {
      SetAllLights(maxPower/2, maxPower, maxPower/4);
    }
    
    if(character == 57) {
      SetAllLights(maxPower/2, maxPower/4, maxPower);
    }

    Serial.println(character);
  }
}

void SetAllLights(int r, int g, int b) {
  /*for(int i=0;i<numberOfNeoPixels;i++){
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }*/
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.setPixelColor(2, pixels.Color(r, g, b));
  pixels.setPixelColor(4, pixels.Color(r, g, b));
  pixels.setPixelColor(9, pixels.Color(r, g, b));
  pixels.setPixelColor(11, pixels.Color(r, g, b));
  pixels.setPixelColor(16, pixels.Color(r, g, b));
  pixels.setPixelColor(18, pixels.Color(r, g, b));
  
  pixels.show();
}

//light animations
void CalibrateWarningAnimation(){
  for(int amount = 0; amount < 3; amount++){
    for(int i = 0; i < maxPower; i++){
      SetAllLights(i, 0, 0);
      delay(5);
    }
  
    for(int i = maxPower; i > 0; i--){
      SetAllLights(i, 0, 0);
      delay(5);
    }
  }
  for(int i = 0; i < maxPower; i++){
     SetAllLights(i, 0, 0);
     delay(5);
  }
}


