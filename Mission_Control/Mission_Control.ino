/****LIBRARIES****/

//NON EAGLE EYE
#include <RH_RF95.h>
#include <SD.h>
#include <TimeLib.h>

//EAGLE EYE'S
#include "Globals.h"
#include "Data.h"
#include "Radio.h"
#include "Save.h"


/*****CONSTRUCTORS*****/ //(Objects that can reference their respective functions & variables)
DATA Data;
RADIO Radio;
SAVE Save;

//Directs the radio object to focus on two specific ports.
RH_RF95 rf95(8,7);


/**
 * Method initializes the main hardware components.
 */
void setup(){

  //Creates a serial communication line between the arduino and the serial port 
  //   found under 'Tools' -> 'Serial Monitor'
  Serial.begin(115200);

  //Initializes the SD Card.
  Save.initialize();

  //Initializes the Radio.
  Radio.initialize();
}


/**
 * MAIN PROGRAM CODE.
 */
void loop(){

  //Responsible for grabbing all of the craft's current information, 
  //   turning that data into an array that can be sent out via radio.
  //   Also reads in incoming messages.
  Radio.manager();

  //Saves all local data to the SD card.
  //Save.saveData();

  delay(100);
}
