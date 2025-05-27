#include <Arduino.h>

const int potPin = 1; // POT가 연결된 아날로그 핀 (A0)
const int ledPin = 38; // LED가 연결된 PWM 핀
const int freq = 5000; // PWM 주파수
const int resolution = 8; // PWM 해상도 (8비트)
const int ledChannel = 0; // PWM 채널

void setup() {
    Serial.begin(115200); // 시리얼 모니터 시작
    ledcSetup(ledChannel, freq, resolution); // PWM 채널 설정
    ledcAttachPin(ledPin, ledChannel); // 핀을 PWM 채널에 연결
}

void loop() {
    int potValue = analogRead(potPin); // POT 값 읽기 (0-4095)
    int pwmValue = map(potValue, 0, 4095, 0, 255); // 0-255 범위로 매핑
    ledcWrite(ledChannel, pwmValue); // PWM 신호로 LED 밝기 조절
    delay(100); // 100ms 대기
}
