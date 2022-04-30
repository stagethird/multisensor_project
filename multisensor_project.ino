#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <Adafruit_LIS3DH.h>

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

// Create the LIS3DH lis object
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

const byte buzz = 2;
const byte LEDgrn = 3;
const byte LEDylo = 4;
const byte LEDred = 5;
const byte LEDblu = 6;
const byte motionPin = 7;
const float tempThreshold = 80.0;

float f;
float x;
float y;
float z;

float x_avg;
float y_avg;
float z_avg;

float x_adj;
float y_adj;
float z_adj;

bool accelTriggered = false;
bool tempTriggered = false;
bool motionTriggered = false;
unsigned long startMillis = 0;
int accelTotal = 0;
int motionTotal = 0;

void setup() {
  Serial.begin(115200);
  
  if (!lis.begin(0x19)) {
    Serial.println("Couldnt find LIS3DH! Check your connections and verify the address is correct.");
    while (1);
  }

  if (!tempsensor.begin(0x1A)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    while (1);
  }
  
  tempsensor.setResolution(2); // Range 0-3

  // Initalize and zero digital I/Os
  for(int pinNumber = 2; pinNumber < 7; pinNumber++) {
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, LOW);
  }
  pinMode(motionPin, INPUT);
  
  /*  The loop below 'normalizes' the LIS3DH outputs,
      storing the average bias in the '_avg' vars. */
   
  for(int i=1; i<=30;i++) {
    sensors_event_t event;
    lis.getEvent(&event);
    
    x=event.acceleration.x;
    y=event.acceleration.y;
    z=event.acceleration.z;

    x_avg=(x_avg+x)/2;
    y_avg=(y_avg+y)/2;
    z_avg=(z_avg+z)/2;

    delay(50);
  }
}

void loop() {
  if (accelTest()) {
    digitalWrite(LEDred, HIGH);
    if (!accelTriggered) beep(2);
    logSensorData(1);
  }
  
  if (tempSensorTest()) {
    digitalWrite(LEDblu, HIGH);
    logSensorData(2);
  }
  else digitalWrite(LEDblu, LOW);
  
  if (digitalRead(motionPin) == HIGH) {
    digitalWrite(LEDgrn, HIGH);
    if (!motionTriggered) beep(1);
    logSensorData(3);
  }
  
  if (!accelTest() && !tempSensorTest() && digitalRead(motionPin) == LOW) logSensorData(0);
}
 
bool accelTest() {
  /* Get a new sensor event and normalize or 'zero'
   * using the '_avg' values gathered during setup */
  
  sensors_event_t event;
  lis.getEvent(&event);

  x=event.acceleration.x;
  y=event.acceleration.y;
  z=event.acceleration.z;

  x_adj = x-x_avg;
  y_adj = y-y_avg;
  z_adj = z-z_avg;

  // If values indicate movement, return true
  if (abs(x_adj)>1.0 || abs(y_adj)>1.0 || abs(z_adj)>1.5) return true;
  return false;
}

bool tempSensorTest() {
  /* Get a temp sensor measurement and compare to max 
   * value 'tempThreshold'. The '- 0.02' is an offset to
   * keep the sensor return from "bouncing" when at the threshold */
   
  f = tempsensor.readTempF();
  if (f >= tempThreshold) return true;
  else if (f <= (tempThreshold - 0.2)) return false;
  else {
    // Is temp LED already high? If yes, stay there
    if (digitalRead(LEDblu) == HIGH) return true;
    else return false;
  }
}

void logSensorData(byte dataIn) {
  if (dataIn == 1) accelTriggered = true;
  if (dataIn == 2) tempTriggered = true;
  if (dataIn == 3) motionTriggered = true;
  if (startMillis == 0) startMillis = millis();
  
  if (millis() > (startMillis + 5000)) {
    // cycle at 5 second intervals
    Serial.println("start");
    if (accelTriggered == true) {
      Serial.println("accelTriggered=True");
      accelTotal += 1;
    } 
    else Serial.println("accelTriggered=False");
    Serial.print("accelTotal=");Serial.println(accelTotal);
    
    if (tempTriggered == true) {
      Serial.println("tempTriggered=True");
    }
    else Serial.println("tempTriggered=False");
    
    f = tempsensor.readTempF();
    Serial.print("Temp="); Serial.println(f);
    
    if (motionTriggered == true) {
      Serial.println("motionTriggered=True");
      motionTotal += 1;
    }
    else Serial.println("motionTriggered=False");
    Serial.print("motionTotal=");Serial.println(motionTotal);
    Serial.println("end");
    
    accelTriggered = false;
    tempTriggered = false;
    motionTriggered = false;
    startMillis = 0;
  }
}

void beep(byte dataIn) {
  // Single beep
  if (dataIn == 1) {
    digitalWrite(buzz, HIGH);
    delay(250);
    digitalWrite(buzz, LOW);
    delay(100);
  }
    
  else if (dataIn == 2) {
    // 3 Short beeps
    for(int i = 1; i < 4; i++) {
      digitalWrite(buzz, HIGH);
      delay(50);
      digitalWrite(buzz, LOW);
      delay(100);
    }
  }
}
