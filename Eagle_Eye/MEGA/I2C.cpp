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
	// Sets the address for the current micro controller.
	// Mega - 0
	// LoRa - 8
	Wire.begin();
}


/**
 * Recieves bytes over I2C Connection. Interrupt method.
 */
void I2C::i2c_receive()
{
    Comm.i2c_packet = "";
    
    Wire.requestFrom(8, 30);
    
    while(Wire.available())
    {
    	char temp = Wire.read();
        // Concatenates character to large string.
        Comm.i2c_packet += temp;
    }

    // Checks for proper formatting.
    if(Comm.i2c_packet[0] == '$')
    {
        // New info is being read in. 
        Data.new_data = Data.YES;
        Comm.complete_packet_flag = true;
    	Comm.to_parse[Comm.i2c_packet.length()];
    	Comm.i2c_packet.toCharArray(Comm.to_parse,Comm.i2c_packet.length());
    	Serial.print("I2C Packet: ");
    	Serial.println(Comm.i2c_packet);
    }
    else
    {
        Comm.complete_packet_flag = false;
    }
}
