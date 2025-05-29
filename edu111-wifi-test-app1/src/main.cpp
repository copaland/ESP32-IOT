//WIFI Test(use keyes link app)
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>

#define sw1Pin 8
#define led1Pin 47

String item = "0";

// WiFi 정보
const char* ssid = "academy5";
const char* password = "";
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(led1Pin, OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("TCP server started");
  MDNS.addService("http", "tcp", 80);
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
      return;
  }
  while(client.connected() && !client.available()){
      delay(1);
  }
  String req = client.readStringUntil('\r');
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
      Serial.print("Invalid request: ");
      Serial.println(req);
      return;
  }
  req = req.substring(addr_start + 1, addr_end);

  // Check if the request is for /favicon.ico
  if (req == "/favicon.ico") {
    // Send a 404 Not Found response for favicon requests
    client.println("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found");
    client.stop();
    return; // Exit the loop function for favicon requests
  }

  item=req;
  Serial.println(item); // Only print to serial for non-favicon requests
  String s;
  if (req == "/")
  {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP32 at ";
      s += ipStr;
      s += "</html>\r\n\r\n";
      Serial.println("Sending 200");
      client.println(s);
  }else if(req == "/btn/0")
  {
    //Serial.write('a');
    client.println(F("turn on the relay"));
    digitalWrite(led1Pin, HIGH);
  }
  else if(req == "/btn/1")
  {
    //Serial.write('b');
    client.println(F("turn off the relay"));
    digitalWrite(led1Pin, LOW);
  }
  //client.print(s);
  client.stop();
}
