#include <Arduino.h>

// 버튼 누르면 부저 소리
#include <Arduino.h>

const int buzzerPin = 11;  // the buzzer pin
const int buttonPin = 9;  // Porta SW1

void test_buzzer()
{
  tone(buzzerPin, 262);//Flat DO played for 500ms
  delay(500);
  tone(buzzerPin, 294);//Flat Re played for 500ms
  delay(500);
  tone(buzzerPin, 330);//Flat Mi played for 500ms
  delay(500);
  tone(buzzerPin, 349);//Flat Fa played for 500ms
  delay(500);
  tone(buzzerPin, 392);//Flat So played for 500ms
  delay(500);
  tone(buzzerPin, 440);//Flat La played for 500ms
  delay(500);
  tone(buzzerPin, 494);//Flat Si played for 500ms
  delay(500);
  noTone(buzzerPin);//stop for 1s
  delay(1000);
}

void setup() {
pinMode(buzzerPin, OUTPUT);
pinMode(buttonPin, INPUT);
}

void loop() {
    while (true) // 
    {
        if (digitalRead(buttonPin) == HIGH)
        {
            break; // 
        }
        test_buzzer(); // 
    }
}
