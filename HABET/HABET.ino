/****LIBRARIES****/

//NON EAGLE EYE
#include "SD.h"
#include "RH_RF95.h"
#include "Servo.h"

//EAGLE EYE'S
#include "Data.h"
#include "Detachment.h"
#include "Radio.h"


void setup() {
  
  //Creates a serial communication line between the arduino and the serial port 
  //   found under 'Tools' -> 'Serial Monitor'
  Serial.begin(115200);
  
  //Initializes the onbaord radio.
  Radio_Initialize();
  
  //Initializes the servo.
  Servo_Initialize();
}


/**
 * MAIN PROGRAM CODE.
 */
void loop() {
  
  //Responsible for reading in radio transmissions and repsonding 
  //   with the appropriate output.
  Radio_Manager();
  
  //Passively watches for the LoRa Event Number or the Command Sent varaible
  //   to prompt the detachment of Eagle Eye from HABET.
  Servo_Manager();

  delay(100);
}
