/**
 * Radio.ccp holds all functions related the radio port/module infused inside the LoRa FeatherWing 
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
		
		Serial.println("LoRa radio init failed");
		while (1);
		
	}
	//Valid connection, program proceeds as planned.
	else{
		
		Serial.println("LoRa radio init OK!");
		
	}
	
	//Checks the radio objects set frequency. 
	//If invalid connection, the program will stall and print an error message.
	if(!rf95.setFrequency(RF95_FREQ)){
		
		Serial.println("setFrequency failed");
		while (1);
		
	}
	//Valid connection, program proceeds as planned.
	else{
		
		Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
		
	}
	
	//Sets the max power to be used to in the amplification of the signal being sent out.
	rf95.setTxPower(23, false);
	Serial.println();
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
	//   HABET - delays 5 seconds
	//   EE - delays 10 seconds
	else if(Network.Craft_ID == 555.5){
		
		//Does not delay because MS is the first node to broadcast after rollcall is compelted.
		//delay(0000);
		
	}
	//Each of the 3 crafts have 5 seconds to broadcast. That means each craft will broadcast every 15 seconds.
	//   15000milliseconds = 15 seconds.
	else if(millis() - start >= 15000){
		
		//Resets the counter. This disabled broadcasting agian until 15 seconds has passed.
		start = millis();
		
		//Sends the transmission via radio.
		Radio.broadcast();
	}
}


/**
 * Responsible for reading in signals over the radio antenna.
 */
void RADIO::radioReceieve()
{
	//Creates a temporary varaible to read in the incoming transmission. 
	uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
	
	//Gets the length of the above temporary varaible.
	uint8_t len = sizeof(buf);
	
	//Reads in the avaiable radio transmission, than checks if it is corrupt or complete.
	if(rf95.recv(buf, &len)) {

    //Blinks LED.
    blinkLED();
    
		//This whole section is comparing the currently held varaibles from the last radio update
		//   to that of the newly received signal. Updates the LoRa's owned variables and copies
		//   down the other nodes varaibles. If the time LoRa currently holds the most updated values
		//   for another node (LoRa's time stamp is higher than the new signal's), it replaces those vars.
		
		
		
		//Reads in the time stamp for HABET's last broadcast.
		float temp_HABET = Radio.getTimeStamp(buf, 5);
		
		//Compares the currently brought in time stamp to the one stored onboad.
		if(temp_HABET > Radio.Network.H_TS){
			
			//If the incoming signal has more up-to-date versions, we overwrite our saved version with
			//   the new ones.
			Network.H_TS = temp_HABET;
			Network.Release_Status = Radio.getReleaseStatus(buf);
			
		}
		
		
		
		//Reads in the time stamp for Mission Control's last broadcast.
		float temp_LoRa = Radio.getTimeStamp(buf, 0);
		
		//Compares the currently brought in time stamp to the one stored onboad.
		if(temp_LoRa > L_TS){
			
			//If the incoming signal has more up-to-date versions, we overwrite our saved version with
			//   the new ones.
			Network.L_TS = temp_LoRa;
			Network.Altitude = Radio.getAltitude(buf);
			Network.Latitude = Radio.getLatitude(buf);
			Network.Longitude = Radio.getLongitude(buf);
			Network.LE = Radio.getLoRaEvent(buf);
			
		}
		
	}
}


/**
 * Alters the craft ID of the radio transmission and broadcasts back to Mission Control.
 */
void RADIO::rollCall()
{
	//Updates the Craft_ID to Mission Control's specific ID #.
	Network.Craft_ID = 1.0;
	
	//Sends the transmission via radio.
	Radio.broadcast();
	
	//Updates Checked_In Status.
	checkedIn = true;
}


/**
 * Creates an array to be sent out via Radio. Fills that array with correct values and returns it.
 */
void RADIO::broadcast()
{
  
	//Updates the time object to hold the most current operation time.
	Network.MS_TS = millis();
	
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
