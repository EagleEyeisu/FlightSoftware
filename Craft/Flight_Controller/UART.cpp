/**
 * UART.cpp handles sending of event messages
 *    between the LORA and MEGA via UART protocol.
 */


#include "UART.h"
#include "DATA.h" 
#include "IMU.h"
#include "MOTOR.h"
#include "Globals.h"
#include <Wire.h>
#include <Arduino.h>


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
    ss.begin(9600);
   	// Initializes the system to have the fltcontroller talk first.
    Comm.uart_my_turn = true;
}


/**
 * Controls what message gets sent and where they go. 
 */
void UART::manager()
{
    // Clears and refills the network packet to be sent to the LoRa.
    create_packet();
    // Checks and handles the deadlock scenario.
    deadlock_monitor();
    // Sends the packet over to the LoRa.
    send_packet();
}


/**
 * Monitors for UART deadlock by monitoring how many cycles
 * have passed by without hearing from the LoRa device.
 */
void UART::deadlock_monitor()
{
    // 5 cycles have passed since last contact with Arduino LoRa.
    if(Comm.uart_deadlock_counter >= 5)
    {   
        // Deadlock detected. Turning true will trigger rebroadcast
        // the next time send_packet is called.
        Comm.uart_my_turn = true;
        // Reset deadlock counter to 0.
        Comm.uart_deadlock_counter = 0;
    }
    // Deadlock not detected yet. Checks if the packet has been set.
    // If this packet set status is true, we iterate the deadlock
    // counter.
    else if(Comm.uart_packet_set)
    {
        // Packet set status is true. Increment deadlock counter.
        Comm.uart_deadlock_counter++;
    }
}

/**
 * Recieves bytes over UART Connection.
 */
void receive_packet()
{
    Comm.uart_my_turn = true;
    // Resets the input string to null.
    Comm.uart_input_buffer = "";
    // Checks if there is available UART input.
    if(ss.available())
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
            while(ss.available())
            {
                // Reads in next ascii int and casts to char.
                temp = ss.read();
                // Checks for the final '$'. If so, stops recording the rest.
                // Prevents duplicating.
                if(temp == '$')
                {
                    // Appends character to string.
                    Comm.uart_input_buffer += temp;
                    while(ss.available())
                    {
                        // Throws away bad UART data.
                        char junk = ss.read();
                    }
                    Comm.flag_complete_packet = true;
                    Serial.println("Valid");
                    return;
                }
                // Middle of the packet data. Add to buffer.
                else
                {
                    // Appends character to string.
                    Comm.uart_input_buffer += temp;
                }
            }
        }
        // Not correct format. Read in it and throw it away.
        else
        {
            // Cycles until there is no input.
            while(ss.available())
            {
                // Throws away bad UART data.
                char junk = ss.read();
            }
        }
    } 
}


/**
 * Builds the message to sent to the LoRa via I2^C.
 */
void UART::create_packet()
{
   /**
    *                I2^C PACKETS   (MEGA -> LoRa)
    *
    * $,W,mega_pressure, mega_altitude, mega_external_temp,$
    *   1      2               3                 4
    * $,G,mega_roll, mega_pitch, mega_yaw,$
    *   1     2           3          4
    * $,P, target_heading, current_heading, craft_state,$
    *   1        2                3               4
    */
    
    Serial.print("Packet check: "); Serial.println(Comm.uart_packet_set);
    if(Comm.uart_packet_set == false)
    {
        // Creates a temporary string to hold all need information.
        uart_packet = "";
        // Each line below appends a certain divider or value to the string.
        uart_packet += '$';
        // The three conditional statements alternate to send each of the three packets.
        // 1 = W packet.
        if(uart_selector == 1)
        {
        	// Alerts the system that the packet has been updated for this turn.
            Comm.uart_packet_set = true;
            // Updates the selector back to 2 to switch to the next packet.
            uart_selector = 2;
            // Appends the appropriate variables to fill out the packet.
            uart_packet += ',';
            uart_packet += 'W';
            uart_packet += ',';
            uart_packet += "0.00";
            uart_packet += ',';
            uart_packet += "0.00";
            uart_packet += ',';
            uart_packet += Data.fltctrl_external_temperature;
            uart_packet += ',';
        }
        // 2 = G packet.
        else if(uart_selector == 2)
        {
        	// Alerts the system that the packet has been updated for this turn.
            Comm.uart_packet_set = true;
            // Updates the selector back to 3 to switch to the next packet.
            uart_selector = 3;
            // Appends the appropriate variables to fill out the packet.
            uart_packet += ',';
            uart_packet += 'G';
            uart_packet += ',';
            uart_packet += Data.fltctrl_roll;
            uart_packet += ',';
            uart_packet += Data.fltctrl_pitch;
            uart_packet += ',';
            uart_packet += Data.fltctrl_yaw;
            uart_packet += ',';
        }
        // 2 = P packet.
        else if(uart_selector == 3)
        {
        	// Alerts the system that the packet has been updated for this turn.
            Comm.uart_packet_set = true;
            // Updates the selector back to 1 to complete the cycle.
            uart_selector = 1;
            // Appends the appropriate variables to fill out the packet.
            uart_packet += ',';
            uart_packet += 'P';
            uart_packet += ',';
            uart_packet += Imu.target_heading;
            uart_packet += ',';
            uart_packet += Imu.craft_heading;
            uart_packet += ',';
            uart_packet += Movement.craft_state;
            uart_packet += ',';
        }
        // Completes the packet.
        uart_packet += '$';
    }
}


/**
 * Sends bytes over uart comms to the Radio.
 */
void UART::send_packet()
{
    // Every 1/20th second, the radio is allowed to send UART data.
    if((millis() - uart_timer > 50) && (Comm.uart_my_turn == true))
    {
        // Resets timer.
        uart_timer = millis();
        // Iterator
        int character_iterator = 0;
        // Sends the message.
        while(character_iterator < uart_packet.length())
        {
            ss.write(uart_packet[character_iterator]);
            character_iterator++;
        }
        Serial.print("Sent: ");
        Serial.println(uart_packet);
        // No longer my turn.
    	  Comm.uart_my_turn = false;
    	  // Allows the next UART packet to cycle to the next packet type.
        Comm.uart_packet_set = false;
    }
}
