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

#define closeObstacle 1
#define farObstacle 2
#define noObstacle 0

#define COMP_LESS     0
#define COMP_GREATER  1

#define DIR_FORWARD   0
#define DIR_BACKWARD  1
#define DIR_LEFT  2
#define DIR_RIGHT  3

const int stepsPerRev = 2048;

// Use the pins you defined in the constructor
Stepper myStepper(stepsPerRev, IN1, IN3, IN2, IN4);


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

void turn90DegreesRight(int speedVal);

void turn90DegreesLeft(int speedVal);

void moveLeft(int speedVal);
void moveRight(int speedVal);

double readDistanceMM(int trigPin, int echoPin);
void colorSensor(int readings);
double averageDistanceReading(int trigPin, int echoPin);
void waterDepth();

void assignObstacleRows(int* row);

void missionStartBottom();
void missionStartTop();

void moveUntilY(double targetY, int comparison, int direction);

int stop;

// --- Setup ---
void setup() {
  
  Enes100.begin("Aquaholics", WATER, 85, 1120, TX, RX);
  delay(1000);
  Enes100.println(Enes100.isConnected());
  Serial.begin(9600);
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

  pinMode(trigPinDepth, OUTPUT);
  pinMode(echoPinDepth, INPUT);

  myStepper.setSpeed(10);

  stop = 1;

}

// --- Main Loop ---
void loop() {

  if(stop == 1){

    if(Enes100.getY() < 1){
      missionStartBottom();
    }
    else{
      missionStartTop();
    }
  }
    
  stopMotors();
  stop = 0;
}


void missionStartBottom(){

  turnToAngle(200, 0);

  moveUntilY(1.53, COMP_GREATER, DIR_FORWARD);

  stopMotors();
  delay(3000);
  waterDepth();
  delay(1000);
  colorSensor(1);
  delay(1000);
  myStepper.step(4000);
  delay(1000);
  myStepper.step(-4000);
  delay(1000);

  moveUntilY(0.8, COMP_LESS, DIR_BACKWARD);

  turn90DegreesRight(200);
  moveForward(200);

  delay(3200);

  stopMotors();

  int row1;
  int row2;
  int row3;

  int* row1p = &row1;
  int* row2p = &row2;
  int* row3p = &row3;

  assignObstacleRows(row1p);

  turn90DegreesLeft(200);
  moveForward(200);
  delay(4200);
  turn90DegreesRight(200);

  assignObstacleRows(row2p);

  turn90DegreesLeft(200);
  moveForward(200);
  delay(4400);
  turn90DegreesRight(200);

  assignObstacleRows(row3p);

  if(row3 == farObstacle){
    moveForward(200);
    delay(7150);
    turn90DegreesRight(200);
    moveForward(200);

    if(row2 == closeObstacle){
      delay(4500);
      turn90DegreesLeft(200);
      moveForward(200);
      delay(7500);
      turn90DegreesLeft(200);
      moveForward(200);
      delay(5800);
    }
    else if(row1 == closeObstacle){
      delay(8300);
      turn90DegreesLeft(200);
      moveForward(200);
      delay(7500);
      turn90DegreesLeft(200);
      moveForward(200);
      delay(8700);
    }
    turn90DegreesRight(200);
    moveForward(200);
    delay(14000);
    stopMotors();
  }
  else if(row3 == closeObstacle){
    turn90DegreesLeft(200);
    moveBackward(200);
    if(row2 == farObstacle){
      delay(4200);
      turn90DegreesRight(200);
      moveForward(200);
      delay(7000);
      turn90DegreesLeft(200);
      moveForward(200);
      delay(4200);
    }
    else if(row1 == farObstacle){
      delay(9000);
      turn90DegreesRight(200);
      moveForward(200);
      delay(7000);
      turn90DegreesLeft(200);
      moveForward(200);
      delay(8400);
    }
    turn90DegreesRight(200);
    moveForward(200);
    delay(14000);
    stopMotors();
  }
  
}

void missionStartTop(){

  turnToAngle(200, PI);

  moveUntilY(0.45, COMP_LESS, DIR_FORWARD);

  stopMotors();
  delay(3000);
  waterDepth();
  delay(1000);
  colorSensor(1);
  delay(1000);

  myStepper.step(4000);
  delay(1000);
  myStepper.step(-4000);

  moveUntilY(1.2, COMP_GREATER, DIR_BACKWARD);

  turn90DegreesLeft(200);

  moveUntilX(0.95);

  int row1;
  int row2;
  int row3;

  int* row1p = &row1;
  int* row2p = &row2;
  int* row3p = &row3;

  assignObstacleRows(row3p);
  moveUntilY(1, COMP_LESS, DIR_RIGHT);
  assignObstacleRows(row2p);
  moveUntilY(0.5, COMP_LESS, DIR_RIGHT);
  assignObstacleRows(row1p);

  if(row1 == farObstacle){ 
    moveUntilX(2);
    if(row2 == closeObstacle){
      moveUntilY(0.9, COMP_GREATER, DIR_LEFT);
    }
    else if(row3 == closeObstacle){
      moveUntilY(1.53, COMP_GREATER, DIR_LEFT);
    }
  }
  else if(row1 == closeObstacle){
    if(row2 == farObstacle){
      moveUntilY(0.9, COMP_GREATER, DIR_LEFT);
    }
    else if(row3 == farObstacle){
      moveUntilY(1.53, COMP_GREATER, DIR_LEFT);
    }
    moveUntilX(2);
    moveUntilY(0.5, COMP_LESS, DIR_RIGHT); 
  }
  moveUntilX(3.12);
  moveUntilY(1.53, COMP_GREATER, DIR_LEFT);
  moveUntilX(4.02);
}

void assignObstacleRows(int* row){

    int distance = averageDistanceReading(trigPinDistance, echoPinDistance);
    Serial.println(distance);
    if(distance > 20 && distance < 400){
      *row = closeObstacle;
      Enes100.println("close obstacle");
    }
    else if(distance > 500 && distance < 2000){
      *row = farObstacle;
      Enes100.println("far obstacle");

    }
    else{
      *row = noObstacle;
      Enes100.println("no obstacle");

    }
}

void moveUntilY(double targetY, int comparison, int direction) {
    int stableCount = 0;

    while (true) {
        double ypos = Enes100.getY();
        if (ypos == -1) continue;  // invalid reading, skip

        bool conditionMet = false;

        if (comparison == COMP_LESS) {
            conditionMet = (ypos < targetY);
        } 
        else if (comparison == COMP_GREATER) {
            conditionMet = (ypos > targetY);
        }

        // --- stable detection ---
        if (conditionMet) {
            stableCount++;
            if (stableCount >= 3) {   // must be stable for ~15ms
                stopMotors();
                break;
            }
        } else {
            stableCount = 0;
        }

        // --- movement ---
        if (direction == DIR_FORWARD) {
          moveForward(200);
        } 
        else if (direction == DIR_BACKWARD) {
          moveBackward(200);
        }
        else if(direction == DIR_LEFT){
          moveLeft(200);
        }
        else{
          moveRight(200);
        }

        delay(5);
    }
}

void moveUntilX(double targetX) {
    int stableCount = 0;

    while (true) {
        double xpos = Enes100.getX();
        if (xpos == -1) continue;  // invalid reading, skip


        // --- stable detection ---
        if (xpos > targetX) {
            stableCount++;
            if (stableCount >= 3) {   // must be stable for ~15ms
                stopMotors();
                break;
            }
        } else {
            stableCount = 0;
        }

        moveForward(200);

        delay(5);
    }
}

void turn90DegreesRight(int speedVal) {
   
    const unsigned long DECAY_TIME_MS = 250; 
    // MIN_SPEED: The lowest PWM value to use during the slowdown phase. (Must be higher than the motor stall speed)
    const int MIN_SPEED = 80;                

    unsigned long startTime = millis();
    unsigned long endTime = startTime + 2800;

    while (millis() < endTime) {
        unsigned long currentTime = millis();
        unsigned long timeRemaining = endTime - currentTime;
        
        int currentSpeed;
        
        if (timeRemaining > DECAY_TIME_MS) {
            currentSpeed = speedVal;
        } 
        else {

            long speedRange = speedVal - MIN_SPEED;
            // Use long arithmetic to avoid overflow during multiplication
            long calculatedSpeed = MIN_SPEED + (long)speedRange * (long)timeRemaining / DECAY_TIME_MS;
            
            // Ensure the speed is constrained between the set minimum and maximum
            currentSpeed = constrain(calculatedSpeed, MIN_SPEED, speedVal);
        }

        // Apply the calculated speed, hardcoded for a RIGHT turn
        turnRight(currentSpeed);
        
        // Small delay to prevent too rapid motor updates
        delay(5); 
    }
    stopMotors();
}

void turn90DegreesLeft(int speedVal) {
   
    const unsigned long DECAY_TIME_MS = 250; 
    // MIN_SPEED: The lowest PWM value to use during the slowdown phase. (Must be higher than the motor stall speed)
    const int MIN_SPEED = 80;                

    unsigned long startTime = millis();
    unsigned long endTime = startTime + 2800;

    while (millis() < endTime) {
        unsigned long currentTime = millis();
        unsigned long timeRemaining = endTime - currentTime;
        
        int currentSpeed;
        
        if (timeRemaining > DECAY_TIME_MS) {
            currentSpeed = speedVal;
        } 
        else {

            long speedRange = speedVal - MIN_SPEED;
            // Use long arithmetic to avoid overflow during multiplication
            long calculatedSpeed = MIN_SPEED + (long)speedRange * (long)timeRemaining / DECAY_TIME_MS;
            
            // Ensure the speed is constrained between the set minimum and maximum
            currentSpeed = constrain(calculatedSpeed, MIN_SPEED, speedVal);
        }

        // Apply the calculated speed, hardcoded for a RIGHT turn
        turnLeft(currentSpeed);
        
        // Small delay to prevent too rapid motor updates
        delay(5); 
    }
    stopMotors();
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

void moveLeft(int speedVal) {
  frontLeftBackward(speedVal);
  frontRightForward(speedVal);
  rearLeftForward(speedVal);
  rearRightBackward(speedVal);
}

void moveRight(int speedVal) {
  frontLeftForward(speedVal);
  frontRightBackward(speedVal);
  rearLeftBackward(speedVal);
  rearRightForward(speedVal);
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

  // normalize target
  if (targetAngle > PI) targetAngle -= 2*PI;
  if (targetAngle < -PI) targetAngle += 2*PI;

  float currentAngle = Enes100.getTheta();
  while (currentAngle == -1) {
    currentAngle = Enes100.getTheta();
    delay(5);
  }

  const float tolerance = 0.04;   // ~6 degrees
  const float Kp = speedVal / PI;
  const int MIN_SPEED = 62;

  int stableCount = 0;

  while (true) {
    currentAngle = Enes100.getTheta();
    if (currentAngle == -1) continue;

    float error = targetAngle - currentAngle;
    if (error > PI) error -= 2*PI;
    if (error < -PI) error += 2*PI;

    // stable requirement
    if (abs(error) < tolerance) {
      stableCount++;
      if (stableCount >= 10) { // must be stable for ~20ms
        stopMotors();
        break;
      }
    } else {
      stableCount = 0;
    }


    int proportionalSpeed = abs(error) * Kp;
    int motorSpeed = constrain(proportionalSpeed, MIN_SPEED, speedVal);

    if (error > 0) turnLeft(motorSpeed);
    else turnRight(motorSpeed);

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

    if(redFrequency < 400 || blueFrequency < 200 || greenFrequency < 200){
      Serial.println("Pollutants are present");
     //  Enes100.println("Pollutants are present");
      Enes100.mission(WATER_TYPE, FRESH_POLLUTED);
    }
    else{
      Serial.println("Pollutants are not present");
      //Enes100.println("Pollutants are not present");
      Enes100.mission(WATER_TYPE, FRESH_UNPOLLUTED);

    }

  }
  
}

void waterDepth(){

  double waterDepth = 81 - readDistanceMM(trigPinDepth, echoPinDepth);
  
  if(waterDepth > 35){
    Serial.println("Distance = 40 mm");
    Enes100.mission(DEPTH, 40);
  }
  else if(waterDepth > 25 && waterDepth < 35){
    Serial.println("Distance = 30 mm");
    Enes100.mission(DEPTH, 30);
  }
  else if(waterDepth < 25){
    Serial.println("Distance = 20 mm");
    Enes100.mission(DEPTH, 20);
  }
}

double readDistanceMM(int trigPin, int echoPin) {
  long duration;
  double distanceMM;

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
  
  // Added print statement for debugging the raw reading

  //Serial.println(distanceMM);
  
  return distanceMM;
}

double averageDistanceReading(int trigPin, int echoPin) {
  // Constants for data collection and processing
  const int NUM_READINGS = 20;
  const int OUTLIERS_TO_DISCARD = 2; // Discard 2 lowest and 2 highest
  const int VALID_READINGS = NUM_READINGS - (2 * OUTLIERS_TO_DISCARD);
  const int DELAY_MS = 200;
  const int MIN_VALID_DISTANCE_MM = 0; // Minimum distance to consider a reading valid

  // Safety delay before starting (kept from your original request)
  delay(500);
  
  double readings[NUM_READINGS];
  int readingIndex = 0; // Counter for valid readings collected
  
  // Loop until 20 valid readings are collected
  while (readingIndex < NUM_READINGS) {
    // Read the raw distance
    double distance = readDistanceMM(trigPin, echoPin);
    
    // Check if the reading is above the unreliable threshold
    if (distance > MIN_VALID_DISTANCE_MM) {
      readings[readingIndex] = distance;
      readingIndex++; // Only increment if we stored a valid reading
    } 
    // Wait before the next reading (wait even if the reading was discarded)
    delay(DELAY_MS);
  }
  
  // --- Sorting (Bubble Sort) ---
  // Sort the 20 collected valid readings in ascending order
  for (int i = 0; i < NUM_READINGS - 1; i++) {
    for (int j = 0; j < NUM_READINGS - i - 1; j++) {
      if (readings[j] > readings[j + 1]) {
        double temp = readings[j];
        readings[j] = readings[j + 1];
        readings[j + 1] = temp;
      }
    }
  }
  
  // --- Calculate Sum (Discarding Outliers) ---
  double sum = 0;
  
  // Sum only the middle readings (e.g., indexes 2 through 17)
  for (int i = OUTLIERS_TO_DISCARD; i < NUM_READINGS - OUTLIERS_TO_DISCARD; i++) {
    sum += readings[i];
  }
  
  // --- Calculate Average ---
  double average = (double)sum / VALID_READINGS;
  //Enes100.println("average of last 20:");
 // Enes100.println(average);
  //Enes100.println("printing the number 80");
  double x = 80;
  //Enes100.println(x);
 // Serial.println(average);

  
  return (double)average;
}

