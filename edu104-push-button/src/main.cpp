#include <Arduino.h>
#define BUTTON_PIN 9  // 푸시 버튼 핀
#define LED_PIN 38    // LED 핀

bool ledState = false; // LED 상태 변수

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // 버튼 핀을 내부 풀업 저항으로 설정
  pinMode(LED_PIN, OUTPUT);           // LED 핀을 출력으로 설정
}

void loop() {
  static bool lastButtonState = HIGH; // 이전 버튼 상태
  bool currentButtonState = digitalRead(BUTTON_PIN); // 현재 버튼 상태 읽기

  // 버튼이 눌렸을 때 상태 변경
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    ledState = !ledState; // LED 상태 토글
    digitalWrite(LED_PIN, ledState); // LED 상태에 따라 핀 출력
    //digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    delay(50); // 디바운싱을 위한 지연
  }

  lastButtonState = currentButtonState; // 이전 상태 업데이트
}
