/****LIBRARIES****/

//NON EAGLE EYE
#include <RH_RF95.h>
#include <SD.h>
#include <TimeLib.h>

//EAGLE EYE'S
#include "Data.h"
#include "Radio.h"
#include "Save.h"


/**
 * Method initializes the main hardware components.
 */
void setup(){

  //Creates a serial communication line between the arduino and the serial port 
  //   found under 'Tools' -> 'Serial Monitor'
  Serial.begin(115200);

  //Initializes the SD Card.
  SD_Initialize();

  //Initializes the Radio.
  Radio_Initialize();
}


/**
 * MAIN PROGRAM CODE.
 */
void loop(){

  //Responsible for grabbing all of the craft's current information, 
  //   turning that data into an array that can be sent out via radio.
  //   Also reads in incoming messages.
  Radio_Manager();

  //Saves all local data to the SD card.
  Save_Data();

  delay(100);
}
