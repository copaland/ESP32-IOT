#include <Arduino.h>

const int ledPin = 2;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(ledPin, OUTPUT); 
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(1000); 
  digitalWrite(ledPin, LOW);
  delay(1000); 
}
