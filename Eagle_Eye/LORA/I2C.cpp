/**
 * I2C.cpp handles sending of event messages
 * between the LORA and MEGA via I2C protocol.
 */

 
#include <Arduino.h>
#include "I2C.h"
#include "DATA.h"
#include "Globals.h"
#include <stdlib.h>
#include <string.h>
#include <Wire.h>


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
	// LoRa - 1
	Wire.begin();
	// Serial.println("I2C Bus Address Set.");
}


/**
 * Controls what message gets sent and where they go. 
 * Much like an old telephone switchboard operator.
 */
void I2C::manager()
{
  // Clears and fills the network packet to be sent to the Mega.
  create_mega_packet();
  
  // Uploads message to CAN to be delivered to Mega.
  send_mega();
}


/**
 * Builds the message to sent to the Mega via I2^C.
 */
void I2C::create_mega_packet(){

  /**
   *                EXAMPLE OF I2^C PACKET.
   *            
   * $,Altitude, Latitude, Longitude, TargetAlt, TargetLat, TargetLon, TargetDistance, Speed, Time,$
   *      1          2         3          4          5          6            7           8      9
   *             
   */
    
  // Creates a temporary string to hold all need information.
  String temp = "";

  // Each line below appends a certain divider or value to the string.
  temp += '$'; 
  temp += ',';
  temp += Data.Local.Altitude;
  temp += ',';
  temp += Data.Local.Latitude * 10000.0;
  temp += ',';
  temp += Data.Local.Longitude * 10000.0;
  temp += ',';
  temp += Data.Local.TargetAlt;
  temp += ',';
  temp += Data.Local.TargetLat * 10000.0;
  temp += ',';
  temp += Data.Local.TargetLon * 10000.0;
  temp += ',';
  temp += Data.Local.TargetDistance;
  temp += ',';
  temp += Data.Local.Speed;
  temp += ',';
  temp += Data.Local.Time;
  temp += ',';
  temp += '$';

  // Copies over values Offical string.
  i2c_packet = temp;
  
}


/**
 * Sends byte over I2C Connection. (Utilizing the Arduino's CAN)
 */
void I2C::send_mega()
{
  // Iterator
  int character_iterator = 0;
  // Sends the first installment of 32 characters to the Mega. (0-32)
	// Assigns address of the receiving board.
	Wire.beginTransmission(8);
	// Sends the message.
	while(character_iterator<32)
  {
    Wire.write(i2c_packet[character_iterator]);
    character_iterator++;
	}
	// Closes the transmission.
	Wire.endTransmission();
  delay(100);
  
  // Sends the second installment of 32 characters to the Mega. (32-64)
  // Assigns address of the receiving board.
  Wire.beginTransmission(8);
  // Sends the message.
  while(character_iterator<64)
  {
    Wire.write(i2c_packet[character_iterator]);
    character_iterator++;
  }
  // Closes the transmission.
  Wire.endTransmission();
  delay(100);

  // Sends the third installment of 32 characters to the Mega. (64-96)
  // Assigns address of the receiving board.
  Wire.beginTransmission(8);
  // Sends the message.
  while(character_iterator<96)
  {
    Wire.write(i2c_packet[character_iterator]);
    character_iterator++;
  }
  // Closes the transmission.
  Wire.endTransmission();
}
