#include <Arduino.h>

#define freq       1000  //define the pwm frequency
#define resolution 8     //define the pwm precision

int ledPins[] = {15, 21, 16};    //define red, green, blue led pins
const byte chns[] = {0, 1, 2};        //define the pwm channels
int red, green, blue;

void setColor(byte r, byte g, byte b) {
  ledcWrite(chns[0], r); //255-r Common anode LED
  ledcWrite(chns[1], g);
  ledcWrite(chns[2], b);
}

void setup() {
  for (int i = 0; i < 2; i++) {   //setup the pwm channels,1KHz,8bit
    ledcSetup(chns[i], freq, resolution);
    ledcAttachPin(ledPins[i], chns[i]);
  }
}

void loop() {
  red = random(0, 256);
  green = random(0, 256);
  blue = random(0, 256);
  setColor(red, green, blue);
  delay(200);
}

