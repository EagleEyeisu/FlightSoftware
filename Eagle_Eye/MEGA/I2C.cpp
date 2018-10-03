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
	// Mega - 1
	// LoRa - 0
	Wire.begin(1);
    // Registers recieveEvent as a interrupt.
    Wire.onReceive(receiveEvent);
	Serial.println("Comms Address Set.");
}


/**
 * Recieves bytes over I2C Connection. Interrupt method.
 */
void receiveEvent(int howmany)
{
    // New info is being read in. 
    Data.new_data = Data.YES;

    if(Comm.first_32 && Comm.second_32 && Comm.third_32)
    {
        Comm.to_parse[Comm.i2c_packet.length()];
        Comm.i2c_packet.toCharArray(Comm.to_parse,Comm.i2c_packet.length());
        Serial.print("Packet: ");
        Serial.println(Comm.i2c_packet);
        Comm.i2c_packet = "";
        Comm.first_32 = false;
        Comm.second_32 = false;
        Comm.third_32 = false;
    }

    // Checks for data on the wire.
    while(Wire.available())
    {
        // Reads in single character from serial port.
        char temp_character = Wire.read();
        // Concatenates character to large string.
        Comm.i2c_packet += temp_character;
    }
    // This series of conditional checks will make sure that the 3 seperate
    // i2c packets are read in (in order) before letting the mega use the data.
    // They are all acting as flags and are reset upon the start of the next packet.
    if(!Comm.first_32)
    {
        Comm.first_32 = true;
    }
    else if(!Comm.second_32)
    {
        Comm.second_32 = true;
    }
    else if(!Comm.third_32)
    {
        Comm.third_32 = true;
    }
}
