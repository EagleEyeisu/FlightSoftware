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


void I2C::validate_packet()
{
  Comm.packet_validation = true;
  bool format_start = false;
  bool format_end = false;
  bool format_length = false;
  bool format_overall = false;
  Serial.print("Length: ");
  Serial.println(Comm.i2c_buffer.length());
  if(Comm.i2c_buffer.length() >= 18)
  {
    Serial.println("Correct length");
    format_length = true;
  }
  if(Comm.i2c_buffer[0] == '$')
  {
    Serial.println("Correct Start");
    format_start = true;
  }
  if(Comm.i2c_buffer[17] == '$') //Hard coded.
  {
    Serial.println("Correct Ending");
    format_end = true;
  }
  if(format_length && format_start && format_end)
  {
    Serial.println("Correct Packet");
    format_overall = true;
  }
  if(format_overall)
  {
    
    Comm.complete_packet_flag = true;
    Serial.println("Reverting to previous packet.");
    char temp_to_parse[Comm.i2c_buffer.length()];
    Comm.i2c_buffer.toCharArray(temp_to_parse,Comm.i2c_buffer.length()+1);
    Serial.print("Reverted to_parse: "); Serial.println(temp_to_parse);
  }
  else
  {
    Comm.complete_packet_flag = false;
  }
  Comm.packet_validation = false;
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
    if(Wire.available() && Comm.packet_validation == false)
    {
    	// Reads in first ascii int and casts to char.
    	char temp = Wire.read();
    	// First char should be the dollar sign. 
    	// If so, continute. If not, junk it.
    	if(temp == '$')
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
    // Interrupt triggered during previous packet validation. Discard input for this cycle.
    else
    {
        // Cycles until there is no input.
        while(Wire.available())
        {
          // Reads in i2c input and kills it.
          char junk = Wire.read();
        }
    }
    
    // Checks for proper formatting. Forces the program to wait for a valid i2c packet
    // prior to trying to parse the data.
    if(start_flag && end_flag)
    {
      Data.new_data = Data.YES;
    	// Sets flag to true signifying a valid packet.
      Comm.complete_packet_flag = true;
      // Converts string to char array that will be used for parsing.
    	char to_parse[Comm.i2c_buffer.length()];
    	Comm.i2c_buffer.toCharArray(to_parse,Comm.i2c_buffer.length()+1);
     
      Serial.print("I2C Parse: ");Serial.println(to_parse);

      Serial.print("Target Throttle: ");
      Data.Local.lora_target_throttle = Data.get_i2c_target_throttle(to_parse); 
      Serial.print("Direction: ");
      Data.Local.craft_manual_direction = Data.get_i2c_manual_command(to_parse);
      //Serial.print("Anchor: ");
      //Data.Local.craft_anchor_status = Data.get_i2c_craft_anchor(to_parse);
    }
}
