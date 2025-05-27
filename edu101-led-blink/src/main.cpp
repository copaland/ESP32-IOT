#include <Arduino.h>
//ESP32-S3 LED BLINK
// ESP32-S3 LED BLINK (Standard Arduino Code)
// 38: LED2 (빨강)
// 47: LED1 (파랑)
const int ledRed = 38; //LED2 (빨강)
const int ledBlue = 47; //LED1 (파랑)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32-S3!");
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);
}

void loop() {
  // 빨간색 LED 켜고, 파란색 LED 끄기
  digitalWrite(ledRed, HIGH);   // HIGH는 켜짐
  digitalWrite(ledBlue, LOW);   // LOW는 꺼짐
  Serial.println("Red LED ON, Blue LED OFF");
  delay(1000); // 1초(1000밀리초) 대기

  // 빨간색 LED 끄고, 파란색 LED 켜기
  digitalWrite(ledRed, LOW);    // LOW는 꺼짐
  digitalWrite(ledBlue, HIGH);  // HIGH는 켜짐
  Serial.println("Red LED OFF, Blue LED ON");
  delay(1000); // 1초(1000밀리초) 대기
}
