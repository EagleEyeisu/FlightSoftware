/**
 * I2C.cpp handles sending of event messages
 * between the LORA and MEGA via I2C protocol.
 */

 
#include <Arduino.h>
#include "I2C.h"
#include "DATA.h"
#include "Globals.h"
#include "RADIO.h"
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
	// LoRa - 8
	Wire.begin();
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
    send_mega_packet();
}


/**
 * Builds the message to sent to the Mega via I2^C.
 */
void I2C::create_mega_packet()
{
   /**
    *                I2^C PACKETS   (LoRA -> MEGA)
    *
    * $,C,current_Altitude, current_Latitude, current_Longitude, current_Speed,$
    *   1        2                 3                 4                5
    * $,T,target_Altitude, target_Latitude, target_Longitude, target_distance,$
    *   1       2                3                 4                 5
    * $,N, authority_mode, target_throttle, manual_direction, craft_anchor,$
    *   1        2                3                 5              5
    */

    // Creates a temporary string to hold all need information.
    i2c_packet = "";
    // Each line below appends a certain divider or value to the string.
    i2c_packet += '$';
    i2c_packet += ',';
    // The three conditional statements alternate to send each of the three packets.
    // 1 = C packet.
    // i2c_send_permission (T or F) signifies if its the LoRa or MEGA's turn
    // to send a i2c packet.
    if(i2c_selector == 1 && i2c_send_permission)
    {
    	// Updates the selector back to 2 to switch to the next packet.
    	i2c_selector = 2;
    	// Appends the appropriate variables to fill out the packet.
    	i2c_packet += ',';
    	i2c_packet += 'C';
    	i2c_packet += ',';
    	i2c_packet += Data.Local.current_altitude;
    	i2c_packet += ',';
    	i2c_packet += Data.Local.current_latitude * 10000.0;
    	i2c_packet += ',';
    	i2c_packet += Data.Local.current_longitude * 10000.0;
    	i2c_packet += ',';
    	i2c_packet += Data.Local.current_speed;
    	i2c_packet += ',';
    }
    // 2 = T packet.
    // i2c_send_permission (T or F) signifies if its the LoRa or MEGA's turn
    // to send a i2c packet.
    else if(i2c_selector == 2 && i2c_send_permission)
    {
    	// Updates the selector back to 3 to switch to the next packet.
    	i2c_selector = 3;
    	// Appends the appropriate variables to fill out the packet.
    	i2c_packet += ',';
    	i2c_packet += 'T';
    	i2c_packet += ',';
    	i2c_packet += Data.Local.current_target_altitude;
    	i2c_packet += ',';
    	i2c_packet += Data.Local.current_target_latitude * 10000.0;
    	i2c_packet += ',';
    	i2c_packet += Data.Local.current_target_longitude * 10000.0;
    	i2c_packet += ',';
    	i2c_packet += Data.Local.current_target_distance;
    	i2c_packet += ',';
    }
    // 2 = T packet.
    // i2c_send_permission (T or F) signifies if its the LoRa or MEGA's turn
    // to send a i2c packet.
    else if(i2c_selector == 3 && i2c_send_permission)
    {
    	// Updates the selector back to 1 to complete the cycle.
    	i2c_selector = 1;
    	// Appends the appropriate variables to fill out the packet.
    	i2c_packet += ',';
    	i2c_packet += 'N';
    	i2c_packet += ',';
    	i2c_packet += Radio.Network.authority_mode;
    	i2c_packet += ',';
    	i2c_packet += Radio.Network.target_throttle;
    	i2c_packet += ',';
    	i2c_packet += Radio.Network.manual_direction;
    	i2c_packet += ',';
    	i2c_packet += Radio.Network.craft_anchor;
    	i2c_packet += ',';
    }
    // Completes the packet.
    i2c_packet += '$';
}


/**
 * Sends byte over I2C Connection. (Utilizing the Arduino's CAN)
 */
void I2C::send_mega_packet()
{
	// Every 1 second, the lora is allowed to send i2c data.
	if(millis() - i2c_timer > 500 && i2c_send_permission)
	{
		// Resets timer.
		i2c_timer = millis();
		// Iterator
	    int character_iterator = 0;
	    // Sends the first installment of 32 characters to the Mega. (0-32)
		// Assigns address of the receiving board.
		Wire.beginTransmission(8);
		// Sends the message.
		while(character_iterator < i2c_packet.length())
	    {
	        Wire.write(i2c_packet[character_iterator]);
	        character_iterator++;
		}
   Serial.println(i2c_packet);
		// Closes the transmission.
		Wire.endTransmission();
	    delay(100);
	}
    
  	
  	/*
    // Sends the second installment of 32 characters to the Mega. (32-64)
    // Assigns address of the receiving board.
    Wire.beginTransmission(1);
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
    Wire.beginTransmission(1);
    // Sends the message.
    while(character_iterator<96)
    {
        Wire.write(i2c_packet[character_iterator]);
        character_iterator++;
    }
    // Closes the transmission.
    Wire.endTransmission();
    */
}
