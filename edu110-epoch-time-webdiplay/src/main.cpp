#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "time.h"
#include <ArduinoJson.h> // ArduinoJson by Benoit Blanchon

// WiFi 정보
const char* ssid = "academy5";
const char* password = "";

// NTP 서버 설정
const char* ntpServer = "pool.ntp.org"; // NTP 서버 주소
const long  gmtOffset_sec = 32400;    // GMT 기준 시간 오프셋 (한국은 +9시간 = 9 * 60 * 60 = 32400초)
const int   daylightOffset_sec = 0;   // 서머타임 오프셋 (한국은 서머타임 없음)

// 날짜 및 시간 문자열 저장을 위한 버퍼
char dateStringBuff[20];
char timeStringBuff[20];

// 웹 서버 객체 생성 (포트 80 사용)
WebServer server(80);

// 마지막으로 시리얼에 시간을 출력한 시간 기록
unsigned long lastSerialTime = 0;
// 시리얼 출력 간격 (밀리초), 예: 5000ms = 5초
const unsigned long serialPrintInterval = 5000;

// 현재 날짜를 포맷된 문자열 (년-월-일)로 가져오는 함수
String getFormattedDate() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "Date Error";
  }
  strftime(dateStringBuff, sizeof(dateStringBuff), "%Y-%m-%d", &timeinfo);
  return String(dateStringBuff);
}

// 현재 시간을 포맷된 문자열 (시:분:초)로 가져오는 함수
String getFormattedTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "Time Error";
  }
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
  return String(timeStringBuff);
}

// 웹페이지의 HTML 및 CSS 콘텐츠를 PROGMEM에 저장 (메모리 절약)
const char HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <link rel='icon' href='data:,'>
  <title>NTP Date & Time</title> <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    body {margin: 0; background-color: #f0f0f0; display: flex; justify-content: flex-start; align-items: center; height: 100vh; flex-direction: column;} /* 중앙 정렬 및 높이 설정, 상단 정렬 */
    .topnav {overflow: hidden; background-color: #1b78e2; width: 100%; position: fixed; top: 0;} /* 상단 고정 */
    h3 {font-size: 1.8rem; color: white;}
    .content {padding: 20px; margin-top: 60px; width: 100%;} /* 상단 네비게이션 높이 */
    /* 날짜 표시 스타일 */
    .date-display {
      font-size: 2em; /* 날짜 폰트 크기 */
      font-weight: bold; /* 볼드체 */
      color: #555555; /* 날짜 색상 */
      margin-bottom: 10px; /* 날짜와 시간 사이 간격 */
    }
    /* 시간 표시 스타일 */
    .time-display {
      font-size: 8em; /* 시간 폰트 크기 8배 확대 */
      font-weight: bold; /* 볼드체 */
      color: #008CBA; /* 시간 색상 */
    }
</style>

  <script>
    function updateData() {
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var data = JSON.parse(this.responseText);
          // 날짜 및 시간 요소를 찾아서 업데이트
          document.getElementById('date').innerHTML = data.date;
          document.getElementById('time').innerHTML = data.time;
        }
      };
      xhr.open('GET', '/data', true);
      xhr.send();
    }

    let updateInterval;
    document.addEventListener('DOMContentLoaded', function() {
      updateData(); // 페이지 로드 시 한 번 업데이트
      updateInterval = setInterval(updateData, 1000); // 1초마다 함수 실행 (업데이트)
    });
  </script>
</head>
<body>
<div class='topnav'>
    <h3>NTP Date & Time Display</h3> </div>
  <div class='content'>
    <p class='date-display' id='date'>%DATE%</p> <p class='time-display' id='time'>%TIME%</p> </div>
</body>
</html>
)rawliteral";

// HTML 템플릿의 플레이스홀더(%PLACEHOLDER%)를 실제 값으로 교체하는 함수
String processHtml() {
  String page = FPSTR(HTML);
  // 페이지 로드 시 초기 날짜와 시간으로 플레이스홀더 교체
  page.replace("%DATE%", getFormattedDate());
  page.replace("%TIME%", getFormattedTime());
  return page;
}

void setup() {
  Serial.begin(115200);

  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
int retry_count = 0;

  while (WiFi.status() != WL_CONNECTED && retry_count < 30) {
    delay(500);
    Serial.print(".");
    retry_count++;
  }
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // NTP 서버를 이용한 시간 설정 (한국 시간)
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("Time configured from NTP"); // NTP 설정 완료 메시지

    // 웹 서버 라우트 설정
    server.on("/", HTTP_GET, []() {
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate, private, max-age=0");
        server.sendHeader("Pragma", "no-cache");
        server.sendHeader("Expires", "Mon, 26 Jul 1997 05:00:00 GMT");
        server.send(200, "text/html", processHtml());
    });

    // 데이터 업데이트 요청("/data")에 대한 GET 요청 처리
    server.on("/data", HTTP_GET, []() {
      JsonDocument doc; // DynamicJsonDocument → JsonDocument
      // 웹 페이지에 표시할 날짜 및 시간 데이터 추가
      doc["date"] = getFormattedDate();
      doc["time"] = getFormattedTime();
      String jsonResponse;
      serializeJson(doc, jsonResponse);
      server.send(200, "application/json", jsonResponse);
    });

    server.begin();
    Serial.println("HTTP server started");
  } else {
    Serial.println("WiFi connection failed. Cannot start server.");
  }
}

void loop() {
  // Wi-Fi 연결 상태 확인 및 재연결 시도
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(1000); // 재연결 시도 중 잠시 대기
    return; // 연결 끊겼을 때는 다른 루프 작업 생략
  }

  // 웹 클라이언트의 요청을 처리
  server.handleClient();

  // 주기적으로 시간을 시리얼 모니터에 출력
  if (millis() - lastSerialTime >= serialPrintInterval) {
    String currentDate = getFormattedDate();
    String currentTime = getFormattedTime();
    Serial.print("Current Date (NTP): ");
    Serial.println(currentDate);
    Serial.print("Current Time (NTP): ");
    Serial.println(currentTime);
    lastSerialTime = millis();
  }

  delay(10); // CPU 과부하 방지를 위한 작은 지연 시간
}
