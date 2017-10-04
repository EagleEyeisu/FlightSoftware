/****LIBRARIES****/
#include <Adafruit_MAX31855.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>
#include <SD.h>

#include "DATA.h"
#include "I2C.h"
#include "IMU.h"
//#include "PARA.h"
#include "SAVE.h"
#include "THERMO.h"
#include "Globals.h"


/*****CONSTRUCTORS*****/ //(Objects that can reference their respective functions & variables)
DATA Data;
I2C Comm;
IMU Imu;
//PARA Para;
SAVE Save;
THERMO Thermo;

//Object used to pull and store the Thermocouple's information.
Adafruit_MAX31855 thermocouple(31, 30, 32);

//Object used to pull and store information from the IMU.
//   Use I2C, ID #1000
Adafruit_LSM9DS0 lsm(1000);

//Object used to communicate and pull information from the BMP085. (Pressure Sensor)
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);


/**
 * Method initializes the main hardware components.
 */
void setup(){

  //Creates a serial communication line between the arduino and the serial port 
  //   found under 'Tools' -> 'Serial Monitor'
  Serial.begin(115200);

  //Initializes the Pressure Sensor.
  Data.initialize();
  
  //Initializes the Parachute and its power relay.
  //Para.initialize();

  //Initializes the SD Card.
  Save.initialize();

  //Initializes the Inter-Intergrated Circuit (I^2C) protocol.
  Comm.initialize();

  //Initializes the Interial Measurement Unit (IMU).
  Imu.initialize();

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
  Imu.manager();
  
  //Repsonsible for Enablement & Deployment of the paracute.
  //Para.manager();

  //Stores the current cycle's data to the SD Card.
  Save.saveData();

  delay(5000);
}
