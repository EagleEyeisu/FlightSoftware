/**
 * I2C.cpp handles sending of event messages
 *    between the LORA and MEGA via I2C protocol.
 */


#include "I2C.h"


/**
 * Assigns the proper address to the current micro controller.
 */
void I2C::I2C_Initialize()
{
	//Sets the address for the current micro controller.
	//   Mega - 0
	//   LoRa - 1
	Wire.begin(0);
	Serial.println("Comms Address Set.");
}


/**
 * Controls what message gets sent where much like an old telephone switchboard operator.
 */
void I2C::I2C_Manager()
{
	
	//When this is false, the program only trys to receive messages.
	if (!DISPATCH_SIGNAL) {
		
		//Checks for incoming communication.
		I2C_SwitchBoard(false, DISPATCH_SIGNAL, 0); 
	}
	
	//Sends handshake to LoRa upon communication switch.
	if (I2C_DirectionSwitch == 1) { 
	
		//Sends the communication switch signal to LoRa.
		I2C_Send(13);
		
		//Increments the iterator to only allow for this senario to happen once.
		I2C_DirectionSwitch++;
	}
	
	
	//Used to send a test signal back to LoRa.
	
	//if(Serial.read() == 's' && DISPATCH_SIGNAL) {
	//	I2C(false,true,0);
	//}
    
}


/**
 * Responsible for if the micro controller looks to receive/send a message, and 
 * where it gets stored.
 *
 * 10 - Motor Start
 * 11 - Brake Off
 * 12 - Brake On
 * 13 - Communication Switch
 */
void I2C::I2C_SwitchBoard(bool Local, bool Send, int System_Event)
{
	//Sends or Receives an event from or to the other micro controller.
	if (!Local) {
		
		//Sends event.
		if (Send) { 
			
			//Sends event signal to LoRa.
			I2C_Send(System_Event);
		}
		//Receive event.
		else { 
		
			//Reads in byte from other micro controller.
			Receive_I2C();
			
		}
	}
	//Keeps the event and saves in locally.
	else {
		
		//Assigns the ME (Mega_Event) to be saved. 
		data.ME = System_Event;
	}
}


/**
 * Sends byte over I2C Connection.
 */
void I2C::I2C_Send(int System_Event)
{
	//Assigns the ME (Mega_Event) to be saved.
	data.ME = System_Event;
	
	//Assigns address of the receiving board.
	Wire.beginTransmission(1);
	
	//Sends the message.
	Wire.write(System_Event);
	
	//Closes the transmission.
	Wire.endTransmission();
}


/**
 * Recieves byte over I2C Connection.
 */
void I2C::I2C_Receieve()
{
	//Reads in byte if serial port is not empty.
	Wire.onReceive(Receive_Byte());
	
	//newData turns true when a new byte has been read in from the 
	//   serial port. Prevents the repeated recording of the same 
	//   message.
	if (newData) {
		
		//Updates the Lora Event variable for data logging purposes.
		data.LE = Receieved_Event;
		
		//Resets the 'newData' check.
		newData = false;
	}
}


/**
 * Reads in byte from serial port.
 */
void I2C::Receive_Byte()
{
	Serial.print("Event Recieved: ");
	
	//Receive byte as an integer
	Receieved_Event = Wire.read();
	
	Serial.println(Receieved_Event);
	
	//Special senario for communication switch. From LoRa -> Mega, to Mega -> LoRa.
	if (Receieved_Event == 8) {
		
		//Program now sends messages instead of receiving them.
		DISPATCH_SIGNAL = true;
		
		//Triggers the check for if the craft is in the correct orientation to detach from HABET.
		DETACH_REQUEST = true;
		
		Serial.print("Sending Mode");
		
		//Used to directly trigger the handshake TO the LoRa micro controller. 
		I2C_DirectionSwitch = 1;
	}
	//Special senario for the detachment of the Eagle Eye craft from HABET.
	else if (Receieved_Event == 4) {
		
		//Eagle Eye is now disconnected from HABET.
		HABET_Connection = false;
		Serial.println("Detached");
	}
	
	//Tells the program that the received message is different from that of the previously
	//   received message. This allows the for the message or 'event' to be saved to the SD card.
	newData = true;
}