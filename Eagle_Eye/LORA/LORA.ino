/****LIBRARIES****/
#include <RH_RF95.h>
#include <TinyGPSPlus.h>

#include "Globals.h"
#include "DATA.h"
#include "GPS.h"
#include "I2C.h"
#include "RADIO.h"


/*****CONSTRUCTORS*****/ //(Objects that can reference their respective functions & variables)
DATA Data;
GPS Gps;
I2C Comm;
RADIO Radio;

// Directs the radio object to focus on two specific ports.
RH_RF95 rf95(8,7);

// Creates an instance of the gps class from TinyGPSPlus.
TinyGPSPlus gps;


/**
 * INITIALIZES ALL REQUIRED PERIPHIALS AND DEPENDENCIES.
 */
void setup(){

    // Creates a serial communication line between the arduino and the serial port 
    // found under 'Tools' -> 'Serial Monitor'
    Serial.begin(115200);

    // Initializes the Inter-Intergrated Circuit (I^2C) protocol.
    Comm.initialize();

    // Initializes the Radio.
    Radio.initialize();
}


/**
 * MAIN PROGRAM CODE.
 */
void loop(){
  
    // Reads in a new NMEA sentence.
    Gps.manager();

    // Communicates with the Mega to check for certain events happening
    // throughout the system.
    Comm.manager();

    // Responsible for grabbing all of the craft's current information, 
    // turning that data into an array, and transmitting the array via radio.
    // Also reads incoming network packets.
    Radio.manager();
}
