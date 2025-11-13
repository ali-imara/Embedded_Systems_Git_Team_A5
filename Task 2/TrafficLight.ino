#include <Arduino.h>

// Car Traffic Light Class
class CarTrafficLight {
  private:
    int greenPin, yellowPin, redPin;
    long greenTime, yellowTime, redTime;
    long previousMillis;
    const long MIN_GREEN_TIME = 10000; // Minimum time before button can trigger change
    
    enum State {GREEN, YELLOW, RED};
    State currentState;
    State previousState;
    
    bool isButtonPressed;
    
  public:
    CarTrafficLight(int g, int y, int r, long gTime, long yTime, long rTime) {
      greenPin = g;
      yellowPin = y;
      redPin = r;
      greenTime = gTime;
      yellowTime = yTime;
      redTime = rTime;
      currentState = GREEN;
      previousState = RED;
      previousMillis = 0;
      isButtonPressed = false;
    }
    
    void begin() {
      pinMode(greenPin, OUTPUT);
      pinMode(yellowPin, OUTPUT);
      pinMode(redPin, OUTPUT);
      switchTo(GREEN);
    }
    
    void update() {
      long currentMillis = millis();
      
      switch (currentState) {
        case GREEN:
          // Full green time passed OR button pressed after minimum time
          if (currentMillis - previousMillis >= greenTime) {
            switchTo(YELLOW);
          }
          else if (isButtonPressed && (currentMillis - previousMillis >= MIN_GREEN_TIME)) {
            switchTo(YELLOW);
          }
          break;
          
        case YELLOW:
          if (currentMillis - previousMillis >= yellowTime) {
            if (previousState == GREEN) {
              switchTo(RED);
            } else {
              switchTo(GREEN);
            }
          }
          break;
          
        case RED:
          if (currentMillis - previousMillis >= redTime) {
            isButtonPressed = false; // Reset button request
            switchTo(YELLOW);
          }
          break;
      }
    }
    
    void requestPedestrianCrossing() {
      isButtonPressed = true;
    }
    
    bool isRed() {
      return currentState == RED;
    }
    
    bool isGreen() {
      return currentState == GREEN;
    }
    
    bool isYellow() {
      return currentState == YELLOW;
    }
    
  private:
    void switchTo(State nextState) {
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, LOW);
      
      previousState = currentState;
      currentState = nextState;
      previousMillis = millis();
      
      switch (nextState) {
        case GREEN:  digitalWrite(greenPin, HIGH);  break;
        case YELLOW: digitalWrite(yellowPin, HIGH); break;
        case RED:    digitalWrite(redPin, HIGH);    break;
      }
    }
};

// Pedestrian Traffic Light Class
class PedestrianTrafficLight {
  private:
    int greenPin, redPin;
    int buttonPin;
    long previousMillis;
    
    enum State {GREEN, RED};
    State currentState;
    
    bool lastButtonState;
    
  public:
    PedestrianTrafficLight(int g, int r, int button) {
      greenPin = g;
      redPin = r;
      buttonPin = button;
      currentState = RED;
      previousMillis = 0;
      lastButtonState = HIGH;
    }
    
    void begin() {
      pinMode(greenPin, OUTPUT);
      pinMode(redPin, OUTPUT);
      pinMode(buttonPin, INPUT_PULLUP);
      switchTo(RED);
    }
    
    void update() {
      // This update just maintains the LED state
      // The actual timing is controlled by the car light's RED phase
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
      switchTo(GREEN);
    }
    
    void setRed() {
      switchTo(RED);
    }
    
    bool isRed() {
      return currentState == RED;
    }
    
    bool isGreen() {
      return currentState == GREEN;
    }
    
  private:
    void switchTo(State nextState) {
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, LOW);
      
      currentState = nextState;
      previousMillis = millis();
      
      switch (nextState) {
        case GREEN: digitalWrite(greenPin, HIGH); break;
        case RED:   digitalWrite(redPin, HIGH);   break;
      }
    }
};

// Pin Definitions
const int CAR_GREEN_PIN = 11;
const int CAR_YELLOW_PIN = 12;
const int CAR_RED_PIN = 13;

const int PED_GREEN_PIN = 8;
const int PED_RED_PIN = 9;
const int PED_BUTTON_PIN = 2;

// Timing Definitions (in milliseconds)
const long T_CAR_GREEN = 30000;  
const long T_CAR_YELLOW = 5000;   
const long T_CAR_RED_TIME = 15000;

// Create instances
CarTrafficLight carLight(CAR_GREEN_PIN, CAR_YELLOW_PIN, CAR_RED_PIN, 
                         T_CAR_GREEN, T_CAR_YELLOW, T_CAR_RED_TIME);
PedestrianTrafficLight pedLight(PED_GREEN_PIN, PED_RED_PIN, PED_BUTTON_PIN);

void setup() {
  carLight.begin();
  pedLight.begin();
}

void loop() {
  // Update car light
  carLight.update();
  
  // Check for button press
  if (pedLight.isButtonPressed()) {
    carLight.requestPedestrianCrossing();
  }
  
  // Sync pedestrian lights with car light state
  if (carLight.isRed()) {
    pedLight.setGreen(); // Car is red = pedestrians can walk
  } else {
    pedLight.setRed();   // Car is green/yellow = pedestrians wait
  }

}
