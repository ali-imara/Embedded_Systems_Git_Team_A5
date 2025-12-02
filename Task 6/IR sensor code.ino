
const int irPin = 4;   // The IR sensor output pin
const int ledPin = 3;  // The LED positive pin

void setup() {
  // Initialize the Serial Monitor for debugging (optional)
  Serial.begin(9600);
  
  // Set the IR pin as Input
  pinMode(irPin, INPUT);
  
  // Set the LED pin as Output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Read the status of the IR sensor
  int sensorStatus = digitalRead(irPin);

  if (sensorStatus == LOW) {
    // If the sensor reads LOW, an object is detected -> Turn LED ON
    digitalWrite(ledPin, HIGH);  
  } 
  else {
    // If the sensor reads HIGH, the way is clear -> Turn LED OFF
    digitalWrite(ledPin, LOW);
  }
  // A small delay for stability
  delay(100);
}
