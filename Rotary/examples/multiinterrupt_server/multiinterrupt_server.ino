#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

#include <Rotary.h>

const int CLK[2] = { 14, 32};
const int DT[2] = { 15, 33};

Rotary ro[2] = { Rotary(CLK[0], DT[0]), Rotary(CLK[1], DT[1])};

void setup() {
  Serial.begin(115200);

  for(int i = 0; i < 2; i++) {
    pinMode(CLK[i], INPUT_PULLUP);
    pinMode(DT[i], INPUT_PULLUP);
  }
  
  attachInterrupt(digitalPinToInterrupt(CLK[0]), rotate0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT[0]), rotate0, CHANGE);

  attachInterrupt(digitalPinToInterrupt(CLK[1]), rotate1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT[1]), rotate1, CHANGE);
}

void loop() {
}

void rotate0() 
{
  ro[0].process();
  ro[0].rotate();
}

void rotate1()
{
  ro[1].process();
  ro[1].rotate();
}
