#include <Arduino.h>

class TrafficLight {
  private:
    // Car Light Pins
    int greenPin, yellowPin, redPin;
    // Pedestrian Light Pins
    int pedGreenPin, pedRedPin;
    // Pedestrian Button Pin
    int buttonPin;

    // Timings (in milliseconds)
    long greenTime, yellowTime, redTime;
    
    // Minimum time the car green must stay on before checking button request
    const long MIN_GREEN_TIME = 10000; 
    
    long previousMillis;
    // Variable to track button press state 
    bool isButtonPressed = false; 
    
    // Traffic Light States 
    enum State {
      GREEN,             
      YELLOW,           
      RED                
    };
    State currentState;
    State previousState;

  public:
    // Constructor initializes all pins and timing durations
    TrafficLight(int g, int y, int r, int pg, int pr, int b,
                 long gTime, long yTime, long rTime) {
      greenPin = g; yellowPin = y; redPin = r;
      pedGreenPin = pg; pedRedPin = pr;
      buttonPin = b;

      greenTime = gTime; yellowTime = yTime; redTime = rTime; 

      currentState = GREEN; // Start in Car Green state
      previousMillis = 0;
    }

    void begin() {
      // Set pin modes
      pinMode(greenPin, OUTPUT);
      pinMode(yellowPin, OUTPUT);
      pinMode(redPin, OUTPUT);
      pinMode(pedGreenPin, OUTPUT);
      pinMode(pedRedPin, OUTPUT);
      // Button input uses internal pull-up (LOW when pressed)
      pinMode(buttonPin, INPUT_PULLUP);

      switchTo(GREEN);
    }

    void update() {
      long currentMillis = millis();

      // Read button state: If pressed (LOW), set the flag ONCE
      if (digitalRead(buttonPin) == LOW) {
        isButtonPressed = true; 
      }

      // State machine logic
      switch (currentState) {
        
        case GREEN:
          // Has the full scheduled greenTime passed? 
          if (currentMillis - previousMillis >= greenTime) {
            switchTo(YELLOW);
          }
          // Has MIN_GREEN_TIME passed AND the button been pressed?
          else if (isButtonPressed && (currentMillis - previousMillis >= MIN_GREEN_TIME)) {
            // If the button is pressed AND the minimum time has been met, transition now.
            switchTo(YELLOW);
          }
          break;

        case YELLOW:
          if (currentMillis - previousMillis >= yellowTime) {
            // YELLOW always transitions to RED from GREEN, and to GREEN from RED
            if (previousState == GREEN) {
                // From GREEN -> go to RED (The Car Red/Ped Green phase)
                switchTo(RED);
            } else {
                // From RED -> go back to GREEN
                // Note: The RED state resets the isButtonPressed flag before transitioning here
                switchTo(GREEN);
            }
          }
          break;

        case RED:
          // *** Car Red, Ped Green is ON ***
          if (currentMillis - previousMillis >= redTime) {
            // Red time is complete. Reset the button press flag before switching back to GREEN
            isButtonPressed = false; 
            switchTo(YELLOW); // Transition to YELLOW before GREEN
          }
          break;
      }
    }

  private:
    void switchTo(State nextState) {
      // Turn off all lights
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(pedGreenPin, LOW);
      digitalWrite(pedRedPin, LOW);
      
      //  Update state and timers
      previousState = currentState;
      currentState = nextState;
      previousMillis = millis();
      
      // Turn on the lights for the new state
      switch (nextState) {
        case GREEN:
          digitalWrite(greenPin, HIGH);
          digitalWrite(pedRedPin, HIGH); // Pedestrian is RED (Car is GREEN)
          break;

        case YELLOW:
          digitalWrite(yellowPin, HIGH);
          digitalWrite(pedRedPin, HIGH); // Pedestrian is RED (Car is YELLOW)
          break;

        case RED:
          // Car Red is ON, Ped Green is ON. (crossing time)
          digitalWrite(redPin, HIGH);
          digitalWrite(pedGreenPin, HIGH); 
          break;
      }
    }
};


// Pin Definitions
const int CAR_GREEN_PIN = 11;
const int CAR_YELLOW_PIN = 12;
const int CAR_RED_PIN = 13;

const int PED_GREEN_PIN = 8;
const int PED_RED_PIN = 9;
const int PED_BUTTON_PIN = 2; // Connect button to pin 2 and GND Ya Ali

// Timing Definitions (in milliseconds)
const long T_CAR_GREEN = 30000;  
const long T_CAR_YELLOW = 5000;   
const long T_CAR_RED_TIME = 15000; // Car Red duration (which is also the Pedestrian Green time)

// Instantiate the traffic light controller
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
