#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// 설정 상수
#define DEBUG true                                           // 디버깅 플래그
#define API_KEY "AIzaSyDYnZAOHlTLbKf8sMqEw8cvwYTkVHNEVaY"   // Firebase API 키
#define DATABASE_URL "https://esp32-iot-8923e-default-rtdb.firebaseio.com/"
#define FIREBASE_LED_PATH "/leds/led"                        // Firebase 경로
#define WIFI_RETRY_COUNT 20                                  // WiFi 연결 재시도 횟수
#define FIREBASE_CHECK_INTERVAL 2000                         // Firebase 상태 확인 간격 (ms)
#define WIFI_CHECK_INTERVAL 30000                            // WiFi 연결 확인 간격 (ms)
#define DEBOUNCE_DELAY 50                                    // 버튼 디바운스 지연 (ms)

// WiFi 설정
const char* ssid = "copaland";
const char* password = "24868080";

// 핀 정의
const int buttonPins[] = {8, 9};                            // 버튼 입력 핀
const int relayPins[]  = {47, 38};                          // 릴레이/LED 출력 핀
const int numChannels = sizeof(buttonPins) / sizeof(buttonPins[0]);  // 채널 수 자동 계산

// 상태 변수
int lastButtonState[numChannels] = {HIGH, HIGH};
int relayState[numChannels] = {LOW, LOW};
unsigned long lastFirebaseCheck = 0;
unsigned long lastWifiCheck = 0;

// Firebase 객체
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

// 웹서버
WebServer server(80);

// 함수 선언
void setupWifi();
void setupFirebase();
void setupWebServer();
void updateRelayState(int channel, bool newState, bool isToggle = false);
void handleData();
void handleRoot();
void handleRelayToggle();
void handleGetStates();
void syncWithFirebase();
void checkPhysicalButtons();
String getFirebasePath(int channel);
void debugPrint(const String &message);
void handleFileRead(String path);  // 이 줄 추가

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n===== ESP32 Firebase LED 웹 컨트롤 시작 =====");
  
  // 핀 설정
  for (int i = 0; i < numChannels; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], relayState[i]);
    Serial.printf("채널 %d 설정: 버튼 핀=%d, LED 핀=%d\n", i+1, buttonPins[i], relayPins[i]);
  }
  
  // SPIFFS 초기화
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS 초기화 실패");
    while (true) delay(100);
  }
  Serial.println("SPIFFS 초기화 완료");
  
  // SPIFFS 파일 목록 (개발 중에만 필요)
  #if DEBUG
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    Serial.println("SPIFFS 파일 목록:");
    while(file) {
      Serial.printf("- %s (%d bytes)\n", file.name(), file.size());
      file = root.openNextFile();
    }
  #endif
  
  setupWifi();
  setupFirebase();
  setupWebServer();
}

void loop() {
  server.handleClient();
  checkPhysicalButtons();
  
  // Firebase에서 상태 동기화 (주기적)
  unsigned long currentMillis = millis();
  if (currentMillis - lastFirebaseCheck > FIREBASE_CHECK_INTERVAL) {
    lastFirebaseCheck = currentMillis;
    syncWithFirebase();
  }

  // WiFi 연결 모니터링 (주기적)
  if (currentMillis - lastWifiCheck > WIFI_CHECK_INTERVAL) {
    lastWifiCheck = currentMillis;
    if (WiFi.status() != WL_CONNECTED) {
      debugPrint("WiFi 연결 끊김 - 재연결 시도");
      WiFi.reconnect();
    }
  }
}

// WiFi 설정
void setupWifi() {
  Serial.printf("WiFi(%s) 연결 중", ssid);
  
  WiFi.begin(ssid, password);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < WIFI_RETRY_COUNT) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println(" 연결 성공!");
    Serial.println("\n============ 접속 정보 ============");
    Serial.printf("웹 서버 주소: http://%s\n", WiFi.localIP().toString().c_str());
    Serial.println("==================================\n");
  } else {
    Serial.println(" 연결 실패! AP 모드로 전환 필요");
  }
}

// Firebase 설정
void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase 로그인 성공");
    signupOK = true;
    
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // Firebase에서 초기 상태 로드
    for (int i = 0; i < numChannels; i++) {
      String path = getFirebasePath(i);
      
      if (Firebase.RTDB.getInt(&fbdo, path.c_str())) {
        relayState[i] = fbdo.to<int>();
        Serial.printf("채널 %d 초기 상태: %d\n", i+1, relayState[i]);
      } else {
        Serial.printf("채널 %d 초기 상태 로드 실패, 기본값 사용\n", i+1);
      }
      digitalWrite(relayPins[i], relayState[i]);
    }
  } else {
    Serial.printf("Firebase 로그인 실패: %s\n", config.signer.signupError.message.c_str());
  }
}

// 웹서버 설정
void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/relays", HTTP_GET, handleGetStates);
  server.on("/relay", HTTP_GET, handleRelayToggle);
  server.on("/data", HTTP_GET, handleData);
  server.serveStatic("/", SPIFFS, "/");
  
  server.onNotFound([]() {
    debugPrint("404 오류: " + server.uri());
    handleFileRead(server.uri());
  });

  server.begin();
  Serial.println("웹 서버 시작 완료");
}

// 파일 읽기 처리
void handleFileRead(String path) {
  if (path.endsWith("/")) path += "index.html";
  
  String contentType;
  if (path.endsWith(".html")) contentType = "text/html";
  else if (path.endsWith(".js")) contentType = "application/javascript";
  else if (path.endsWith(".css")) contentType = "text/css";
  else if (path.endsWith(".json")) contentType = "application/json";
  else if (path.endsWith(".ico")) contentType = "image/x-icon";
  else if (path.endsWith(".png")) contentType = "image/png";
  else contentType = "text/plain";
  
  debugPrint("요청 파일: " + path);
  
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "File Not Found");
  }
}

// 루트 페이지 처리
void handleRoot() {
  debugPrint("루트 경로 요청 처리");
  handleFileRead("/index.html");
}

// 릴레이 상태 요청 처리
void handleGetStates() {
  debugPrint("릴레이 상태 요청 처리");
  String json = "[";
  for (int i = 0; i < numChannels; i++) {
    json += String(relayState[i]);
    if (i != numChannels - 1) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
  debugPrint("응답: " + json);
}

// 릴레이 토글 요청 처리
void handleRelayToggle() {
  debugPrint("릴레이 토글 요청 처리");
  
  #if DEBUG
    String allArgs = "요청 인자: ";
    for (int i = 0; i < server.args(); i++) {
      allArgs += " [" + server.argName(i) + "=" + server.arg(i) + "]";
    }
    debugPrint(allArgs);
  #endif
  
  // 매개변수 파싱
  int channel = -1;
  bool toggleAction = false;
  bool newState = false;
  
  // 다양한 요청 형식 처리
  if (server.hasArg("relay")) {
    channel = server.arg("relay").toInt() - 1;
    toggleAction = true;
    
    // value 매개변수가 있으면 직접 상태 설정
    if (server.hasArg("value")) {
      newState = server.arg("value").toInt() == 1;
      debugPrint("웹 요청: 채널=" + String(channel+1) + ", 상태=" + String(newState));
    } else {
      debugPrint("웹 토글 요청: 채널=" + String(channel+1));
    }
  }
  else if (server.hasArg("ch") && server.hasArg("toggle")) {
    channel = server.arg("ch").toInt() - 1;
    toggleAction = server.arg("toggle").toInt() == 1;
  }
  else if (server.hasArg("id") && server.hasArg("state")) {
    channel = server.arg("id").toInt() - 1;
    newState = server.arg("state").toInt() == 1;
    toggleAction = true;
  }
  
  // 채널 유효성 검사
  if (channel < 0 || channel >= numChannels) {
    server.send(400, "application/json", "{\"error\":\"Invalid channel\"}");
    return;
  }
  
  if (!toggleAction) {
    server.send(400, "application/json", "{\"error\":\"Missing toggle information\"}");
    return;
  }
  
  // 상태 업데이트
  bool isToggleMode = !(server.hasArg("state") || server.hasArg("value"));
  updateRelayState(channel, newState, isToggleMode);
  
  // 응답 전송
  String response = "{\"relay" + String(channel+1) + "\":" + String(relayState[channel]) + "}";
  server.send(200, "application/json", response);
}

// 상태 데이터 응답 처리
void handleData() {
  String json = "{";
  for (int i = 0; i < numChannels; i++) {
    json += "\"relay" + String(i+1) + "\":" + String(relayState[i]);
    if (i != numChannels - 1) json += ",";
  }
  json += "}";
  
  server.send(200, "application/json", json);
  debugPrint("데이터 응답: " + json);
}

// 릴레이 상태 업데이트 (통합 함수)
void updateRelayState(int channel, bool newState, bool isToggle) {
  // 상태 설정 (토글 또는 직접 설정)
  if (isToggle) {
    relayState[channel] = !relayState[channel];
    debugPrint("상태 토글: 채널 " + String(channel+1) + " = " + String(relayState[channel]));
  } else {
    relayState[channel] = newState;
    debugPrint("상태 설정: 채널 " + String(channel+1) + " = " + String(relayState[channel]));
  }
  
  // 출력 설정
  digitalWrite(relayPins[channel], relayState[channel]);
  
  // Firebase 업데이트
  if (signupOK) {
    String path = getFirebasePath(channel);
    if (Firebase.RTDB.setInt(&fbdo, path.c_str(), relayState[channel])) {
      debugPrint("Firebase 업데이트 성공: " + path);
    } else {
      debugPrint("Firebase 업데이트 실패");
    }
  }
}

// Firebase와 동기화
void syncWithFirebase() {
  if (!signupOK) return;
  
  for (int i = 0; i < numChannels; i++) {
    String path = getFirebasePath(i);
    if (Firebase.RTDB.getInt(&fbdo, path.c_str())) {
      int fbRelay = fbdo.to<int>();
      if (fbRelay != relayState[i]) {
        debugPrint("Firebase 동기화: 채널 " + String(i+1) + " = " + String(fbRelay));
        relayState[i] = fbRelay;
        digitalWrite(relayPins[i], relayState[i]);
      }
    }
  }
}

// 물리적 버튼 처리
void checkPhysicalButtons() {
  for (int i = 0; i < numChannels; i++) {
    int btn = digitalRead(buttonPins[i]);
    
    if (lastButtonState[i] == HIGH && btn == LOW) {
      debugPrint("버튼 " + String(i+1) + " 눌림 감지");
      updateRelayState(i, false, true); // 토글 모드
      delay(DEBOUNCE_DELAY);
    }
    lastButtonState[i] = btn;
  }
}

// Firebase 경로 생성
String getFirebasePath(int channel) {
  return String(FIREBASE_LED_PATH) + String(channel+1);
}

// 디버그 출력 함수
void debugPrint(const String &message) {
  #if DEBUG
    Serial.println(message);
  #endif
}