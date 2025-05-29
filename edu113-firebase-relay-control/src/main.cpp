//ESP32 firebase Relay control
#include <Arduino.h> 
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Firebase 데이터베이스 경로 생성에 필요한 헬퍼 함수 포함
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// WiFi 자격 증명
const char* ssid = "copaland";
const char* password = "24868080";

// Firebase 프로젝트 웹 API 키 및 데이터베이스 URL 입력
#define API_KEY "AIzaSyDYnZAOHlTLbKf8sMqEw8cvwYTkVHNEVaY"
#define DATABASE_URL "https://esp32-iot-8923e-default-rtdb.firebaseio.com/"

// 핀 정의
const int inputPins[] = {16, 17, 18, 19}; // 스위치 입력 (Pulldown HIGH active)
const int relayPins[] = {26, 27, 32, 33}; // 릴레이 출력 (Source Output)
const int numChannels = 4; // 채널 수

// 디바운스 변수
int switchStates[4] = {LOW, LOW, LOW, LOW};      // 현재 스위치 상태
int lastSwitchStates[4] = {LOW, LOW, LOW, LOW};  // 이전 스위치 상태
unsigned long lastDebounceTime[4] = {0, 0, 0, 0}; // 디바운스 타이머
const unsigned long debounceDelay = 50;    // 디바운스 시간 (밀리초)

// 마지막 Firebase 읽기 시간
unsigned long lastFirebaseReadTime = 0;
const unsigned long firebaseReadInterval = 1000; // Firebase 읽기 간격 (밀리초)

// Firebase 데이터 객체 정의
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// 타이머 변수
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

// 함수 프로토타입 선언 추가
void initFirebaseData();

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP32 Firebase 릴레이 제어 시작");
  
  // 핀 설정
  for (int i = 0; i < numChannels; i++) {
    pinMode(inputPins[i], INPUT_PULLDOWN); // 풀다운 입력으로 설정
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);       // 릴레이 초기 상태: OFF
    Serial.printf("핀 설정 - 입력: %d, 출력: %d\n", inputPins[i], relayPins[i]);
  }

  // WiFi 연결
  WiFi.begin(ssid, password);
  Serial.print("WiFi 연결 중");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP 주소로 연결됨: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Firebase 구성
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Firebase에 익명으로 로그인
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase 로그인 성공");
    signupOK = true;
  } else {
    Serial.printf("Firebase 로그인 실패: %s\n", config.signer.signupError.message.c_str());
  }

  // 토큰 상태에 대한 콜백 할당
  config.token_status_callback = tokenStatusCallback;
  
  // Firebase 시작
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // Firebase 데이터베이스 초기화
  if (signupOK) {
    initFirebaseData();
  }
}

// Firebase 데이터 초기화
void initFirebaseData() {
  for (int i = 0; i < numChannels; i++) {
    // 스위치 경로 및 초기 상태 설정
    String switchPath = "/switches/switch" + String(i + 1);
    if (Firebase.RTDB.setBool(&fbdo, switchPath.c_str(), false)) {
      Serial.println("스위치 " + String(i + 1) + " 상태 초기화 성공");
    } else {
      Serial.println("스위치 " + String(i + 1) + " 상태 초기화 실패: " + fbdo.errorReason());
    }
    
    // 릴레이 경로 및 초기 상태 설정
    String relayPath = "/relays/relay" + String(i + 1);
    if (Firebase.RTDB.setBool(&fbdo, relayPath.c_str(), false)) {
      Serial.println("릴레이 " + String(i + 1) + " 상태 초기화 성공");
    } else {
      Serial.println("릴레이 " + String(i + 1) + " 상태 초기화 실패: " + fbdo.errorReason());
    }
  }
}

void loop() {
  // WiFi 연결 확인
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi 연결이 끊겼습니다. 재연결 중...");
    WiFi.reconnect();
    delay(1000);
    return;
  }
  
  // Firebase 준비 확인
  if (Firebase.isTokenExpired()) {
    Firebase.refreshToken(&config);
    Serial.println("Firebase 토큰 새로 고침");
  }
  
  unsigned long currentMillis = millis();

  // 1. 물리적 스위치 입력 처리 (토글 방식)
  for (int i = 0; i < numChannels; i++) {
    int reading = digitalRead(inputPins[i]);

    // 스위치 상태가 변경되면 디바운스 타이머 재설정
    if (reading != lastSwitchStates[i]) {
      lastDebounceTime[i] = currentMillis;
      lastSwitchStates[i] = reading;
    }

    // 디바운스 시간이 지난 후 스위치 상태 확인
    if ((currentMillis - lastDebounceTime[i]) > debounceDelay) {
      // 스위치가 눌림(HIGH) → OFF에서 ON으로 변할 때만 토글
      if (reading == HIGH && switchStates[i] == LOW) {
        // 릴레이 상태 토글
        bool newRelayState = !digitalRead(relayPins[i]);
        digitalWrite(relayPins[i], newRelayState ? HIGH : LOW);

        // Firebase에 릴레이 상태 업데이트
        if (signupOK) {
          String relayPath = "/relays/relay" + String(i + 1);
          if (Firebase.RTDB.setBool(&fbdo, relayPath.c_str(), newRelayState)) {
            Serial.printf("릴레이 %d 토글: %s (Firebase 동기화)\n", i + 1, newRelayState ? "ON" : "OFF");
          } else {
            Serial.println("Firebase 릴레이 토글 실패: " + fbdo.errorReason());
          }
        }
      }
      switchStates[i] = reading;
    }
  }

  // 2. Firebase에서 릴레이 상태 읽기 (주기적으로)
  if (currentMillis - lastFirebaseReadTime > firebaseReadInterval) {
    lastFirebaseReadTime = currentMillis;

    if (signupOK) {
      for (int i = 0; i < numChannels; i++) {
        String path = "/relays/relay" + String(i + 1);

        if (Firebase.RTDB.getBool(&fbdo, path.c_str())) {
          bool relayValue = fbdo.to<bool>();
          digitalWrite(relayPins[i], relayValue ? HIGH : LOW);
          Serial.printf("릴레이 %d 상태 읽기: %s\n", i + 1, relayValue ? "ON" : "OFF");
        } else {
          Serial.println("Firebase 읽기 실패: " + fbdo.errorReason());
        }
      }
    }
  }

  delay(10);
}
