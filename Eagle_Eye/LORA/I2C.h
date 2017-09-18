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
	void Initialize();
		
	//Processes all incoming and outgoing 
	void Manager(bool DROP_SIGNAL, class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in);
		
	//Reads in the incoming I2C message.
	void Receive();
		
	//Reads in the byte and asses if new data has been obtained.
	void Receive_Byte();
		
	//Sends the passed in integer over I2C.
	static void Transfer(int System_Event);
		
		


  //Holds the passed in class objects to gain access to their references.
  class Data;
  class GPS;
  //class I2C;
  class Para;
  class Radio;
  class Save;
  
	//Used in the detachment from HABET. Turned true by receiving '8' from LoRa.
	//   Once turned true, the IMU will decide if the craft is in the correct oreintation to drop.
	bool Detach_Request = false;
		
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
  int Received_Event;

};
#endif

