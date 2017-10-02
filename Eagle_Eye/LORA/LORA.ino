/****LIBRARIES****/
#include <RH_RF95.h>
#include <TinyGPSPlus.h>
#include "Globals.h"
#include "DATA.h"
#include "GPS.h"
#include "I2C.h"
#include "PARA.h"
#include "RADIO.h"
#include "SAVE.h"


/*****CONSTRUCTORS*****/ //(Objects that can reference their respective functions & variables)
DATA Data;
GPS Gps;
I2C Comm;
PARA Para;
RADIO Radio;
SAVE Save;

//Directs the radio object to focus on two specific ports.
RH_RF95 rf95(8,7);

//Creates an instance of the gps class from TinyGPSPlus.
TinyGPSPlus gps;


/**
 * INITIALIZES ALL REQUIRED PERIPHIALS AND DEPENDENCIES.
 */
void setup(){

  //Creates a serial communication line between the arduino and the serial port 
  //   found under 'Tools' -> 'Serial Monitor'
  Serial.begin(115200);

  //Initializes the Parachute and its power relay.
  //Para.initialize();

  //Initializes the SD Card.
  //Save.initialize();

  //Initializes the Inter-Intergrated Circuit (I^2C) protocol.
  //Comm.initialize();

  //Initializes the Radio.
  //Radio.initialize();
  
}


/**
 * Handles Event Logging. Sends MEGA milestone updates/errors.
 *  LORA EVENTS
 *  0 - Chute Disabled
 *  1 - Chute Enabled
 *  2 - Chute Deployed
 *  3 - GPS Offline
 *  4 - Detached
 *  5 - Abort Detach
 *  6 - Radio Connection Lost
 *  7 - TouchDown
 *  8 - Switch Communication Direction
 *  9 - Mega's Orientation GO for drop.
 */
void loop(){
  
  //Reads in a new NMEA sentence.
  Gps.manager();
  
  //Updates the craft's struct. This pulls data from all periphals,
  //   and network to update to the most current situation.
  Data.manager();

  //Communicates with the Mega to check for certain events happening
  //   throughout the system.
  //Comm.manager(false);

  //Watches over the craft's changing situation and responds accordingly
  //   by either arming/deploying or disabling the parachute.
  //Para.manager();

  //Responsible for grabbing all of the craft's current information, 
  //   turning that data into an array that can be sent out via radio.
  //   Also reads in incoming messages.
  //Radio.manager();
  
  //Saves all local data to the SD card.
  //Save.saveData();

  delay(4000);
}
