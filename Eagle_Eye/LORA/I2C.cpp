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
	// Predeclaration of method that will be set as a interrupt.
    void receiveEvent(int howMany);
	// Sets the address for the current micro controller.
	// Mega - 0
	// LoRa - 8
	Wire.begin(8);
	Wire.onReceive(receiveEvent);
    // Initializes the system to have the LoRa start.
    Comm.i2c_my_turn = false;
    Comm.i2c_packet_set = false;
}


/**
 * Controls what message gets sent and where they go.
 */
void I2C::manager()
{
    Serial.print("Packet check: "); Serial.println(Comm.i2c_packet_set);
    if(Comm.i2c_packet_set == false)
    {
        // Clears and refills the network packet to be sent to the Mega.
        create_packet();
    }
    // Sends the packet over to the Mega. 
    send_packet();
}


/**
 * Recieves bytes over I2C Connection.
 */
void receiveEvent(int howMany)
{
	Comm.i2c_my_turn = true;
    // Resets the input string to null.
    Comm.i2c_input_buffer = "";
    // Resets formatting variables to false.
    bool start_flag = false;
    bool end_flag = false;
    bool junk_flag = false;
    // Checks if there is available i2c input.
    if(Wire.available())
    {
    	// Reads in first ascii int and casts to char.
    	char temp = Wire.read();
    	// First char should be the dollar sign. 
    	// If so, continute. If not, junk it.
    	if(temp == '$')
    	{
    		// Appends character to string.
    		Comm.i2c_input_buffer += temp;
    		// Signals the format was correct in the beginning.
    		start_flag = true;
    		// Cycles until there is no input.
    		while(Wire.available())
		    {
		    	// Reads in next ascii int and casts to char.
		    	char temp = Wire.read();
		    	// Checks for the final '$'. If so, stops recording the rest.
		    	// Prevents duplicating.
		    	if(temp == '$' && junk_flag == false)
		    	{
		    		// Appends character to string.
		    		Comm.i2c_input_buffer += temp;
		    		// Ending format was correct.
		    		end_flag = true;
		    		// Signals to throw away the rest of the packet is there's more input.
		    		junk_flag = true;
		    	}
		    	// End '$' has already been seen. Throw away the rest.
		    	else if (junk_flag == true)
		    	{	
		    		// Reads in i2c input and kills it.
		    		char junk = temp;
		    	}
                // Middle of the packet data. Add to buffer.
                else
                {
                    // Appends character to string.
                    Comm.i2c_input_buffer += temp;
                }
		    }
    	}
    	// Not correct format. Read in it and throw it away.
    	else
    	{
    		// Cycles until there is no input.
    		while(Wire.available())
		    {
		    	// Reads in i2c input and kills it.
		    	char junk = Wire.read();
		    }
    	}
    }

    // Checks for proper formatting. Forces the program to wait for a valid i2c packet
    // prior to trying to parse the data.
    if(start_flag && end_flag)
    {
        // Updates packet flag to true.
        Comm.flag_complete_packet = true;
        Serial.println("Valid Packet.");
        Serial.println(Comm.i2c_input_buffer);
    }
    // Formatting was incorrect. Invalid packet.
    else
    {
        // Packet is screwed up. Alerts the system not to try to pull data from
        // this packet.
        Comm.flag_complete_packet = false;
        Serial.println("Incorrect Packet.");
    }
}


/**
 * Builds the message to sent to the Mega via I2^C.
 */
void I2C::create_packet()
{
   /**
    *                I2^C PACKETS   (LoRA -> MEGA)
    *
    * $,C,current_altitude, current_latitude, current_longitude, current_Speed,$
    *   1        2                 3                 4                5
    * $,T,target_altitude, target_latitude, target_longitude, target_distance,$
    *   1       2                3                 4                 5
    * $,N, authority_mode, target_throttle, manual_direction, craft_anchor,$
    *   1        2                3                 5              5
    */

    // Creates a temporary string to hold all need information.
    i2c_packet = "";
    // Each line below appends a certain divider or value to the string.
    i2c_packet += '$';
    // The three conditional statements alternate to send each of the three packets.
    // 1 = C packet.
    if(i2c_selector == 1)
    {
        // Alerts the system that the packet has been updated for this turn.
        Comm.i2c_packet_set = true;
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
    else if(i2c_selector == 2)
    {
        // Alerts the system that the packet has been updated for this turn.
        Comm.i2c_packet_set = true;
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
    // 2 = N packet.
    else if(i2c_selector == 3)
    {
        // Alerts the system that the packet has been updated for this turn.
        Comm.i2c_packet_set = true;
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
 * Sends byte over I2C Connection.
 */
void I2C::send_packet()
{
    // Every 1 second, the lora is allowed to send i2c data.
    if((millis() - i2c_timer > 50) && (Comm.i2c_my_turn == true))
    {
        // Resets timer.
        i2c_timer = millis();
        // Iterator
        int character_iterator = 0;
        // Assigns address of the receiving board.
        Wire.beginTransmission(0);
        // Sends the message.
        while(character_iterator < i2c_packet.length())
        {
            Wire.write(i2c_packet[character_iterator]);
            character_iterator++;
        }
        Serial.print("Sent: ");
        Serial.println(i2c_packet);
        // Closes the transmission.
        Wire.endTransmission();
        // No longer my turn.
        Comm.i2c_my_turn = false;
        // Allows the next i2c packet to cycle to the next packet type.
        Comm.i2c_packet_set = false;
    }
}
