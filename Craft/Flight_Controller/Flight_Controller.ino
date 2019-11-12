/****LIBRARIES****/
#include <Adafruit_MAX31855.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>
#include <SoftwareSerial.h>

#include "DATA.h"
#include "UART.h"
#include "IMU.h"
#include "MOTOR.h"
#include "THERMO.h"
#include "Globals.h"

/*****CONSTRUCTORS*****/ 
DATA Data;
UART Comm;
IMU Imu;
THERMO Thermo;
MOTOR Movement;

// Object used to pull and store the Thermocouple's information.
Adafruit_MAX31855 thermocouple(5, 4, 3);
// Object used to pull and store information from the IMU.
// Use UART, ID #1000
Adafruit_LSM9DS0 lsm(1000);
// Defines a serial line (uart rx tx) over a pair of digital pins.
// (RX,TX)
SoftwareSerial ss(13, 12);

/**
 * INITIALIZES ALL REQUIRED PERIPHIALS AND DEPENDENCIES.
 */
void setup()
{
    // Creates a serial communication line between the arduino and the serial port 
    // found under 'Tools' -> 'Serial Monitor'
    Serial.begin(115200);
    // Initializes the Pressure Sensor.
    //Data.initialize();
    // Initializes the Inter-Intergrated Circuit (I^2C) protocol.
    Comm.initialize();
    // Initializes the Inertial Measurement Unit (IMU).
    Imu.initialize();
    // Initializes all ESC's, TurboFans, & Servos.
    Movement.initialize();
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
    // Print data of the current cycle to the screen. Only prints when new
    // information is collected. 
    Data.to_screen();
    // Resets the new_data state to false.
    Data.new_data = Data.NO;
    delay(500);
}
