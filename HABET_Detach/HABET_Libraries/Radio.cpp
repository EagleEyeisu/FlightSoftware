/**
 * Radio.ccp holds all functions related the radio port/module infused inside the LoRa FeatherWing 
 *    development micro controller.
 */


#include "Radio.h"



/**
 * Parses and returns the radio transmission's altitude.
 */
float Radio::getAltitude(uint8_t buf)
{
	return (Parse(buf,1))
}


/**
 * Parses and returns the radio transmission's Command Received.
 */
float Radio::getCommandReceived(uint8_t buf)
{
	return (Parse(buf,9))
}


/**
 * Parses and returns the radio transmission's Command Sent.
 */
float Radio::getCommandSent(uint8_t buf)
{
	return (Parse(buf,8))
}


/**
 * Parses and returns the radio transmission's Craft ID.
 */
float Radio::getCraftID(uint8_t buf)
{
	return (Parse(buf,10))
}


/**
 * Parses and returns the radio transmission's latitude.
 */
float Radio::getLatitude(uint8_t buf)
{
	return (Parse(buf,2))
}


/**
 * Parses and returns the radio transmission's longitude.
 */
float Radio::getLongitude(uint8_t buf)
{
	return (Parse(buf,3))
}


/**
 * Parses and returns the radio transmission's LoRa Event.
 */
float Radio::getLoRaEvent(uint8_t buf)
{
	return (Parse(buf,4))
}


/**
 * Parses and returns the radio transmission's Release Status.
 */
float Radio::getReleaseStatus(uint8_t buf)
{
	return (Parse(buf,6))
}


/**
 * Parses and returns the radio transmission's Time Stamp (ms).
 *    LoRa  -> 0
 *    HABET -> 5
 *    MC    -> 7
 */
float Radio::getTimeStamp(uint8_t buf, int selector)
{
	return Parse(buf, selector);
}


/**
 * Assigns correct pins to the radio output port. Tests connections and variables.
 */
void Radio::Radio_Initialize()
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
void Radio::Radio_Manager()
{
	//Reads in radio transmission if available.
	Radio_Receive();
	
	//Checks for a specific Craft ID. '999.9' signals the start of operation.
	if(Network.Craft_ID == 999.9 && !Checked_In){
		
		//Responds to Mission Control with the correct ID to signal this node is here and listening.
		RollCall();
		
	}
	//After Roll Call is complete, Mission Control will broadcast the start signal. Appropriate delays are
	//   distributed below to initally sync the network to a 5 second split. This makes for a 15 second revolution.
	//
	//   MS - starts instantly
	//   HABET - delays 5 seconds
	//   EE - delays 10 seconds
	else if(Network.Craft_ID == 555.5){
		
		//Delays 5 seconds.
		delay(5000);
		
	}
	//Each of the 3 crafts have 5 seconds to broadcast. That means each craft will broadcast every 15 seconds.
	//   15000milliseconds = 15 seconds.
	else if(millis() - start >= 15000){
		
		//Resets the counter. This disabled broadcasting agian until 15 seconds has passed.
		start = millis();
		
		//Sends the transmission via radio.
		Radio_Send();
		
	}
}


/**
 * Responsible for reading in signals over the radio antenna.
 */
void Radio::Radio_Receieve()
{
	//Creates a temporary varaible to read in the incoming transmission. 
	uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
	
	//Gets the length of the above temporary varaible.
	uint8_t len = sizeof(buf);
	
	//Reads in the avaiable radio transmission, than checks if it is corrupt or complete.
	if(rf95.recv(buf, &len)) {
		
		//This whole section is comparing the currently held varaibles from the last radio update
		//   to that of the newly received signal. Updates the LoRa's owned variables and copies
		//   down the other nodes varaibles. If the time LoRa currently holds the most updated values
		//   for another node (LoRa's time stamp is higher than the new signal's), it replaces those vars.
		
		
		
		//Reads in the time stamp for LoRa's last broadcast.
		float temp_LoRa = getTimeStamp(buf, 0);
		
		//Compares the currently brought in time stamp to the one stored onboad.
		if(temp_LoRa > L_TS){
			
			//If the incoming signal has more up-to-date versions, we overwrite our saved version with
			//   the new ones.
			Network.L_TS = temp_LoRa;
			Network.Altitude = getAltitude(buf);
			Network.Latitude = getLatitude(buf);
			Network.Longitude = getLongitude(buf);
			Network.LE = getLoRaEvent(buf);
			
		}
		
		
		
		//Reads in the time stamp for Mission Control's last broadcast.
		float temp_MS = getTimeStamp(buf, 7);
		
		//Compares the currently brought in time stamp to the one stored onboad.
		if(temp_MS > MS_TS){
			
			//If the incoming signal has more up-to-date versions, we overwrite our saved version with
			//   the new ones.
			Network.MS_TS = temp_MS;
			Network.Command_Sent = getCommandSent(buf);
			Network.Command_Received = getCommandReceived(buf);
		}
		
	}
}


/**
 * Alters the craft ID of the radio transmission and broadcasts back to Mission Control.
 */
void Radio::Roll_Call()
{
	//Updates the Craft_ID to HABET's specific ID #.
	Network.Craft_ID = 2.0;
	
	//Sends the transmission via radio.
	Radio_Send();
	
	//Updates Checked_In Status.
	Checked_In = true;
}


/**
 * Creates an array to be sent out via Radio. Fills that array with correct values and returns it.
 */
float[] Radio::Construct_Transmission()
{
	//Updates the time object to hold the most current operation time.
	Time = millis();
	Network.H_TS = Time;
	
	//Temp array to be filled with the Network components.
	float[] temp = {Network.L_TS,
				    Network.Altitude,
				    Network.Latitude,
				    Network.Longitude,
				    Network.LE,
				    Network.H_TS,
				    Network.Release_Status,
				    Network.MS_TS,
				    Network.Command_Sent,
					Network.Command_Received,
					Network.Craft_ID
					};
				   
	//Returns the filled array.
	return temp;
}


/**
 * Responsible for sending out messages via the radio antenna.
 */
void Radio::Radio_Send()
{	
	//Fills out a character array with the updated Network components.
	float[] Transmission = Construct_Transmission();
	
	//Serial.print("Radio Sending: ");Serial.println(message);
	
	//Sends message passed in as paramter via antenna.
	rf95.send(Transmission, sizeof(Transmission));
		
	//Pauses all operations until the micro controll has guarrenteed the transmission of the
	//   signal. 
	rf95.waitPacketSent();
}