#include <Arduino.h>

const int buttonPin = 34;     // 버튼 핀
const int ledPin = 2;       // LED 핀

bool ledState = LOW;         // 현재 LED 상태
bool lastButtonState = LOW;  // 이전 버튼 상태
bool buttonState = LOW;      // 현재 버튼 상태

unsigned long lastDebounceTime = 0;  // 마지막 디바운싱 시간
const unsigned long debounceDelay = 50;  // 디바운스 딜레이 (ms)

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
}

void loop() {
  int reading = digitalRead(buttonPin); // 현재 버튼 상태 읽기

  // 버튼 상태가 이전과 다를 경우 디바운스 시간 기록
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // 디바운스 시간 경과 후 안정된 입력이면 버튼 상태 갱신
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // 버튼 눌림 상태에서만 LED 토글
      if (buttonState == LOW) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
      }
    }
  }

  lastButtonState = reading;
}

