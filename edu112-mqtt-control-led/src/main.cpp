// Copaland ESP32 IoT MQTT LED Control Example
// PubSubClient by Nick O'Leary
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClient.h>    
#include <PubSubClient.h>  // Make sure this library is installed

// WiFi 정보
const char* ssid = "academy5";
const char* password = "";

//시뮬레이션
//const char* ssid = "Wokwi-GUEST";    // Wokwi-GUEST Wi-Fi SSID
//const char* password = ""; // Wokwi passwords

// MQTT broker
const char* mqtt_broker = "broker.hivemq.com"; // a public Mosquitto MQTT broker
//const char* mqtt_broker = "test.mosquitto.org";  // a public Mosquitto MQTT broker
const int mqtt_port = 1883;                      // MQTT uses port 1883 for an unencrypted connection

// Define a unique clientId for the MQTT client
String clientId = "copaland_esp32_iot";  // 고유한 클라이언트 ID

// Handle MQTT topics for sending and receiving control signals
const char* topic_led_red = "esp32/led_control_red";        // red LED control
const char* topic_led_green = "esp32/led_control_green";    // green LED control
const char* topic_led_blue = "esp32/led_control_blue";  // yellow LED control

const int red_led_pin = 15;     // GPIO 13 (connect to red led）
const int blue_led_pin = 16;  // GPIO 14 (connect to yellow LED）
const int green_led_pin = 21;   // GPIO 16 (connect to green LED）

// Create WiFi client and MQTT client instances
WiFiClient wifiClient;                // Use normal WiFiClient to connect
PubSubClient mqttClient(wifiClient);  // the MQTT client using the WiFi client

long previous_time = 0;  // store the last time data was sent

// MQTT message callback function that processes received messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);  // Topic of the output message
  Serial.print("]: ");
  String message;

  // Convert the received byte data to a string
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("message:");
  Serial.println(message);

  // control LED according to the received message
  if (message == "on-r") {
    Serial.println("Turning on LED R");
    digitalWrite(red_led_pin, HIGH);  // turn on red led
  } else if (message == "off-r") {
    Serial.println("Turning off LED R");
    digitalWrite(red_led_pin, LOW);  // turn off red led
  } else if (message == "on-g") {
    Serial.println("Turning on LED G");
    digitalWrite(green_led_pin, HIGH);  // turn on green LED
  } else if (message == "off-g") {
    Serial.println("Turning off LED G");
    digitalWrite(green_led_pin, LOW);  // turn off green LED
  } else if (message == "on-b") {
    Serial.println("Turning on LED Y");
    digitalWrite(blue_led_pin, HIGH);  // turn on blue LED
  } else if (message == "off-b") {
    Serial.println("Turning off LED Y"); 
    digitalWrite(blue_led_pin, LOW);  // turn off blue LED
  }
}

// Set up the MQTT client
void setupMQTT() {
  mqttClient.setServer(mqtt_broker, mqtt_port);  // Set the MQTT broker
  mqttClient.setCallback(mqttCallback);          // Set the message callback function
}

// try to re-connect to MQTT broker
void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {  // If it is not connected, is repeated
    Serial.println("Reconnecting to MQTT Broker...");
    clientId += String(random(0xffff), HEX);  
    Serial.print("Failed to connect to MQTT Broker, state: ");
    Serial.println(mqttClient.state());

    if (mqttClient.connect(clientId.c_str())) {  //Connect to the broker through clientId
      Serial.println("Connected to MQTT Broker.");

      // subsribe LED control topic
      mqttClient.subscribe(topic_led_red);     // subsribe red LED control
      mqttClient.subscribe(topic_led_green);   // subsribe green LED control
      mqttClient.subscribe(topic_led_blue);  // subsribe blue LED control
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());  // Print the reason for the connection failure
      Serial.println(" try again in 5 seconds");
      delay(5000);  // Retry the connection every 5 seconds
    }
  }
}

void setup() {
  Serial.begin(115200);  // set baud rate to 115200

  // set LED pin to output
  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);

  // connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {  // Wait for the Wi-Fi connection
    delay(500);                            // Check the Wi-Fi status every 500ms
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to Wi-Fi");

  setupMQTT();  // Set up the MQTT client
}

void loop() {
  if (!mqttClient.connected()) {  // If MQTT broker is not connected
    reconnect();                  // Attempt to reconnect
  }
  mqttClient.loop();  // Keep MQTT client connections active
}
