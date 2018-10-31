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
	
	// Processes all outgoing packets.
	void manager();
  
  	// Builds byte array to be sent to MEGA.
  	void create_mega_packet();
	
	// Sends byte array to MEGA.
	void send_mega_packet();

  	// Inter-Intergrated Circuit (I^2C) packet.
  	String i2c_packet = "";

  	// Timer.
  	unsigned long i2c_timer = 0;

  	// Selects between the Craft data(1), Target data(2), and network data(3).
  	int i2c_selector = 1;

  	// Controls who has access to send over the i2c line.
  	bool i2c_send_permission = true;
  
};
#endif

