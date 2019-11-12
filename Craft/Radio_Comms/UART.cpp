/**
 * UART.cpp handles sending of event messages
 * between the LORA and MEGA via UART protocol.
 */

 
#include <Arduino.h>
#include "UART.h"
#include "DATA.h"
#include "Globals.h" 
#include "RADIO.h"
#include "GPS.h"
#include <stdlib.h>
#include <string.h>


/**
 * Constructor used to reference all other variables & functions.
 */
UART::UART()
{
	
}


/**
 * Assigns the proper address to the current micro controller.
 */
void UART::initialize()
{
    // Predeclaration of method that will be set as a interrupt.
    void receiveEvent(int howMany);
    // Sets the address for the current micro controller.
    // Flight Controller - 0
    // Radio - 8
    Wire.begin(8);
    Wire.onReceive(receiveEvent);
    // Initializes the system to have the LoRa start.
    Comm.uart_my_turn = false;
    Comm.uart_packet_set = false;
}


/**
 * Controls what message gets sent and where they go.
 */
void UART::manager()
{
    // Checks for turn to send.
    if(Comm.uart_my_turn == true)
    {
        // Clears and refills the network packet to be sent to the Mega.
        Comm.create_packet();
        // Sends the packet over to the Mega. 
        Comm.send_packet();
    }
}


/**
 * Recieves bytes over UART Connection.
 */
void receiveEvent(int howMany)
{
    Comm.uart_my_turn = true;
    // Resets the input string to null.
    Comm.uart_input_buffer = "";
    // Checks if there is available UART input.
    if(Wire.available())
    {
        // Reads in first ascii int and casts to char.
        char temp = Wire.read();
        // First char should be the dollar sign. 
        // If so, continute. If not, junk it.
        if(temp == '$')
        {
            // Appends character to string.
            Comm.uart_input_buffer += temp;
            // Cycles until there is no input.
            while(Wire.available())
            {
                // Reads in next ascii int and casts to char.
                temp = Wire.read();
                // Checks for the final '$'. If so, stops recording the rest.
                // Prevents duplicating.
                if(temp == '$')
                {
                    // Appends character to string.
                    Comm.uart_input_buffer += temp;
                    while(Wire.available())
                    {
                        // Throws away bad UART data.
                        char junk = Wire.read();
                    }
                    Comm.flag_complete_packet = true;
                    Serial.println("Valid");
                    return;
                }
                // Otherwise appends character to string.
                Comm.uart_input_buffer += temp;
            }
        }
        // Incorrect format. Read in it and throw it away.
        else
        {
            // Cycles until there is no input.
            while(Wire.available())
            {
                // Throws away bad UART data.
                char junk = Wire.read();
            }
        }
    } 
}


/**
 * Builds the message to sent to the Flight Controller via I2^C.
 */
void UART::create_packet()
{
   /**
    *                I2^C PACKETS   (Radio -> Flight Controller)
    *
    * $,C,current_altitude, current_latitude, current_longitude, current_Speed,$
    *   1        2                 3                 4                5
    * $,T,target_altitude, target_latitude, target_longitude, target_distance,$
    *   1       2                3                 4                 5
    * $,N, authority_mode, target_throttle, manual_direction, craft_anchor,$
    *   1        2                3                 5              5
    */

    //Serial.print("Packet check: "); Serial.println(Comm.uart_packet_set);
    if(Comm.uart_packet_set == false)
    {
        // Creates a temporary string to hold all need information.
        uart_packet = "";
        // Each line below appends a certain divider or value to the string.
        uart_packet += '$';
        // The three conditional statements alternate to send each of the three packets.
        // 1 = C packet. (Current positioning)
        if(uart_selector == 1)
        {
            // Alerts the system that the packet has been updated for this turn.
            Comm.uart_packet_set = true;
        	// Updates the selector back to 2 to switch to the next packet.
        	uart_selector = 2;
        	// Appends the appropriate variables to fill out the packet.
        	uart_packet += ',';
        	uart_packet += 'C';
        	uart_packet += ',';
        	uart_packet += Gps.craft_altitude;
        	uart_packet += ',';
        	uart_packet += Gps.craft_latitude * 10000.0;
        	uart_packet += ',';
        	uart_packet += Gps.craft_longitude * 10000.0;
        	uart_packet += ',';
        	uart_packet += Gps.craft_speed;
        	uart_packet += ',';
        }
        // 2 = T packet. (Target positioning)
        else if(uart_selector == 2)
        {
            // Alerts the system that the packet has been updated for this turn.
            Comm.uart_packet_set = true;
        	// Updates the selector back to 3 to switch to the next packet.
        	uart_selector = 3;
        	// Appends the appropriate variables to fill out the packet.
        	uart_packet += ',';
        	uart_packet += 'T';
        	uart_packet += ',';
        	uart_packet += Gps.craft_target_altitude;
        	uart_packet += ',';
        	uart_packet += Gps.craft_target_latitude * 10000.0;
        	uart_packet += ',';
        	uart_packet += Gps.craft_target_longitude * 10000.0;
        	uart_packet += ',';
        	uart_packet += Gps.craft_distance;
        	uart_packet += ',';
        }
        // 2 = N packet. (Network commands)
        else if(uart_selector == 3)
        {
            // Alerts the system that the packet has been updated for this turn.
            Comm.uart_packet_set = true;
        	// Updates the selector back to 1 to complete the cycle.
        	uart_selector = 1;
        	// Appends the appropriate variables to fill out the packet.
        	uart_packet += ',';
        	uart_packet += 'N';
        	uart_packet += ',';
        	uart_packet += Radio.authority_mode;
        	uart_packet += ',';
        	uart_packet += Radio.target_throttle;
        	uart_packet += ',';
        	uart_packet += Radio.manual_direction;
        	uart_packet += ',';
        	uart_packet += Radio.craft_anchor;
        	uart_packet += ',';
        }
        // Completes the packet.
        uart_packet += '$';
    }
}


/**
 * Sends byte over UART Connection.
 */
void UART::send_packet()
{  
      // Iterator
      int character_iterator = 0;
      // Assigns address of the receiving board.
      Wire.beginTransmission(0);
      // Sends the message.
      while(character_iterator < uart_packet.length())
      {
          Wire.write(uart_packet[character_iterator]);
          character_iterator++;
      }
      Serial.print("Sent: ");
      Serial.println(uart_packet);
      // No longer my turn.
      Comm.uart_my_turn = false;
      // Allows the next UART packet to cycle to the next packet type.
      Comm.uart_packet_set = false;
}
