#include <Arduino.h>

const int buttonPin = 34;  // 버튼 입력 핀
const int ledPin = 2;      // LED 출력 핀

enum ButtonState {
  RELEASED,
  PRESSED,
  DEBOUNCING_PRESS,
  DEBOUNCING_RELEASE
};

ButtonState buttonState = RELEASED;
unsigned long debounceStartTime = 0;
const unsigned long debounceDelay = 50;

bool ledState = false;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  Serial.begin(115200);
}

void loop() {
  int reading = digitalRead(buttonPin);
  unsigned long currentTime = millis();

  switch (buttonState) {
    case RELEASED:
      if (reading == LOW) {
        debounceStartTime = currentTime;
        buttonState = DEBOUNCING_PRESS;
      }
      break;

    case PRESSED:
      if (reading == HIGH) {
        debounceStartTime = currentTime;
        buttonState = DEBOUNCING_RELEASE;
      }
      break;

    case DEBOUNCING_PRESS:
      if (currentTime - debounceStartTime >= debounceDelay) {
        if (reading == LOW) {
          buttonState = PRESSED;
          Serial.println("버튼 눌림");

          // LED 상태 토글
          ledState = !ledState;
          digitalWrite(ledPin, ledState);
        } else {
          buttonState = RELEASED;
        }
      }
      break;

    case DEBOUNCING_RELEASE:
      if (currentTime - debounceStartTime >= debounceDelay) {
        if (reading == HIGH) {
          buttonState = RELEASED;
          Serial.println("버튼 떼어짐");
        } else {
          buttonState = PRESSED;
        }
      }
      break;
  }
}