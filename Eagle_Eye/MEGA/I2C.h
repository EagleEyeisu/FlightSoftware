/**
 * I2C.h declares and holds the variables and frunctions used for I2C.
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
  void manager();

	//Responsible for sending/receiving messages and saving them.
	void switchBoard(bool Local, bool Send, int System_Event);
	
	//Sends event over I2C comms.
	void Transfer(int System_Event);
	
	//Receives event over I2C comms.
	void Receive();
	
	//Reads in a individual byte from the I2C port.
	void receiveByte();
	
	
	
	//Used in the detachment from HABET. Turned true by receiving '8' from LoRa.
	//   Once turned true, the IMU will decide if the craft is in the correct oreintation to drop.
	bool Detach_Request = false;
	
	//Status for I2C direction. False = (LoRa -> Mega), True = (Mega -> LoRa)
	bool DISPATCH_SIGNAL = false;
	
	//Status for Connection to HABET.
  bool HABET_Connection = true;
	
	//Has to do with the communication switch process between the micro controllers in the Eagle Eye Craft.
	//   When this is set to 1 via receiving the event message '4' from the LoRa, it triggers the handshake
	//   of the direction switch back to the LoRa. (Keeps both micro controllers in check)
	int directionSwitch = 0;  
	
	//Purpose is to stop message spamming from either micro controller.
	//   Turns true if data is recieved from from LoRa.
	bool newData = false;      

	//Event Number.
	int receievedEvent;     

};

#endif

