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

void testMotor(char key);

int stop;

// --- Setup ---
void setup() {
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

  Serial.println("Motor Test Mode Active");
  Serial.println("Press 1–4 to test each wheel");
  Serial.println("1 = Front Left | 2 = Front Right | 3 = Rear Left | 4 = Rear Right");

  stop = 1;
}

// --- Main Loop ---
void loop() {

  if(stop == 1){

    moveForward(255);
    delay(3000);
    turnRight(200);
    delay(3000);
    moveForward(200);
    delay(2000);
    stopMotors();
  }
  stop = 0;
  if (Serial.available()) {
    char key = Serial.read();
    key = toupper(key); // normalize input
    testMotor(key);
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
