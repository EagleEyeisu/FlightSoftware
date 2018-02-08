/**
 * RADIO.ccp holds all functions related the radio port/module infused inside the LoRa FeatherWing 
 *    development micro controller.
 */


#include <Arduino.h>
#include "Radio.h"
#include "Data.h"
#include <RH_RF95.h>
#include "Globals.h"


/**
 * Constructor used to reference all other variables & functions.
 */
RADIO::RADIO()
{
	
}


/**
 * Parses and returns the radio transmission's altitude.
 */
float RADIO::getRadioAltitude(uint8_t buf)
{
	return (Data.Parse(buf,1));
}


/**
 * Parses and returns the radio transmission's Command Received.
 */
float RADIO::getCommandReceived(uint8_t buf)
{
	return (Data.Parse(buf,9));
}


/**
 * Parses and returns the radio transmission's Command Sent.
 */
float RADIO::getCommandSent(uint8_t buf)
{
	return (Data.Parse(buf,8));
}


/**
 * Parses and returns the radio transmission's Craft ID.
 */
float RADIO::getCraftID(uint8_t buf)
{
	return (Data.Parse(buf,10));
}


/**
 * Parses and returns the radio transmission's latitude.
 */
float RADIO::getRadioLatitude(uint8_t buf)
{
	return (Data.Parse(buf,2));
}


/**
 * Parses and returns the radio transmission's longitude.
 */
float RADIO::getRadioLongitude(uint8_t buf)
{
	return (Data.Parse(buf,3));
}


/**
 * Parses and returns the radio transmission's LoRa Event.
 */
float RADIO::getLoRaEvent(uint8_t buf)
{
	return (Data.Parse(buf,4));
}


/**
 * Parses and returns the radio transmission's Release Status.
 */
float RADIO::getReleaseStatus(uint8_t buf)
{
	return (Data.Parse(buf,6));
}


/**
 * Parses and returns the radio transmission's Time Stamp (ms).
 *    LoRa  -> 0
 *    HABET -> 5
 *    MC    -> 7
 */
float RADIO::getTimeStamp(uint8_t buf, int selector)
{
	return (Data.Parse(buf, selector));
}


/**
 * Assigns correct pins to the radio output port. Tests connections and variables.
 */
void RADIO::initialize()
{
	//Assigns pin 13 to have an output power connection to the LoRa's onboard LED.
	pinMode(LED, OUTPUT);
	
	//Assigns pin 4 to have an output singal connection to the LoRa's radio port.
	pinMode(RFM95_RST, OUTPUT);
	
	//Sends a high signal to the radio port for intialization.
	digitalWrite(RFM95_RST, HIGH);
	
	//Adjust the LED to be insync with radio trasmission.
	digitalWrite(RFM95_RST, LOW);
	
	//10 millisecond delay to allow for radio setup to complete before next instruction.
	delay(10);
	
	//Turns the radio output high to compelte setup.
	digitalWrite(RFM95_RST, HIGH);
  
	//Checks for the creation of the radio object and its physical connection attribute.
	//If invalid connection, the program will stall and print an error message.
	if(!rf95.init()){
		
		//Serial.println("LoRa radio init failed");
		while (1);
		
	}
	//Valid connection, program proceeds as planned.
	else{
		
		//Serial.println("LoRa radio init OK!");
		
	}
	
	//Checks the radio objects set frequency. 
	//If invalid connection, the program will stall and print an error message.
	if(!rf95.setFrequency(RF95_FREQ)){
		
		//Serial.println("setFrequency failed");
		while (1);
		
	}
	//Valid connection, program proceeds as planned.
	else{
		
		//Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
		
	}
	
	//Sets the max power to be used to in the amplification of the signal being sent out.
	rf95.setTxPower(23, false);
	//Serial.println();
}


/**
 * Manages all radio comms either incoming or outgoing.
 */
void RADIO::manager()
{
  
	//Reads in radio transmission if available.
	Radio.radioReceive();
	
	//Checks for a specific Craft ID. '999.9' signals the start of operation.
	if(Network.Craft_ID == 999.9 && !Radio.checkedIn){
		
		//Responds to Mission Control with the correct ID to signal this node is here and listening.
		Radio.rollCall();
		
	}
 
	//After Roll Call is complete, Mission Control will broadcast the start signal. Appropriate delays are
	//   distributed below to initally sync the network to a 5 second split. This makes for a 15 second revolution.
	//   
	//   MS - starts instantly
	//   HABET - delays .. seconds  <- NOT CURRENTLY INCLUDED.
	//   EE - delays 5 seconds
	else if(Network.Craft_ID == 555.5){
		
		//Delays 5 seconds.
		delay(5000);
    
    //Starts the broadcasting timer.
    start = millis();
		
	}
	//Each of the 3 crafts have 5 seconds to broadcast. That means each craft will broadcast every 15 seconds.
	//   15000 milliseconds = 15 seconds.
	else if(millis() - start >= 15000){
		
		//Resets the counter. This disables broadcasting again until 15 seconds has passed.
		start = millis();
		
		//Sends the transmission via radio.
		Radio.broadcast();
	}
}


/**
 * Responsible for reading in signals over the radio antenna.
 */
void RADIO::radioReceive()
{
  
	//Creates a temporary varaible to read in the incoming transmission. 
	uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
	
	//Gets the length of the above temporary varaible.
	uint8_t len = sizeof(buf);
	
	//Reads in the avaiable radio transmission, then checks if it is corrupt or complete.
	if(rf95.recv(buf, &len)) {

    //Blinks LED.
    blinkLED();
		
		//This whole section is comparing the currently held varaibles from the last radio update
		//   to that of the newly received signal. Updates the craft's owned variables and copies
		//   down the other nodes varaibles. If the timestamp indicates that this craft currently 
		//	 holds the most updated values for another node (ie: LoRa's time stamp is higher than the 
		//   new signal's), it replaces those variables.
		
		
		
		//Reads in the time stamp for HABET's last broadcast.
		float tempHABET = Radio.getTimeStamp(buf, 5);
		
		//Compares the currently brought in time stamp to the one stored onboad.
		if(tempHABET > Radio.Network.H_TS){
			
			//If the incoming signal has more up-to-date versions, we overwrite our saved version with
			//   the new ones.
			Network.H_TS = tempHABET;
			Network.Release_Status = Radio.getReleaseStatus(buf);
			
		}
		
		
		
		//Reads in the time stamp for Mission Control's last broadcast.
		float tempMC = Radio.getTimeStamp(buf, 7);
		
		//Compares the currently brought in time stamp to the one stored onboad.
		if(tempMC > Network.MC_TS){
			
			//If the incoming signal has more up-to-date versions, we overwrite our saved version with
			//   the new ones.
			Network.MC_TS = tempMC;
			Network.Command_Sent = Radio.getCommandSent(buf);
			Network.Command_Received = Radio.getCommandReceived(buf);
		}
		
	}
}


/**
 * Alters the craft ID of the radio transmission and broadcasts back to Mission Control.
 */
void RADIO::rollCall()
{
	//Updates the Craft_ID to Eagle Eye's specific ID #.
	Network.Craft_ID = 2.0;
	
	//Sends the transmission via radio.
	Radio.broadcast();
	
	//Updates Checked_In Status.
	checkedIn = true;
}


/**
 * Responsible for sending out messages via the radio antenna.
 */
void RADIO::broadcast()
{
  
  //Updates the time object to hold the most current operation time.
  Network.L_TS = millis();
  
  //Updates the Networks struct to reflect the crafts most up-to-date postioning before
  //   it broadcasts the signal on the network.
  Network.Altitude = Data.Local.Altitude;
  Network.Latitude = Data.Local.Latitude;
  Network.Longitude = Data.Local.Longitude;
  
  //Alter this line so when ground recieves, it resets Lora's LE to 0.
  Network.LE = Data.Local.LE;
  
  //Casting all float values to a character array with commas saved in between values
  //   so the character array can be parsed when received by another craft.
  char transmission[] = {char(Network.L_TS),
                         ',',
                         char(Network.Altitude),
                         ',',
                         char(Network.Latitude),
                         ',',
                         char(Network.Longitude),
                         ',',
                         char(Network.LE),
                         ',',
                         char(Network.H_TS),
                         ',',
                         char(Network.Release_Status),
                         ',',
                         char(Network.MC_TS),
                         ',',
                         char(Network.Command_Sent),
                         ',',
                         char(Network.Command_Received),
                         ',',
                         char(Network.Craft_ID)
                         };
	
	//Serial.print("Radio Sending: ");Serial.println(transmission);
  
	//Sends message passed in as paramter via antenna.
	rf95.send(transmission, sizeof(transmission));
		
	//Pauses all operations until the micro controll has guaranteed the transmission of the
	//   signal. 
	rf95.waitPacketSent();
}


/*
 * Blinks LED.
 */
void blinkLED(){

  //ON
  digitalWrite(LED, HIGH);

  delay(10);

  //OFF
  digitalWrite(LED, LOW);
}
