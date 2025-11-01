#include <arduino.h>

class TrafficLight {
//main program only interacts with begin() and update()
  private:
    int greenPin, yellowPin, redPin;
    long greenTime, yellowTime, redTime;
    long previousMillis;
//traffic light states
    enum State {GREEN, YELLOW, RED};
    State currentState;
    State previousState;

  public:
    // every traffic light colour duration can be set indvidually
  // pin definitons can be set individually
    TrafficLight(int g, int y, int r, long gTime, long yTime, long rTime) {
      greenPin = g;
      yellowPin = y;
      redPin = r;
      greenTime = gTime;
      yellowTime = yTime;
      redTime = rTime;
      currentState = GREEN;
      previousState = RED;  // So first yellow knows what came before
      previousMillis = 0;
    }

    void begin() {
      pinMode(greenPin, OUTPUT);
      pinMode(yellowPin, OUTPUT);
      pinMode(redPin, OUTPUT);
      turnOn(greenPin);
      previousMillis = millis();
    }

    void update() {
      long currentMillis = millis();
//after red or green defaults to yellow,but after yellow checks previous state and act accordingly
      switch (currentState) {
        case GREEN:
          if (currentMillis - previousMillis >= greenTime) {
            switchTo(YELLOW);
          }
          break;

        case RED:
          if (currentMillis - previousMillis >= redTime) {
            switchTo(YELLOW);
          }
          break;

        case YELLOW:
          if (currentMillis - previousMillis >= yellowTime) {
            if (previousState == GREEN) switchTo(RED);
            else if (previousState == RED) switchTo(GREEN);
          }
          break;
      }
    }

  private:
    void switchTo(State nextState) {
      // Turn off all lights first
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, LOW);

      previousState = currentState;
      currentState = nextState;
      previousMillis = millis();

      // Turn on the LED for the new state
      switch (nextState) {
        case GREEN:  turnOn(greenPin);  break;
        case YELLOW: turnOn(yellowPin); break;
        case RED:    turnOn(redPin);    break;
      }
    }
//turn on function
    void turnOn(int pin) {
      digitalWrite(pin, HIGH);
    }
};
//START
TrafficLight light(11, 12, 13, 20000, 5000, 20000);

void setup() {
  light.begin();
}

void loop() {
  light.update();
}
  } 

}
