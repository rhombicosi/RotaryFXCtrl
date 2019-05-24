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

//const int encoderStepsPerRevolution = 30;
//volatile int angle = 0;
//volatile int period = 0;
//
//volatile int angle0 = 0;

// Rotary encoder is wired with the common to ground and the two
// outputs to pins 14 and 15.
Rotary ro = Rotary(14, 15);
Rotary ro0 = Rotary(32, 33);

void setup() {
  Serial.begin(115200);
  
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);

  pinMode(CLK0, INPUT_PULLUP);
  pinMode(DT0, INPUT_PULLUP);


  attachInterrupt(digitalPinToInterrupt(CLK), rotor, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT), rotor, CHANGE);

  attachInterrupt(digitalPinToInterrupt(CLK0), rotor0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT0), rotor0, CHANGE);
}

void loop() {
}

void rotor() 
{
  ro.process();
  ro.rotate();
}

void rotor0()
{
  ro0.process();
  ro0.rotate();
}
