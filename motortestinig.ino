#include "Enes100.h"
#include <Stepper.h>


// --- Pin Definitions ---
// First H-Bridge (Motors A & B)
#define IN1_1 31
#define IN2_1 33
#define IN3_1 35
#define IN4_1 37
#define ENA_1 2
#define ENB_1 3

// Second H-Bridge (Motors C & D)
#define IN1_2 41
#define IN2_2 43
#define IN3_2 45
#define IN4_2 49
#define ENA_2 6
#define ENB_2 7

//color sensor
#define S0 4
#define S1 5
#define S2 9
#define S3 10
#define OUT 8

//wifi module
#define TX 50
#define RX 51

//distance ultrasonic sensor
#define trigPinDistance 27
#define echoPinDistance 29

//depth ultrasonic sensor
#define trigPinDepth 13
#define echoPinDepth 12

//navigation system
#define upTheta 0.05
#define downTheta -3.1
#define rightTheta -1.46
#define leftTheta 1.56
#define missionStart1 0.5
#define missionStart2 1.5

//stepper motor
#define IN1 25
#define IN2 23
#define IN3 24
#define IN4 22

// Variables to store the frequency readings for each color
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// --- Global Variables ---
int motorSpeed = 200; // PWM speed value (0–255)

// --- Function Prototypes ---
void moveForward(int speedVal);
void moveBackward(int speedVal);
void stopMotors();

void frontLeftForward(int speedVal);
void frontRightForward(int speedVal);
void rearLeftForward(int speedVal);
void rearRightForward(int speedVal);

void frontLeftBackward(int speedVal);
void frontRightBackward(int speedVal);
void rearLeftBackward(int speedVal);
void rearRightBackward(int speedVal);

void turnLeft(int speedVal);
void turnRight(int speedVal);

void turnToAngle(int speedVal, float targetAngle);

void moveUp(int speedVal);
void moveDown(int speedVal);
void moveRight(int speedVal);
void moveLeft(int speedVal);

void navigateToMission(int speedVal);

void testMotor(char key);

int stop;

// --- Setup ---
void setup() {
  
  Enes100.begin("Aquaholics", WATER, 85, 1116, TX, RX);
  delay(1000);
  Enes100.println(Enes100.isConnected());
  
  // Motor pins
  pinMode(IN1_1, OUTPUT);
  pinMode(IN2_1, OUTPUT);
  pinMode(IN3_1, OUTPUT);
  pinMode(IN4_1, OUTPUT);
  pinMode(ENA_1, OUTPUT);
  pinMode(ENB_1, OUTPUT);

  pinMode(IN1_2, OUTPUT);
  pinMode(IN2_2, OUTPUT);
  pinMode(IN3_2, OUTPUT);
  pinMode(IN4_2, OUTPUT);
  pinMode(ENA_2, OUTPUT);
  pinMode(ENB_2, OUTPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  // Set the output pin as an input
  pinMode(OUT, INPUT);

  // Set the frequency scaling to 20% (common for Arduino)
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  pinMode(trigPinDistance, OUTPUT);
  pinMode(echoPinDistance, INPUT);

  stop = 1;

}

// --- Main Loop ---
void loop() {
  
  if(stop == 1){
    navigateToMission(200);
    navigateObstacles(200);
  }


  stop = 0;
}


void navigateToMission(int speedVal){

  if(Enes100.getY() < 1){ //bottom of arena start

    turnToAngle(speedVal, 0);
    
    stopMotors();
    delay(1000);

    while(Enes100.getY() < 1.5){
      moveForward(speedVal);
    }

  }
  else{//top of arena start

    turnToAngle(speedVal, PI);

    stopMotors();
    delay(1000);

    while(Enes100.getY() > 0.5){
      moveForward(speedVal);
    }
  }

  stopMotors();
}


void navigateObstacles(int speedVal){

  turnToAngle(speedVal, -PI/2 - 0.2);

  while(Enes100.getX() < 0.9){
    moveForward(speedVal);
  }

  int distanceFromObstacle = averageDistanceReading(27, 29);

  if(distanceFromObstacle > 45){
    
    while(Enes100.getX() < 1.8){
      moveForward(speedVal);
    }
  }
  else if (Enes100.getY() > 1){

    turnToAngle(speedVal, -PI);

    while(Enes100.getY() > 1){
      moveForward(speedVal);
    }

    turnToAngle(speedVal, -PI/2);

    int distanceFromObstacle = averageDistanceReading(27, 29);

    if(distanceFromObstacle > 45){
        
      while(Enes100.getX() < 1.8){
        moveForward(speedVal);
      }
    }
    else{
      
      turnToAngle(speedVal, -PI);

      while(Enes100.getY() > 0.5){
        moveForward(speedVal);
      }

      turnToAngle(speedVal, -PI/2);

      while(Enes100.getX() < 1.8){
        moveForward(speedVal);
      }
    }
  }
}

// --- Motor Control Functions ---
void moveForward(int speedVal) {
  frontLeftForward(speedVal);
  frontRightForward(speedVal);
  rearLeftForward(speedVal);
  rearRightForward(speedVal);
}

void moveBackward(int speedVal) {
  frontLeftBackward(speedVal);
  frontRightBackward(speedVal);
  rearLeftBackward(speedVal);
  rearRightBackward(speedVal);
}

void stopMotors() {
  digitalWrite(IN1_1, LOW);
  digitalWrite(IN2_1, LOW);
  digitalWrite(IN3_1, LOW);
  digitalWrite(IN4_1, LOW);
  digitalWrite(IN1_2, LOW);
  digitalWrite(IN2_2, LOW);
  digitalWrite(IN3_2, LOW);
  digitalWrite(IN4_2, LOW);

  analogWrite(ENA_1, 0);
  analogWrite(ENB_1, 0);
  analogWrite(ENA_2, 0);
  analogWrite(ENB_2, 0);
}

// --- Individual Motor Forward Functions ---
void frontLeftForward(int speedVal) {
  // Reversed direction
  digitalWrite(IN1_2, LOW);
  digitalWrite(IN2_2, HIGH);
  analogWrite(ENA_2, speedVal);
}

void frontRightForward(int speedVal) {
  digitalWrite(IN3_2, HIGH);
  digitalWrite(IN4_2, LOW);
  analogWrite(ENB_2, speedVal);
}

void rearLeftForward(int speedVal) {
  digitalWrite(IN3_1, HIGH);
  digitalWrite(IN4_1, LOW);
  analogWrite(ENB_1, speedVal);
}

void rearRightForward(int speedVal) {
  digitalWrite(IN1_1, HIGH);
  digitalWrite(IN2_1, LOW);
  analogWrite(ENA_1, speedVal);
}

// --- Individual Motor Backward Functions ---
void frontLeftBackward(int speedVal) {
  // Reversed direction
  digitalWrite(IN1_2, HIGH);
  digitalWrite(IN2_2, LOW);
  analogWrite(ENA_2, speedVal);
}

void frontRightBackward(int speedVal) {
  digitalWrite(IN3_2, LOW);
  digitalWrite(IN4_2, HIGH);
  analogWrite(ENB_2, speedVal);
}

void rearLeftBackward(int speedVal) {
  digitalWrite(IN3_1, LOW);
  digitalWrite(IN4_1, HIGH);
  analogWrite(ENB_1, speedVal);
}

void rearRightBackward(int speedVal) {
  digitalWrite(IN1_1, LOW);
  digitalWrite(IN2_1, HIGH);
  analogWrite(ENA_1, speedVal);
}

void turnRight(int speedVal) {
  // Front-left and rear-left move forward
  frontLeftForward(speedVal);
  rearLeftForward(speedVal);
  
  // Front-right and rear-right move backward
  frontRightBackward(speedVal);
  rearRightBackward(speedVal);
}

void turnLeft(int speedVal) {
  // Front-right and rear-right move forward
  frontRightForward(speedVal);
  rearRightForward(speedVal);
  
  // Front-left and rear-left move backward
  frontLeftBackward(speedVal);
  rearLeftBackward(speedVal);
}

void turnToAngle(int speedVal, float targetAngle) {
  // Normalize target angle to [-PI, PI]
  if (targetAngle > PI) targetAngle -= 2 * PI;
  if (targetAngle < -PI) targetAngle += 2 * PI;

  // Get initial angle (retry if invalid) - Good as is
  float currentAngle = Enes100.getTheta();
  while (currentAngle == -1) {
    currentAngle = Enes100.getTheta();
    delay(5);
  }

  // Safety timer and parameters
  unsigned long startTime = millis();
  const unsigned long TIMEOUT = 8000; // stop turning after 8 seconds
  const float tolerance = 0.03; // radians (~5 degrees)

  // --- NEW: Proportional Control Constants ---
  const float Kp = speedVal / PI; // A basic proportionality constant: Full speed at PI error, 0 speed at 0 error
  const int MIN_SPEED = 80;       // Minimum motor speed to ensure movement
  // ------------------------------------------

  // Start turning loop
  while (true) {
    currentAngle = Enes100.getTheta();
    if (currentAngle == -1) {
      delay(5);
      continue; // skip invalid readings
    }

    // Compute shortest angular difference (ERROR)
    float error = targetAngle - currentAngle;
    if (error > PI) error -= 2 * PI;
    if (error < -PI) error += 2 * PI;

    // 1. Check if we've reached the target (stopping condition remains the same)
    if (abs(error) < tolerance) {
      stopMotors();
      break;
    }

    // 2. Safety timeout
    if (millis() - startTime > TIMEOUT) {
      stopMotors();
      break;
    }

    // --- CRITICAL CHANGE: Proportional Control Implementation ---

    // Calculate the required speed based on the error
    // The magnitude of the speed is proportional to the magnitude of the error.
    int proportionalSpeed = abs(error) * Kp;

    // Limit the calculated speed between a minimum and the initial speedVal
    int motorSpeed = constrain(proportionalSpeed, MIN_SPEED, speedVal);

    // 3. Continuously choose direction and apply the calculated speed
    if (error > 0) {
      // Turn left (positive error) with the proportional speed
      turnLeft(motorSpeed);
    } else { // error < 0
      // Turn right (negative error) with the proportional speed
      turnRight(motorSpeed);
    }

    // Delay between readings to prevent over-rapid looping
    delay(5);
  }
}

void moveUp(int speedVal){
  turnToAngle(speedVal, 0);
  moveForward(speedVal);
}

void moveDown(int speedVal){
  turnToAngle(speedVal, PI);
  moveForward(speedVal);
}

void moveLeft(int speedVal){
  turnToAngle(speedVal, -PI/2);
  moveForward(speedVal);
}

void moveRight(int speedVal){
  turnToAngle(speedVal, PI/2);
  moveForward(speedVal);
}


// --- Keyboard Testing Function ---
void testMotor(char key) {
  switch (key) {
    case '1': frontLeftForward(motorSpeed); break;
    case '2': frontRightForward(motorSpeed); break;
    case '3': rearLeftForward(motorSpeed); break;
    case '4': rearRightForward(motorSpeed); break;
    case 'F': moveForward(motorSpeed); break;
    case 'B': moveBackward(motorSpeed); break;
    case 'S': stopMotors(); break;
    default: break;
  }
}

void colorSensor(int readings){

  for(int i = 0; i < readings; i++){

    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    redFrequency = pulseIn(OUT, LOW); // Measure the low pulse width

    // Read Blue color frequency
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    blueFrequency = pulseIn(OUT, LOW);

    // Read Green color frequency
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    greenFrequency = pulseIn(OUT, LOW);

    // Print the frequencies to the Serial Monitor
    Serial.print("Red: ");
    Serial.print(redFrequency);
    Serial.print(" | Green: ");
    Serial.print(greenFrequency);
    Serial.print(" | Blue: ");
    Serial.println(blueFrequency);

    if(redFrequency < 700 || blueFrequency < 200 || greenFrequency < 200){
      Serial.println("Pollutants are present");
     // Enes100.println("Pollutants are present");
     // Enes100.mission(WATER_TYPE, FRESH_POLLUTED);
    }
    else{
      Serial.println("Pollutants are not present");
     // Enes100.println("Pollutants are not present");
     // Enes100.mission(WATER_TYPE, FRESH_UNPOLLUTED);

    }

  }
  
}

int readDistanceMM(int trigPin, int echoPin) {
  long duration;
  int distanceMM;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  
  // Speed of sound is approx. 0.343 mm/uS.
  // Distance = (duration * speed of sound) / 2 (for one-way trip)
  // 0.343 / 2 approx 0.1715
  distanceMM = duration * 0.1715; 
  
  return distanceMM;
}

int averageDistanceReading(int trigPin, int echoPin) {
  const int NUM_READINGS = 20;
  const int OUTLIERS_TO_DISCARD = 2;
  const int VALID_READINGS = NUM_READINGS - (2 * OUTLIERS_TO_DISCARD);
  const int DELAY_MS = 200;
  
  int readings[NUM_READINGS];
  
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = readDistanceMM(trigPin, echoPin);
    delay(DELAY_MS);
  }
  
  for (int i = 0; i < NUM_READINGS - 1; i++) {
    for (int j = 0; j < NUM_READINGS - i - 1; j++) {
      if (readings[j] > readings[j + 1]) {
        int temp = readings[j];
        readings[j] = readings[j + 1];
        readings[j + 1] = temp;
      }
    }
  }
  
  long sum = 0;
  
  for (int i = OUTLIERS_TO_DISCARD; i < NUM_READINGS - OUTLIERS_TO_DISCARD; i++) {
    sum += readings[i];
  }
  
  float average = (float)sum / VALID_READINGS;
  
  return (int)average;
}
