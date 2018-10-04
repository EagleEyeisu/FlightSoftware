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
	// LoRa - 8
	Wire.begin();
}


/**
 * Recieves bytes over I2C Connection.
 */
void I2C::i2c_receive()
{
    Comm.i2c_packet = "";
    
    while(Wire.available())
    {
    	char temp = Wire.read();
        Serial.print(temp);
        // Concatenates character to large string.
        Comm.i2c_packet += temp;
    }
    Serial.println();

    // Checks for proper formatting.
    if(Comm.i2c_packet[0] == '$')
    {
        Comm.complete_packet_flag = true;
    	Comm.to_parse[Comm.i2c_packet.length()];
        Serial.print("Packet Length: ");
        Serial.println(packet_length);
    	Comm.i2c_packet.toCharArray(Comm.to_parse,Comm.i2c_packet.length());
    	Serial.print("I2C Packet: ");
    	Serial.println(Comm.i2c_packet);
    }
    else
    {
        Comm.complete_packet_flag = false;
    }
}
