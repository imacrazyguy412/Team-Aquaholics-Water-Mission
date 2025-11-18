#include <Stepper.h>

const int stepsPerRev = 2048;

Stepper myStepper(stepsPerRev, 8, 10, 9, 11);

void setup() {
  myStepper.setSpeed(10);  // 10 RPM
}

void loop() {
  // CHOOSE YOUR NUMBER OF STEPS HERE:
  int stepsToMove = 512;   // <-- change this

  // CHOOSE DIRECTION HERE:
  myStepper.step(stepsToMove);   // forward
  delay(1000);

  myStepper.step(-stepsToMove);  // reverse
  delay(1000);
}
    