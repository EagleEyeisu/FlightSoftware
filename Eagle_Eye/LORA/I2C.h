/**
 * I2C.h 
 */

#ifndef I2C_h
#define I2C_h

#include <Arduino.h>

class I2C
{
  public:

	//Constructor
	I2C();
  
	//Runs initialzation script for the I2C protocol.
	void initialize();
	
	//Processes all incoming and outgoing 
	void manager(bool DROP_SIGNAL);
	
	//Reads in the incoming I2C message.
	void Receive();
	
	//Reads in the byte and asses if new data has been obtained.
	void receiveByte();
	
	//Sends the passed in integer over I2C.
	static void Transfer(int systemEvent);
	
	
	
	
	//Status for I2C direction. False = (LoRa -> Mega), True = (Mega -> LoRa)
	bool DISPATCH_SIGNAL = true;
	
	//Status for Connection to HABET.
	bool HABET_Connection = true;

	//Purpose is to stop message spamming from either micro controller.
	//   Turns true if data is recieved from from LoRa.
	bool newData = false;      

	//Turned true when Mega replies with Go Ahead for drop.
	bool READY_FOR_DROP = false;

	//Event Number.
	int receivedEvent;

};
#endif

