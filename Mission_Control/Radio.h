/**
 * Radio.h holds all declarations and varaibles related to the radio.ccp file.
 */

#ifndef RADIO_h
#define RADIO_h

#include <Arduino.h>

class RADIO
{
  public:

  //Constructor
  RADIO();
  
  //Returns the transmission's Altitude.
  float getRadioAltitude(uint8_t buf);
    
  //Returns the transmission's CR variable.
  float getCommandReceived(uint8_t buf);
  
  //Returns the transmission's CS variable.
  float getCommandSent(uint8_t buf);
    
  //Returns the transmission's craft ID.
  float getCraftID(uint8_t buf);
    
  //Returns the transmission's Latitude.
  float getRadioLatitude(uint8_t buf);
    
  //Returns the transmission's Longitude.
  float getRadioLongitude(uint8_t buf);
    
  //Returns the transmission's Latitude.
  float getLoRaEvent(uint8_t buf);
  
  //Returns the transmission's Release Status.
  float getReleaseStatus(uint8_t buf);
  
  //Returns the transmission's time stamp.
  float getTimeStamp(uint8_t buf, int selector);
	
  //Runs initialzation script for the Radio.
  void initialize();

  //Passively watches for incoming radio transmissions from Mission Control and other crafts.
  void manager();
  
  //Receives incoming transmission.
  void radioReceive();
  
  //Responds to the RollCall signal sent from Mission Control.
  void rollCall();
  
  //Sends the desired signal out over the radio antenna.
  void broadcast();

  //Blinks the LED on the LoRa uC to show a signal has been received.
  void blinkLED();
	
	
	
  //Chip select pin for the radio.
  const byte RFM95_CS = 8;
  
  //Intialization pin for radio.
  const byte RFM95_INT = 7;
  
  //Reset pin onboard the radio.
  const byte RFM95_RST = 4;
  
  //Pins used to blink an LED to signal receival packet.
  const byte LED = 13;
	
	//Radio frequency used throught the Eagle Eye Program. CHECK WITH HABET BEFORE EACH FLIGHT!!!!!
	#define RF95_FREQ 433.0
	
	//Status of the craft replying to Mission Control with its node #.
  bool checkedIn = false;
	
  //Stores all information related to the network of the Eagle Eye program.
	//   This struct reads specific indexes and than rebroadcasts the updated transmission to
	//   the other nodes in the network. 
	struct Network_Data {
		
		/**
		 * The first four varaibles are accessed and overseen by the Eagle Eye craft.
		 */
		
		//Each of these is defined in the Data.h struct. Refer to its documentation as needed.
		float L_TS = 0.0;
		float Altitude = 0.0;
		float Latitude = 0.0;
		float Longitude = 0.0;
		float LE = 0.0;
		
		/**
		 * The below varaible is overseen by HABET_Detach.
		 */
		
		//HABET's ms Time stamp.
		float H_TS = 0.0;
		
		//Release status '0' -> not released, '1' -> released.
		float Release_Status = 0.0;
		
		/**
		 * The last 3 variables are overseen by Mission Control.
		 */
		
		//Mission Control's ms Time stamp.
		float MS_TS = 0.0;
		
		//Command_Sent will be what triggers a command on another craft (Eagle Eye). 
		float Command_Sent = 0.0;
		
		//Command_Received will be turned 1 when the Command_Sent receahes its destination. 
		//   Used as a handhshake, but primary purpose is to be able to verify that an action 
		//   reached its destination via Mission Control.
		float Command_Received = 0.0;
		
		/**
		 * This varaible is updated by each craft right before the array is broadcasted.
		 */
		
		//Craft_ID is used to tell which craft is currently broadcasting the signal. This allows
		//   for Mission Control to have a sense of if information is being relayed through nodes,
		//   or if we have a direct line of communication with each node.
		float Craft_ID = 0.0;
	}
	struct Network_Data Network;

  //This timer is used for syncing the mesh network. Every craft as 5 seconds to talk. 
  //   When this timer is at 0, that means the craft just broadcasted its data. When this
  //   this timer hits 15, it resets to 0. This insures that the 2 other nodes have their 5
  //   seconds to talk. 3nodes * 5seconds = 15seconds for a total netowrk revolution. 
  unsigned long start = 0;

};

#endif
