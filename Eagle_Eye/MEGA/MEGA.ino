/****LIBRARIES****/

//NON EAGLE EYE
#include "Default_Libraries/Wire.h"
#include "Default_Libraries/Adafruit_Sensor.h"
#include "Default_Libraries/Adafruit_BMP085_U.h"
#include "Default_Libraries/Adafruit_LSM9DS0.h"
#include "Default_Libraries/Adafruit_Simple_AHRS.h"
#include "Default_Libraries/Servo.h"
#include "Default_Libraries/SD.h"
#include "Default_Libraries/Adafruit_MAX31855.h"
#include "Default_Libraries/SPI.h"

//EAGLE EYE'S
#include "Mega_Libraries/Data.h"
#include "Mega_Libraries/I2C.h"
#include "Mega_Libraries/IMU.h"
#include "Mega_Libraries/Para.h"
#include "Mega_Libraries/Save.h"
#include "Mega_Libraries/Thermo.h"


/**
 * Method initializes the main hardware components.
 */
void setup(){

  //Creates a serial communication line between the arduino and the serial port 
  //   found under 'Tools' -> 'Serial Monitor'
  Serial.begin(115200);

  //Initializes the Pressure Sensor.
  Pressure_Initialize()
  
  //Initializes the Parachute and its power relay.
  Para_Initialize();

  //Initializes the SD Card.
  SD_Initialize();

  //Initializes the Inter-Intergrated Circuit (I^2C) protocol.
  I2C_Initialize();

  //Initializes the Interial Measurement Unit (IMU).
  IMU_Initialize();

  //Initializes the Thermocouple.
  Thermo_Initialize();
}


/**
 * MAIN PROGRAM CODE.
 */
void loop(void){

  //Reads in all needed values from periphals.
  Data_Manager();

  //Decides to Send or Recieve I2C information.
  I2C_Manager();

  //Watches for LoRa to ask for permission to drop.
  IMU_Manager();
  
  //Repsonsible for Enablement & Deployment of the paracute.
  Para_Manager();

  //Stores the current cycle's data to the SD Card.
  Save_Data();

  //Delays for half a second. (500 milliseconds)
  delay(500);
}
