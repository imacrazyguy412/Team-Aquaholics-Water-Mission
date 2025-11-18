#include <Stepper.h>

// Define the motor pins
#define IN1 25
#define IN2 23
#define IN3 24
#define IN4 22

const int stepsPerRev = 2048;

// Use the pins you defined in the constructor
Stepper myStepper(stepsPerRev, IN1, IN3, IN2, IN4);
// Note: The order is important! Typically: IN1, IN2, IN3, IN4
// But some 28BYJ-48 motors may need different sequencing

void setup() {
  // Initialize the stepper speed
  myStepper.setSpeed(10);  // 10 RPM
  
  // Optional: Initialize serial for debugging
  Serial.begin(9600);
  Serial.println("Stepper motor initialized");
}

void loop() {
  // Move the bucket forward
  Serial.println("Moving bucket forward...");
  myStepper.step(2000);   // 90 degrees for 2048-step motor
  delay(2000);

  // Move the bucket back to original position
  Serial.println("Moving bucket backward...");
  myStepper.step(-2000);  // Reverse 90 degrees
  delay(2000);
}
