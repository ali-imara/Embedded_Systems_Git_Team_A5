//Pin Definitions
const int  green_pin = 11;
const int  yellow_pin = 12;
const int red_pin = 13;
//Color Runing Time (ms)
const long green_time = 20000;
const long yellow_time = 5000;
const long red_time = 20000;
//Trafficlight States
enum TrafficLightState {green_state, yellow_state, red_state};
int CurrentState = green_state;
long previousMillis = 0;

void setup(){
  pinMode(green_pin, OUTPUT);
  pinMode(yellow_pin, OUTPUT);
  pinMode(red_pin, OUTPUT);

  digitalWrite(green_pin, HIGH);
  previousMillis = millis();
}

void loop(){
  long currentMillis= millis();

  switch(CurrentState){
    case green_state:
      if(currentMillis-previousMillis >= green_time){
        previousMillis = currentMillis;
        digitalWrite(green_pin, LOW);
        digitalWrite(yellow_pin, HIGH);
        CurrentState = yellow_state;
      }
      break;
    case yellow_state:
      if(currentMillis-previousMillis >= yellow_time){
        previousMillis = currentMillis;
        digitalWrite(yellow_pin, LOW);
        digitalWrite(red_pin, HIGH);
        CurrentState = red_state;
      }
      break;
    case red_state:
      if(currentMillis-previousMillis >= red_time){
        previousMillis = currentMillis;
        digitalWrite(red_pin, LOW);
        digitalWrite(green_pin, HIGH);
        CurrentState = green_state;
      }
      break;
  } 
}