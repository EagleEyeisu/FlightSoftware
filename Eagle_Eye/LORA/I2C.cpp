/**
 * I2C.cpp handles sending of event messages
 * between the LORA and MEGA via I2C protocol.
 */


#include "Data.h"
#include "I2C.h"
#include "Wire.h"
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
void I2C::Initialize()
{
	//Sets the address for the current micro controller.
	//   Mega - 0
	//   LoRa - 1
	Wire.begin(1);
	Serial.println("Comms Address Set.");
}


/**
 * Controls what message gets sent where much like an old telephone switchboard operator.
 */
void I2C::Manager(bool DROP_SIGNAL, class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in)
{
  Data = Data_in;
  GPS = GPS_in;
  //I2C = I2C_in;
  Para = Para_in;
  Radio = Radio_in;
  Save = Save_in;
  
	//Special senario that signals to the Mega for communication switch. 
	if(DROP_SIGNAL){
		
		//Sends event to Mega signaling the start of the drop process. This triggers the Mega
		//   to check the craft's orientation and respond back to the LoRa with its results.
		Transfer(8);
		
		//Switches the direciton of I2C Communication. From (LoRa -> Mega) / To (Mega -> LoRa)
		DISPATCH_SIGNAL = false;
		
		Serial.println("Receiving Mode");
	}
	//Checks for an incoming signal from the Mega.
	if(!DISPATCH_SIGNAL){
		
		//Reads in byte from other micro controller.
		Receive();
	}
}


/**
 * Recieves byte over I2C Connection.
 */
void I2C::Receive()
{
	//Reads in byte if serial port is not empty.
	if(Wire.available()){
    Receive_Byte();
	}
	
	//newData turns true when a new byte has been read in from the 
	//   serial port. Prevents the repeated recording of the same 
	//   message.
	if (newData) {
		
		//Updates the Mega Event variable for data logging purposes.
		Data.Local.ME = Received_Event;
		
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
	Received_Event = Wire.read();
	
	Serial.println(Received_Event);
	
	//Special senario for receiving the Go Ahead for drop singal from the Mega.
	if(Received_Event == 9){
		
		//The final saftey check in the DETACHMENT process from HABET. 
		READY_FOR_DROP = true;
		
	}
	//Special senario for receiving the Handshake from the Mega confirming the switch in I2C direction.
	else if(Received_Event == 13){
		
		Serial.println("Handshake Recieved");
    
	}
	
	//Tells the program that the received message is different from that of the previously
	//   received message. This allows the for the message or 'event' to be saved to the SD card.
	newData = true;
}


/**
 * Sends byte over I2C Connection.
 */
void I2C::Transfer(int System_Event)
{
	//Assigns the LE (LoRa_Event) to be saved.
	Data.Local.LE = System_Event;
	
	//Assigns address of the receiving board.
	Wire.beginTransmission(1);
	
	//Sends the message.
	Wire.write(System_Event);
	
	//Closes the transmission.
	Wire.endTransmission();
}
