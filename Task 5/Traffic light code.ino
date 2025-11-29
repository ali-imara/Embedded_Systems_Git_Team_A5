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

// Pin Definitions
const int CAR_GREEN_PIN = 11;
const int CAR_YELLOW_PIN = 12;
const int CAR_RED_PIN = 13;

// Timing Definitions
const long T_CAR_GREEN = 30000;  
const long T_CAR_YELLOW = 5000;   
const long T_CAR_RED_TIME = 15000;

CarTrafficLight carLight(CAR_GREEN_PIN, CAR_YELLOW_PIN, CAR_RED_PIN, 
                         T_CAR_GREEN, T_CAR_YELLOW, T_CAR_RED_TIME);

char lastSentState = ' '; 

void setup() {
  carLight.begin();
  Serial.begin(9600); // start serial communication
}

void loop() {
  // Update car light
  carLight.update();
  
  // check for data from pedestrian uC
  if (Serial.available() > 0) {
    char received = Serial.read();
    if (received == 'P') {
      carLight.requestPedestrianCrossing(); // button was pressed
    }
  }

  // send status to pedestrian uC
  // 'G' = car is red (ped go), 'S' = car is moving (ped stop)
  char stateToSend;
  if (carLight.isRed()) {
    stateToSend = 'G';
  } else {
    stateToSend = 'S';
  }
  
  // only send if state changed to avoid flooding serial
  if (stateToSend != lastSentState) {
    Serial.write(stateToSend);
    lastSentState = stateToSend;
  }
}
