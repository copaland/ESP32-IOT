#include <Arduino.h>

const int buttonPin = 34;  // 푸시버튼 연결 핀
const int ledPin = 2;      // LED 연결 핀

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);  // 내부 풀업 저항 활성화
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 버튼 상태 읽기 (풀업 저항으로 인해 눌렸을 때 LOW)
  int buttonState = digitalRead(buttonPin);

  // 버튼 상태에 따라 LED 제어
  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH);  // 버튼이 눌렸을 때 LED 켜기
    Serial.println("버튼 눌림 - LED ON");
  } else {
    digitalWrite(ledPin, LOW);   // 버튼이 떼어졌을 때 LED 끄기
    Serial.println("버튼 떼어짐 - LED OFF");
  }

  delay(100);  // 짧은 지연으로 디바운싱 효과 부분 적용
}
