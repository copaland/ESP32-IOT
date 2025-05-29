#include <Arduino.h>
#include <WiFi.h>
#include "time.h"

// WiFi 정보
const char* ssid = "academy5";
const char* password = "";
// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";

unsigned long epochTime; // Variable to save current epoch time

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  // 한국 시간대(KST, UTC+9)로 설정
  configTime(9 * 3600, 0, ntpServer);
}

void loop() {
  epochTime = getTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);

  // epochTime을 날짜와 시간 문자열로 변환
  time_t rawTime = epochTime;
  struct tm * timeinfo = localtime(&rawTime);
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  Serial.print("DateTime: ");
  Serial.println(buffer);

  delay(1000);
}
