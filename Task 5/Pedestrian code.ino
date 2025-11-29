#include <Arduino.h>

// Pedestrian Traffic Light Class
class PedestrianTrafficLight {
  private:
    int greenPin, redPin;
    int buttonPin;
    bool lastButtonState;
    
  public:
    PedestrianTrafficLight(int g, int r, int button) {
      greenPin = g;
      redPin = r;
      buttonPin = button;
      lastButtonState = HIGH;
    }
    
    void begin() {
      pinMode(greenPin, OUTPUT);
      pinMode(redPin, OUTPUT);
      pinMode(buttonPin, INPUT_PULLUP);
      setRed(); // default state
    }
    
    bool isButtonPressed() {
      bool currentButtonState = digitalRead(buttonPin);
      // Button pressed when it goes LOW (with pull-up)
      if (currentButtonState == LOW && lastButtonState == HIGH) {
        lastButtonState = currentButtonState;
        return true;
      }
      lastButtonState = currentButtonState;
      return false;
    }
    
    void setGreen() {
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH);
    }
    
    void setRed() {
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, HIGH);
    }
};

// Pin Definitions
const int PED_GREEN_PIN = 8;
const int PED_RED_PIN = 9;
const int PED_BUTTON_PIN = 2;

PedestrianTrafficLight pedLight(PED_GREEN_PIN, PED_RED_PIN, PED_BUTTON_PIN);

void setup() {
  pedLight.begin();
  Serial.begin(9600); // start serial communication
}

void loop() {
  // check button and send request
  if (pedLight.isButtonPressed()) {
    Serial.write('P'); // send 'P' to car uC
  }
  
  // check for command from car uC
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    if (command == 'G') {
      pedLight.setGreen(); // car is red, walk
    } 
    else if (command == 'S') {
      pedLight.setRed();   // car is not red, wait
    }
  }
}
