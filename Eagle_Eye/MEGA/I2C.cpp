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
	//Sets the address for the current micro controller.
	//   Mega - 0
	//   LoRa - 1
	Wire.begin(0);
	//Serial.println("Comms Address Set.");
}


/**
 * Controls what message gets sent where much like an old telephone switchboard operator.
 */
void I2C::manager()
{
	
	//When this is false, the program only trys to receive messages.
	if (!DISPATCH_SIGNAL) {
		
		//Checks for incoming communication.
		switchBoard(false, DISPATCH_SIGNAL, 0); 
	}
	
	//Sends handshake to LoRa upon communication switch.
	if (directionSwitch == 1) { 
	
		//Sends the communication switch signal to LoRa.
		Transfer(13);
		
		//Increments the iterator to only allow for this senario to happen once.
		directionSwitch++;
	}
    
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
void I2C::switchBoard(bool Local, bool Send, int systemEvent)
{
	//Sends or Receives an event from or to the other micro controller.
	if (!Local) {
		
		//Sends event.
		if (Send) { 
			
			//Sends event signal to LoRa.
			Transfer(systemEvent);
		}
		//Receive event.
		else { 
		
			//Reads in byte from other micro controller.
			Receive();
			
		}
	}
	//Keeps the event and saves in locally.
	else {
		
		//Assigns the ME (Mega_Event) to be saved. 
		Data.Local.ME = systemEvent;
	}
}


/**
 * Sends byte over I2C Connection.
 */
void I2C::Transfer(int systemEvent)
{
	//Assigns the ME (Mega_Event) to be saved.
	Data.Local.ME = systemEvent;
	
	//Assigns address of the receiving board.
	Wire.beginTransmission(0);
	
	//Sends the message.
	Wire.write(systemEvent);
	
	//Closes the transmission.
	Wire.endTransmission();
}


/**
 * Recieves byte over I2C Connection.
 */
void I2C::Receive()
{
	//Reads in byte if serial port is not empty.
  if(Wire.available()){
    receiveByte();
  }
	
	//newData turns true when a new byte has been read in from the 
	//   serial port. Prevents the repeated recording of the same 
	//   message.
	if (newData) {
		
		//Updates the Lora Event variable for data logging purposes.
		Data.Local.LE = receievedEvent;
		
		//Resets the 'newData' check.
		newData = false;
	}
}


/**
 * Reads in byte from serial port.
 */
void I2C::receiveByte()
{
	//Serial.print("Event Recieved: ");
	
	//Receive byte as an integer
	receievedEvent = Wire.read();
	
	//Serial.println(receievedEvent);
	
	//Special senario for communication switch. From LoRa -> Mega, to Mega -> LoRa.
	if (receievedEvent == 8) {
		
		//Program now sends messages instead of receiving them.
		DISPATCH_SIGNAL = true;
		
		//Triggers the check for if the craft is in the correct orientation to detach from HABET.
		Detach_Request = true;
		
		//Serial.print("Sending Mode");
		
		//Used to directly trigger the handshake TO the LoRa micro controller. 
		directionSwitch = 1;
	}
	//Special senario for the detachment of the Eagle Eye craft from HABET.
	else if (receievedEvent == 4) {
		
		//Eagle Eye is now disconnected from HABET.
		HABET_Connection = false;
		//Serial.println("Detached");
	}
	
	//Tells the program that the received message is different from that of the previously
	//   received message. This allows the for the message or 'event' to be saved to the SD card.
	newData = true;
}
