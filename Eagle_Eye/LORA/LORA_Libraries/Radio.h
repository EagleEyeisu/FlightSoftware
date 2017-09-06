/**
 * Radio.h holds all declarations and varaibles related to the radio.ccp file.
 */

#ifndef Radio_h
#define Radio_h

class Radio
{
  public:
	
	//Returns the transmission's Altitude.
	float getAltitude();
	
	//Returns the transmission's CR variable.
	float getCommandRecieve();
	
	//Returns the transmission's CS variable.
	float getCommandSent();
	
	//Returns the transmission's craft ID.
	float getCraftID();
	
	//Returns the transmission's Latitude.
	float getLatitude();
	
	//Returns the transmission's Longitude.
	float getLongitude();
	
	//Returns the transmission's Latitude.
	float getLoRaEvent();
	
	//Returns the transmission's Release Status.
	float getReleaseStatus();
	
	//Returns the transmission's time stamp.
	float getTimeStamp();
	
  	//Runs initialzation script for the Radio.
	void Radio_Initialize();

	//Passively watches for incoming radio transmissions from Mission Control and other crafts.
	void Radio_Manager();
	
	//Receives incoming transmission.
	void Radio_Receive();
	
	//Responds to the RollCall signal sent from Mission Control.
	void Roll_Call();
	
	//Fills out the array with the Network componenets.
	float[] Construct_Transmission();
	
	//Sends the desired signal out over the radio antenna.
	void Radio_Send();
	
	
	
	//Chip select pin for the radio.
	#define RFM95_CS 8
	
	//Intialization pin for radio.
	#define RFM95_INT 7
	
	//Reset pin onboard the radio.
	#define RFM95_RST 4
	
	//Pins used to blink an LED for currently unknown purpose.
	#define LED 13
	
	//Radio frequency used throught the Eagle Eye Program. CHECK WITH HABET BEFORE EACH FLIGHT!!!!!
	#define RF95_FREQ 433.0
	
	//Status of the craft replying to Mission Control with its node #.
	bool Checked_In = false;
	
	//Directs the radio object to focus on two specific ports.
	RH_RF95 rf95(8, 7);
	
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
		//   or if we have a direct line of communication with each node. LoRa's ID is capital 'C'.
		float Craft_ID = 0.0;
	}
	struct Network_Data Network;

};

#endif