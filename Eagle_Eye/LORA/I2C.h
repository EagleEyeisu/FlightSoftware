/**
 * I2C.h 
 */

#ifndef I2C_h
#define I2C_h

#include <Arduino.h>

class I2C
{
	public:
  
	// Constructor
	I2C();
  
	// Runs initialzation script for the I2C protocol.
	void initialize();
	
	// Processes all incoming and outgoing 
	void manager();
  
  	// Builds byte array to be sent to MEGA.
  	void create_mega_packet();
	
	// Sends byte array to MEGA.
	void send_mega_packet();

  	// Inter-Intergrated Circuit (I^2C) packet..
  	String i2c_packet = "";
  
};
#endif

