/*
 * Filename    : Temperature and Humidity Sensor
 * Description : Use XHT11 to measure temperature and humidity.Print the result to the serial port.
 * Device      : ESP32-S3 DevKitC
 * Library     : adafruit/DHT sensor library
*/

#include <Arduino.h>
#include "DHT.h" 

#define DHTPIN 10  // Set the pin connected to the DHT11 data pin
#define DHTTYPE DHT11 // DHT11 DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 test!");
  dht.begin();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!

  float humidity = dht.readHumidity();
  // Read temperature as Celsius (), Fahrenheit(true)
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("RH:");
  Serial.print(humidity); 
  Serial.print("%  ");
  Serial.print("Temp:");
  Serial.print(temperature); 
  Serial.println("C");

  delay(1000);
}


/*
#include <Arduino.h>
#include "xht11.h"
//gpio10
xht11 xht(10);

unsigned char dht[4] = {0, 0, 0, 0};//Only the first 32 bits of data are received, not the parity bits
void setup() {
  Serial.begin(115200);//Start the serial port monitor and set baud rate to 115200
}

void loop() {
  if (xht.receive(dht)) { //Returns true when checked correctly
    Serial.print("RH:");
    Serial.print(dht[0]); //The integral part of humidity, DHT [0] is the fractional part
    Serial.print("%  ");
    Serial.print("Temp:");
    Serial.print(dht[2]); //The integral part of temperature, DHT [3] is the fractional part
    Serial.println("C");
  } else {    //Read error
    Serial.println("sensor error");
  }
  delay(1000);  //It takes 1000ms to wait for the device to read
}
*/