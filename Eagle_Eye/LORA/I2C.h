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
  
  	// Builds byte array to be sent via Controller Access Network.
  	void preLoadCAN();
	
	// Sends the NDP byte array via CAN bus.
	void upLoadCAN();

  	// Neighbor Discovery Protocol to be sent via I2C CAN bus.
  	String NDP = "";
  
};
#endif

