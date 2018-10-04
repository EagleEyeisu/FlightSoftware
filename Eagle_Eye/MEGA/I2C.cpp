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
	Wire.begin(8);
 	Wire.onReceive(receiveEvent);
}


/**
 * Recieves bytes over I2C Connection.
 */
void receiveEvent(int howMany)
{
    Comm.i2c_buffer = "";
    bool start_flag = false;
    bool end_flag = false;
    bool junk_flag = false;
    
    /*-----------------------------------------*/
    //
    // INPUT MUST BE ->      $,#.##,#.##,#.##,$
    //
    /*-----------------------------------------*/
    Serial.println("In Interrupt");
    // Checks if there is available i2c input.
    if(Wire.available())
    {
    	// Reads in first ascii int and casts to char.
    	char temp = Wire.read();
    	// First char should be the dollar sign. 
    	// If so, continute. If not, junk it.
    	if(temp == '$' && Comm.packet_validation == false)
    	{
    		// Appends character to string.
    		Comm.i2c_buffer += temp;
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
		    		Comm.i2c_buffer += temp;
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
         // MIddle of the packet data. Add to buffer.
         else
         {
           // Appends character to string.
           Comm.i2c_buffer += temp;
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
      Data.new_data = Data.YES;
      Comm.complete_packet_flag = true;
    }
    else
    {
      Comm.complete_packet_flag = false;
    }
}
