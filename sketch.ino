#include <Arduino.h>

const int CAR_GREEN_PIN = 11;
const int CAR_YELLOW_PIN = 12;
const int CAR_RED_PIN = 13;
const int PED_GREEN_PIN = 8; 
const int PED_RED_PIN = 9;   
const int PED_BUTTON_PIN = 10; // ****i have changed this pin from pin2 to pin 10  for the display wiring********

// display Segment Pins 
const int SEGMENT_PINS[7] = {0, 1, 2, 3, 4, 5, 6}; 

// --- new timeing to fit  ---
const long T_CAR_GREEN = 10000;
const long MIN_GREEN_TIME = 5000;
const long T_CAR_YELLOW = 4000;
const long T_CAR_RED_TIME = 7000; 

//look up pattern table for the didgits to be dispalyed
const int DIGIT_PATTERNS[10][7] = {
      {1, 1, 1, 1, 1, 1, 0},
      {0, 1, 1, 0, 0, 0, 0},
      {1, 1, 0, 1, 1, 0, 1}, 
      {1, 1, 1, 1, 0, 0, 1},
      {0, 1, 1, 0, 0, 1, 1},
      {1, 0, 1, 1, 0, 1, 1}, 
      {1, 0, 1, 1, 1, 1, 1}, 
      {1, 1, 1, 0, 0, 0, 0}, 
      {1, 1, 1, 1, 1, 1, 1}, 
      {1, 1, 1, 1, 0, 1, 1}
};

class TrafficLight {
private:
    int greenPin, yellowPin, redPin;
    int pedGreenPin, pedRedPin;
    int buttonPin;

    long greenTime, yellowTime, redTime;
    const long MIN_GREEN_TIME; 
    long previousMillis;
    long waitStartTime = 0;
    long maxWaitDuration = 0;
    bool isButtonPressed = false; 
    enum State { GREEN, YELLOW, RED };
    State currentState;
    State previousState;

    // Private display Helper Methods
    void displayDigit(int digit) {
        if (digit < 0 || digit > 9) {
            for (int i = 0; i < 7; i++) digitalWrite(SEGMENT_PINS[i], LOW);
            return;
        }
        for (int i = 0; i < 7; i++) {
            digitalWrite(SEGMENT_PINS[i], DIGIT_PATTERNS[digit][i]);
        }
    }
    void clearDisplay() {
        for (int i = 0; i < 7; i++) {
            digitalWrite(SEGMENT_PINS[i], LOW);
        }
    }


public:
    TrafficLight(int g, int y, int r, int pg, int pr, int b,
                 long gTime, long yTime, long rTime) : MIN_GREEN_TIME(5000) {
      greenPin = g;
      yellowPin = y;
      redPin = r;
      pedGreenPin = pg;
      pedRedPin = pr;
      buttonPin = b;
      greenTime = gTime;
      yellowTime = yTime;
      redTime = rTime; 
      currentState = GREEN;
      previousMillis = 0;
    }

    void begin() {
      // Set pin modes for lights, button, and SSD
      pinMode(greenPin, OUTPUT); pinMode(yellowPin, OUTPUT); pinMode(redPin, OUTPUT);
      pinMode(pedGreenPin, OUTPUT); pinMode(pedRedPin, OUTPUT); 
      pinMode(buttonPin, INPUT_PULLUP);

      for (int i = 0; i < 7; i++) {
          pinMode(SEGMENT_PINS[i], OUTPUT);
      }
      switchTo(GREEN);
    }

    void update() {
      long currentMillis = millis();

      // Read button state
      if (digitalRead(buttonPin) == LOW && !isButtonPressed) {
        // Button pressed for the FIRST TIME
        isButtonPressed = true;
        //*** 
        // timing logic for the display
        //***
        if (currentState == GREEN) {
            long elapsedTimeInGreen = currentMillis - previousMillis;
            long remainingCarGreenTime = greenTime - elapsedTimeInGreen;
            if (elapsedTimeInGreen < MIN_GREEN_TIME) {
                remainingCarGreenTime = MIN_GREEN_TIME - elapsedTimeInGreen;
            } else {
                remainingCarGreenTime = 0; 
            }
            maxWaitDuration = remainingCarGreenTime + yellowTime;
            waitStartTime = currentMillis;
        }
      }

      // --- countdown display logic ---
      if (isButtonPressed && currentState != RED) { 
          long elapsedTime = currentMillis - waitStartTime;
          long remainingTime = maxWaitDuration - elapsedTime;
          int secondsLeft = (remainingTime / 1000) + 1; 
          
          if (remainingTime > 0 && secondsLeft >= 1) {
              displayDigit(min(secondsLeft, 9)); 
          } else {
              clearDisplay();
          }
      } else {
          clearDisplay();
      }



      // State machine logic
      switch (currentState) {
        
        case GREEN:
          if (currentMillis - previousMillis >= greenTime || 
             (isButtonPressed && (currentMillis - previousMillis >= MIN_GREEN_TIME))) {
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
          // countdown is finished
          if (currentMillis - previousMillis >= redTime) {
            isButtonPressed = false; 
            maxWaitDuration = 0; // Reset countdown duration
            switchTo(YELLOW);
          }
          break;
      }
    }

private:
    void switchTo(State nextState) {
      // Turn off all lights
      digitalWrite(greenPin, LOW); digitalWrite(yellowPin, LOW); digitalWrite(redPin, LOW);
      digitalWrite(pedGreenPin, LOW); digitalWrite(pedRedPin, LOW);
      
      previousState = currentState;
      currentState = nextState;
      previousMillis = millis();
      
      // Turn on the lights for the new state
      switch (nextState) {
        case GREEN:
          digitalWrite(greenPin, HIGH);
          digitalWrite(pedRedPin, HIGH); 
          break;

        case YELLOW:
          digitalWrite(yellowPin, HIGH);
          digitalWrite(pedRedPin, HIGH); 
          break;

        case RED:
          // Countdown is finished, pedestrian can cross
          digitalWrite(redPin, HIGH);
          digitalWrite(pedGreenPin, HIGH); 
          break;
      }
    }
};


TrafficLight controller(
  CAR_GREEN_PIN, CAR_YELLOW_PIN, CAR_RED_PIN,
  PED_GREEN_PIN, PED_RED_PIN, PED_BUTTON_PIN,
  T_CAR_GREEN, T_CAR_YELLOW, T_CAR_RED_TIME
);

void setup() {
  controller.begin();
}

void loop() {
  controller.update();
}