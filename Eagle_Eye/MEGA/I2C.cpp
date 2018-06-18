/**
 * I2C.cpp handles sending of event messages
 *    between the LORA and MEGA via I2C protocol.
 */



#include "I2C.h"
#include "DATA.h"
#include "Globals.h"
#include <Wire.h>
#include <Arduino.h>


/**
 * Constructor used to reference all other variables & functions.
 */
I2C::I2C()
{
  
}


/**
 * Assigns the proper address to the current micro controller.
 */
void I2C::initialize()
{
  // Predeclaration of method that will be set as a interrupt.
  void receiveEvent(int howMany);
  
	// Sets the address for the current micro controller.
	// Mega - 0
	// LoRa - 1
	Wire.begin(8);
  
  // Registers recieveEvent as a interrupt.
  Wire.onReceive(receiveEvent);
  
	Serial.println("Comms Address Set.");
}


/**
 * Recieves bytes over I2C Connection.
 */
void receiveEvent(int howmany)
{
  // New info is being read in. 
  Data.newData = Data.YES;

  if(Comm.First8 && Comm.Second8 && Comm.Third8)
  {
    Comm.NDP = "";
    Comm.First8 = false;
    Comm.Second8 = false;
    Comm.Third8 = false;
  }
  
  // Checks for data on the wire.
  while(Wire.available())
  {
    // Reads in single character from serial port.
    char character = Wire.read();
    Comm.NDP += character;
  }
  
  if(!Comm.First8)
  {
    Comm.First8 = true;
  }
  else if(!Comm.Second8)
  {
    Comm.Second8 = true;
  }
  else if(!Comm.Third8)
  {
    Comm.Third8 = true;
  }
  
}
