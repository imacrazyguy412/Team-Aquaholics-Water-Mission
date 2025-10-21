// Define the pins for the ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;


// Define variables for duration and distance
long duration;
float distance;


void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
 
  // Set the trigger pin as an output
  pinMode(trigPin, OUTPUT);
  // Set the echo pin as an input
  pinMode(echoPin, INPUT);
}


void loop() {
  // Send a 10 microsecond pulse to the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Measure the duration of the echo pulse
  duration = pulseIn(echoPin, HIGH);
 
  // Calculate the distance in cm (distance = duration * speed_of_sound / 2)
  // Speed of sound is approximately 0.034 cm/microsecond
  distance = duration * 0.34 / 2 + 18;
 
  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" mm");
 
  // Add a short delay between readings
  delay(100);
}

