// Define Pin Connections
const int trigPin = 5;  // Trig sends the sound wave
const int echoPin = 4;  // Echo receives the sound wave
const int ledPin = 3;   // LED positive pin

// Define variables for calculations
long duration;
int distance;

// The LED turns ON if an object is closer than this value.
const int distanceThreshold = 20; 

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  
  // Set Pin Modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 1. Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // 2. Send a 10 microsecond pulse to trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // 3. Read the echoPin (measures how long the pin stays HIGH)
  // pulseIn returns the duration in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // 4. Calculate the distance
  // Speed of sound is 0.034 cm/us
  distance = duration * 0.034 / 2;
  
  // 5. Logic to control LED
  if (distance > 0 && distance < distanceThreshold) {
    digitalWrite(ledPin, HIGH);
    Serial.print("Object Detected! Distance: ");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.print("Clear. Distance: ");
  }
  
  // Print distance to Serial Monitor for debugging
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(100); // Small delay for stability
}
