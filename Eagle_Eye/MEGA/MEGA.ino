/****LIBRARIES****/

//NON EAGLE EYE
#include "Default_Libraries/AHRS/Adafruit_Simple_AHRS.h"
#include "Default_Libraries/BMP085/Adafruit_BMP085_U.h"
#include "Default_Libraries/LSM9DS0/Adafruit_LSM9DS0.h"
#include "Default_Libraries/MAX31855/Adafruit_MAX31855.h"
#include "Default_Libraries/SD/src/SD.h"
#include "Default_Libraries/Sensor/Adafruit_Sensor.h"
#include "Default_Libraries/Servo/src/Servo.h"
#include "Default_Libraries/SPI/src/SPI.h"
#include "Default_Libraries/Time/TimeLib.h"
#include "Default_Libraries/Wire/src/Wire.h"

//EAGLE EYE'S
#include "Mega_Libraries/Data/DATA.h"
#include "Mega_Libraries/I2C/I2C.h"
#include "Mega_Libraries/IMU/IMU.h"
#include "Mega_Libraries/Para/PARA.h"
#include "Mega_Libraries/Save/SAVE.h"
#include "Mega_Libraries/Thermo/THERMO.h"


/**
 * Method initializes the main hardware components.
 */
void setup(){

  //Creates a serial communication line between the arduino and the serial port 
  //   found under 'Tools' -> 'Serial Monitor'
  Serial.begin(115200);

  //Initializes the Pressure Sensor.
  Press.initialize()
  
  //Initializes the Parachute and its power relay.
  Para.initialize();

  //Initializes the SD Card.
  Save.initialize();

  //Initializes the Inter-Intergrated Circuit (I^2C) protocol.
  I2C.initialize();

  //Initializes the Interial Measurement Unit (IMU).
  IMU.initialize();

  //Initializes the Thermocouple.
  Thermo.initialize();
}


/**
 * MAIN PROGRAM CODE.
 */
void loop(){

  //Reads in all needed values from periphals.
  Data.manager();

  //Decides to Send or Recieve I2C information.
  Comm.manager();

  //Watches for LoRa to ask for permission to drop.
  IMU.manager();
  
  //Repsonsible for Enablement & Deployment of the paracute.
  Para.manager();

  //Stores the current cycle's data to the SD Card.
  Save.saveData();

  delay(100);
}
