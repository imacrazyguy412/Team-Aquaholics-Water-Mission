#include "Arduino.h"
#include "Enes100.h"
#include "Tank.h"

// ================= USER TUNING =================
const int   BASE_PWM        = 160;      // forward speed (try 140–200)
const int   MIN_PWM         = 90;       // enough to overcome stiction
const int   MAX_PWM         = 220;      // don't start at 255; tune later
const int   LEFT_TRIM       = 0;        // + makes left motor faster, - slower
const int   RIGHT_TRIM      = 0;        // + makes right motor faster, - slower

// PID-lite: proportional heading correction (deg -> PWM)
const float KP_HEADING      = 1.2f;     // increase if it drifts; decrease if it wiggles
const int   MAX_CORRECTION  = 60;       // limit how hard we bias a side

// If your Enes100.getTheta() is in degrees, leave true.
// If it’s in radians, set to false and we’ll convert to degrees internally.
const bool  THETA_IS_DEGREES = true;

// Optional: stop after traveling this many meters (set <=0 to run forever)
const float STOP_AFTER_METERS = 0.0f;
// ===============================================

// ---- tiny helpers (no math.h) ----
static inline float absf(float v){ return v < 0 ? -v : v; }
static inline int   clamp01(int v){ return v < 0 ? 0 : (v > 255 ? 255 : v); }
static inline int   imax(int a,int b){ return (a>b)?a:b; }
static inline int   imin(int a,int b){ return (a<b)?a:b; }

// wrap angle error to [-180,180] (degrees)
float wrapDeg(float a){
  while (a >  180.0f) a -= 360.0f;
  while (a < -180.0f) a += 360.0f;
  return a;
}

void setMotorsForward(int leftPWM, int rightPWM){
  // Non-reversing: clamp to 0..255
  Tank.setLeftMotorPWM(clamp01(leftPWM));
  Tank.setRightMotorPWM(clamp01(rightPWM));
}

void stopMotors(){ setMotorsForward(0,0); }

// ---- globals ----
float theta0_deg = 0.0f;
float x0 = 0.0f, y0 = 0.0f;
unsigned long t0 = 0;

void setup() {
  Enes100.begin("Simulator", FIRE, 3, 1116, 8, 9);
  Tank.begin();

  // capture initial pose
  x0 = Enes100.getX();
  y0 = Enes100.getY();
  float th = Enes100.getTheta();
  if (THETA_IS_DEGREES) theta0_deg = th;
  else                  theta0_deg = th * (180.0f / 3.1415926f);

  t0 = millis();

  Enes100.println("Straight-line drive (heading hold, no reverse)");
}

void loop() {
  // read current heading in degrees
  float th   = Enes100.getTheta();
  float th_d = THETA_IS_DEGREES ? th : th * (180.0f / 3.1415926f);

  // heading error relative to start
  float eDeg = wrapDeg(th_d - theta0_deg);

  // proportional correction -> PWM bias (non-reversing)
  int corr = (int)(KP_HEADING * eDeg);
  if (corr >  MAX_CORRECTION) corr =  MAX_CORRECTION;
  if (corr < -MAX_CORRECTION) corr = -MAX_CORRECTION;

  // base forward thrust + manual trim
  int left  = BASE_PWM + LEFT_TRIM;
  int right = BASE_PWM + RIGHT_TRIM;

  // apply correction: if eDeg > 0 we’re rotated left of the target heading,
  // so slow the left side a bit and/or push the right side more.
  left  = left  - corr;
  right = right + corr;

  // keep both sides within limits and above stiction if they’re non-zero
  left  = imin(MAX_PWM, imax(0, left));
  right = imin(MAX_PWM, imax(0, right));

  if (left > 0 && left < MIN_PWM)   left = MIN_PWM;
  if (right > 0 && right < MIN_PWM) right = MIN_PWM;

  // gentle ramp-up for the first second to avoid wheel slip
  unsigned long dt = millis() - t0;
  if (dt < 1000) {
    int ramp = MIN_PWM + (int)((BASE_PWM - MIN_PWM) * (dt / 1000.0f));
    if (left  > ramp) left  = ramp;
    if (right > ramp) right = ramp;
  }

  setMotorsForward(left, right);

  // optional distance-based stop (no sqrt): stop when (x-x0)^2+(y-y0)^2 >= D^2
  if (STOP_AFTER_METERS > 0.0f) {
    float dx = Enes100.getX() - x0;
    float dy = Enes100.getY() - y0;
    float d2 = dx*dx + dy*dy;
    float D2 = STOP_AFTER_METERS * STOP_AFTER_METERS;
    if (d2 >= D2) {
      stopMotors();
      Enes100.println("Stopped: distance reached.");
      while (1) { delay(100); } // hold
    }
  }

  // light telemetry (comment out if noisy)
  // Enes100.print("th0="); Enes100.print(theta0_deg,1);
  // Enes100.print(" th="); Enes100.print(th_d,1);
  // Enes100.print(" e=");  Enes100.print(eDeg,1);
  // Enes100.print(" L=");  Enes100.print(left);
  // Enes100.print(" R=");  Enes100.println(right);

  delay(40); // ~25 Hz loop
}
