#include "Arduino.h"
#include "Enes100.h"
#include "Tank.h"

// ================= USER TUNING =================
// Straight segment distances (meters)
const float SEG1_M = 1.00f;   // straight #1
const float SEG2_M = 0.80f;   // straight #2
const float SEG3_M = 1.00f;   // straight #3

// Turn directions: +90 = left (CCW), -90 = right (CW)
const float TURN1_DEG = +90.0f; // after straight #1
const float TURN2_DEG = -90.0f; // immediately after TURN1
const float TURN3_DEG = -90.0f; // immediately after TURN2

// Motor PWMs (0..255); NO REVERSING is used.
const int FWD_PWM  = 160;    // straight speed
const int TURN_PWM = 180;    // pivot speed

// Turn tolerance (stop turning when within this error)
const float TURN_TOL_DEG = 4.0f;

// Stop/pause timing
const int STOP_BEFORE_TURN_MS = 400;  // before the first turn after straight #1
const int STOP_AFTER_TURN_MS  = 300;  // between consecutive turns
// ===============================================

// --------- helpers (no math.h) ----------
static inline float absf(float v){ return v < 0 ? -v : v; }
static inline int   clamp01(int v){ return v < 0 ? 0 : (v > 255 ? 255 : v); }

float wrapDeg(float a){
  while (a >  180.0f) a -= 360.0f;
  while (a < -180.0f) a += 360.0f;
  return a;
}

// Auto-detect if theta is degrees or radians
float thetaToDegAuto(float thRaw){
  float a = thRaw; if (a < 0) a = -a;
  if (a <= 3.5f) return thRaw * (180.0f / 3.1415926f); // radians -> degrees
  return thRaw;                                        // already degrees
}

void setMotorsForward(int L, int R){
  Tank.setLeftMotorPWM (clamp01(L));
  Tank.setRightMotorPWM(clamp01(R));
}

void stopMotors(){
  Tank.setLeftMotorPWM(0);
  Tank.setRightMotorPWM(0);
}

// --------- distance tracking (no sqrt) ----------
float x_start = 0.0f, y_start = 0.0f;
void dist_begin(){ x_start = Enes100.getX(); y_start = Enes100.getY(); }
bool dist_reached(float meters){
  float dx = Enes100.getX() - x_start;
  float dy = Enes100.getY() - y_start;
  return (dx*dx + dy*dy) >= (meters*meters);
}

// --------- turning ----------
float target_heading_deg = 0.0f;
void turn_begin(float delta_deg){
  float current_deg = thetaToDegAuto(Enes100.getTheta());
  target_heading_deg = wrapDeg(current_deg + delta_deg);
}

bool turn_step(){
  float th = thetaToDegAuto(Enes100.getTheta());
  float e  = wrapDeg(target_heading_deg - th);

  if (absf(e) <= TURN_TOL_DEG){
    stopMotors();
    return true; // done turning
  }

  // Pivot in place WITHOUT reverse: stop one side, push the other
  if (e > 0){
    // rotate left (CCW)
    setMotorsForward(0, TURN_PWM);
  } else {
    // rotate right (CW)
    setMotorsForward(TURN_PWM, 0);
  }
  return false;
}

// --------- simple state machine (no enum) ----------
// New sequence: Straight1 -> Turn1 -> Turn2 -> Turn3 -> Straight2 -> Straight3 -> Done
#define S_STRAIGHT1   0
#define S_TURN1       1
#define S_TURN2       2
#define S_TURN3       3
#define S_STRAIGHT2   4
#define S_STRAIGHT3   5
#define S_DONE        6

int state = S_STRAIGHT1;

void setup(){
  Enes100.begin("Simulator", FIRE, 3, 1116, 8, 9);
  Tank.begin();
  Enes100.println("Straight + three consecutive 90-degree turns (no reverse, no math.h)");
  dist_begin(); // init first straight segment
}

void loop(){
  switch (state){
    case S_STRAIGHT1:
      setMotorsForward(FWD_PWM, FWD_PWM);
      if (dist_reached(SEG1_M)){
        stopMotors();
        delay(STOP_BEFORE_TURN_MS);
        turn_begin(TURN1_DEG);
        state = S_TURN1;
      }
      break;

    case S_TURN1:
      if (turn_step()){
        delay(STOP_AFTER_TURN_MS);         // pause briefly
        turn_begin(TURN2_DEG);             // immediately start next turn
        state = S_TURN2;
      }
      break;

    case S_TURN2:
      if (turn_step()){
        delay(STOP_AFTER_TURN_MS);         // pause briefly
        turn_begin(TURN3_DEG);             // immediately start next turn
        state = S_TURN3;
      }
      break;

    case S_TURN3:
      if (turn_step()){
        delay(STOP_AFTER_TURN_MS);         // short settle before resuming forward
        dist_begin();
        state = S_STRAIGHT2;               // proceed with remaining straights
      }
      break;

    case S_STRAIGHT2:
      setMotorsForward(FWD_PWM, FWD_PWM);
      if (dist_reached(SEG2_M)){
        stopMotors();
        delay(200);
        dist_begin();
        state = S_STRAIGHT3;
      }
      break;

    case S_STRAIGHT3:
      setMotorsForward(FWD_PWM, FWD_PWM);
      if (dist_reached(SEG3_M)){
        stopMotors();
        Enes100.println("Sequence complete.");
        state = S_DONE;
      }
      break;

    case S_DONE:
      stopMotors();
      delay(100);
      return;
  }

  delay(30); // ~33 Hz control loop
}
