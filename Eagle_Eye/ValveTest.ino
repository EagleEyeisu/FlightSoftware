
#define VALVE_PIN_OFF 13
#define VALVE_CTRL 12

void setup() {
  //Connects switch to its respective pin
  pinMode(VALVE_PIN_OFF, OUTPUT);
  pinMode(VALVE_CTRL, OUTPUT);
  
}

void loop() {
  //digitalWrite(VALVE_PIN, LOW);
  //delay(3000);
  digitalWrite(VALVE_PIN_OFF, HIGH);
  digitalWrite(VALVE_CTRL, LOW);
  //delay(3000);
}
