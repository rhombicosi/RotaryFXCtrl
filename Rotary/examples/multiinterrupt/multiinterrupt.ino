/*
 * Example using the Rotary library, dumping integers to the serial
 * port. The integers increment or decrement depending on the direction
 * of rotation.
 *
 * This example uses interrupts rather than polling.
 */

#include <Rotary.h>

const int CLK = 14;
const int DT = 15;

const int CLK0 = 32;
const int DT0 = 33;

Rotary ro = Rotary(14, 15);
Rotary ro0 = Rotary(32, 33);

void setup() {
  Serial.begin(115200);
  
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);

  pinMode(CLK0, INPUT_PULLUP);
  pinMode(DT0, INPUT_PULLUP);


  attachInterrupt(digitalPinToInterrupt(CLK), rotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT), rotate, CHANGE);

  attachInterrupt(digitalPinToInterrupt(CLK0), rotate0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT0), rotate0, CHANGE);
}

void loop() {
}

void rotate() 
{
  ro.process();
  ro.rotate();
}

void rotate0()
{
  ro0.process();
  ro0.rotate();
}
