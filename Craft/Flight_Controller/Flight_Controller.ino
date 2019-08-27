/****LIBRARIES****/
#include <Adafruit_MAX31855.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>

#include "DATA.h"
#include "I2C.h"
#include "IMU.h"
#include "MOTOR.h"
#include "THERMO.h"
#include "Globals.h"
#include "VALVE.h"

/*****CONSTRUCTORS*****/ 
DATA Data;
I2C Comm;
IMU Imu;
THERMO Thermo;
MOTOR Movement;
VALVE Valve;
 
// Object used to pull and store the Thermocouple's information.
Adafruit_MAX31855 thermocouple(5, 4, 3);
// Object used to pull and store information from the IMU.
// Use I2C, ID #1000
Adafruit_LSM9DS0 lsm(1000);
// Object used to communicate and pull information from the BMP085. (Pressure Sensor)
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);


/**
 * INITIALIZES ALL REQUIRED PERIPHIALS AND DEPENDENCIES.
 */
void setup()
{
    // Creates a serial communication line between the arduino and the serial port 
    // found under 'Tools' -> 'Serial Monitor'
    Serial.begin(115200);
    // Initializes the Pressure Sensor.
    Data.initialize();
    // Initializes the Inter-Intergrated Circuit (I^2C) protocol.
    Comm.initialize();
    // Initializes the Inertial Measurement Unit (IMU).
    Imu.initialize();
    // Initializes all ESC's, TurboFans, & Servos.
    Movement.initialize();
    // Initializes the Valve.
    Valve.initialize();
}


/**
 * MAIN PROGRAM CODE.
 */
void loop()
{
    // Communicates with the LoRa to check for certain events happening
    // throughout the system.
    Comm.manager();
    // Reads in all needed values from periphals.
    Data.update_data();
    // Watches for LoRa to ask for permission to drop.
    Imu.manager();
    // Dynamically updates the orientation & position of the craft. 
    Movement.manager();
   // Manages the logic for the Valve
    Valve.manager();
    // Print data of the current cycle to the screen. Only prints when new
    // information is collected. 
    Data.to_screen();
    // Resets the new_data state to false.
    Data.new_data = Data.NO;
    delay(500);
}
