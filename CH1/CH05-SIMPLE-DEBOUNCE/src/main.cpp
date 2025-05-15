#include <Arduino.h>

const int buttonPin = 34;  // 버튼 입력 핀
const int ledPin = 2;      // LED 출력 핀

// 변수 선언
int currentKey = 0;
int oldKey = 0;

// 타이밍 변수
unsigned long previousMillis = 0; // 마지막으로 함수를 호출한 시간
const long interval = 10;          // 함수를 호출할 간격 (10 ms)

// 버튼 상태를 확인하는 함수
void checkButtonState() {
    currentKey = ![digitalRead](buttonPin); // 버튼 상태 읽기

    // 버튼 상태가 변경되었고 현재 눌린 경우
    if ((currentKey != oldKey) && currentKey) {
        digitalWrite(ledPin, ![digitalRead](ledPin)); // LED 토글
    }

    oldKey = currentKey; // 이전 키 상태 업데이트
}

void setup() {
    pinMode(buttonPin, INPUT); // 버튼 핀을 입력으로 설정
    pinMode(ledPin, OUTPUT);    // LED 핀을 출력으로 설정
}

// 메인 루프
void loop() {
    unsigned long currentMillis = millis(); // 현재 시간 가져오기

    // 마지막 호출 이후 10ms가 지났는지 확인
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; // 마지막 호출 시간 업데이트
        checkButtonState(); // 버튼 상태 확인 함수 호출
    }

    // 다른 비블로킹 작업을 여기에 추가할 수 있습니다.
}