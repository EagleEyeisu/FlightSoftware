/****LIBRARIES****/

//NON EAGLE EYE
#include "Default_Libraries/SD.h"
#include "Default_Libraries/SoftwareSerial.h"
#include "Default_Libraries/Wire.h"
#include "Default_Libraries/RH_RF95.h"
#include "Default_Libraries/TimeLib.h"

//EAGLE EYE'S
#include "LORA_Libraries/Data.h"
#include "LORA_Libraries/GPS.h"
#include "LORA_Libraries/I2C.h"
#include "LORA_Libraries/Para.h"
#include "LORA_Libraries/Radio.h"
#include "LORA_Libraries/Save.h"


void setup(){

  //Creates a serial communication line between the arduino and the serial port 
  //   found under 'Tools' -> 'Serial Monitor'
  Serial.begin(115200);

  //Initializes the Parachute and its power relay.
  Para_Initialize();

  //Initializes the SD Card.
  SD_Initialize();

  //Initializes the Inter-Intergrated Circuit (I^2C) protocol.
  I2C_Initialize();

  //Initializes the GPS.
  GPS_Initialize();

  //Initializes the Radio.
  Radio_Initialize();
  
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
  GPS_Manager();
  
  //Updates the craft's struct. This pulls data from all periphals,
  //   and network to update to the most current situation.
  Data_Manager();

  //Communicates with the Mega to check for certain events happening
  //   throughout the system.
  I2C_Manager();

  //Watches over the craft's changing situation and responds accordingly
  //   by either arming/deploying or disabling the parachute.
  Para_Manager();

  //Responsible for grabbing all of the craft's current information, 
  //   turning that data into an array that can be sent out via radio.
  //   Also reads in incoming messages.
  Radio_Manager();
  
  //Saves all local data to the SD card.
  Save_Data();
  
}
