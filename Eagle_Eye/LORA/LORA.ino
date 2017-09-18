/****LIBRARIES****/

//NON EAGLE EYE
#include "Default_Libraries/SD/src/SD.h"
#include "Default_Libraries/SoftwareSerial/src/SoftwareSerial.h"
#include "Default_Libraries/Wire/src/Wire.h"
#include "Default_Libraries/RadioHead/RH_RF95.h"

//EAGLE EYE'S
#include "Data.h"
#include "GPS.h"
#include "I2C.h"
#include "Para.h"
#include "Radio.h"
#include "Save.h"


//Constructors (Objects that can reference their respective functions)
Data Data;
GPS GPS;
I2C I2C;
Para Para;
Radio Radio;
Save Save;


/**
 * INITIALIZES ALL REQUIRED PERIPHIALS AND DEPENDENCIES.
 */
void setup(){

  //Creates a serial communication line between the arduino and the serial port 
  //   found under 'Tools' -> 'Serial Monitor'
  Serial.begin(115200);

  //Initializes the Parachute and its power relay.
  Para.Initialize();

  //Initializes the SD Card.
  Save.Initialize();

  //Initializes the Inter-Intergrated Circuit (I^2C) protocol.
  I2C.Initialize();

  //Initializes the GPS.
  GPS.Initialize();

  //Initializes the Radio.
  Radio.Initialize();
  
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
  GPS.Manager(Data&, GPS&, I2C&, Para&, Radio&, Save&);
  
  //Updates the craft's struct. This pulls data from all periphals,
  //   and network to update to the most current situation.
  Data.Manager(Data&, GPS&, I2C&, Para&, Radio&, Save&);

  //Communicates with the Mega to check for certain events happening
  //   throughout the system.
  I2C.Manager(false, Data&, GPS&, I2C&, Para&, Radio&, Save&);

  //Watches over the craft's changing situation and responds accordingly
  //   by either arming/deploying or disabling the parachute.
  Para.Manager(Data&, GPS&, I2C&, Para&, Radio&, Save&);

  //Responsible for grabbing all of the craft's current information, 
  //   turning that data into an array that can be sent out via radio.
  //   Also reads in incoming messages.
  Radio.Manager(Data&, GPS&, I2C&, Para&, Radio&, Save&);
  
  //Saves all local data to the SD card.
  Save.Save_Data(Data&, GPS&, I2C&, Para&, Radio&, Save&);
  
}
