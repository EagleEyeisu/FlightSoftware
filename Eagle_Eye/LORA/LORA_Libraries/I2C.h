/*
  I2C.h
*/

#ifndef I2C_h
#define I2C_h

class I2C
{
  public:
	//Runs initialzation script for the I2C protocol.
  	void I2C_Initialize();
	
	//Processes all incoming and outgoing 
    void I2C_Manager();
	
	//Reads in the incoming I2C message.
	void I2C_Receive();
	
	//Reads in the byte and asses if new data has been obtained.
	void Receive_Byte();
	
	//Sends the passed in integer over I2C.
	void I2C_Send(int System_Event);
	
	
	
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

