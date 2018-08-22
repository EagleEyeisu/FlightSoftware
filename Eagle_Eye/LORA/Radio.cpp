/**
 * RADIO.ccp holds all functions related the radio port/module infused inside the LoRa FeatherWing 
 * development micro controller.
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
float RADIO::getRadioAltitude(char buf[])
{
	return (Data.Parse(buf,1));
}


/**
 * Parses and returns the radio transmission's latitude.
 */
float RADIO::getRadioLatitude(char buf[])
{
  return (Data.Parse(buf,2)) / 10000.0;
}


/**
 * Parses and returns the radio transmission's longitude.
 */
float RADIO::getRadioLongitude(char buf[])
{
  return (Data.Parse(buf,3)) / 10000.0;
}


/**
 * Parses and returns the radio transmission's LoRa Event.
 */
float RADIO::getLoRaEvent(char buf[])
{
  return (Data.Parse(buf,4));
}



/**
 * Parses and returns the radio transmission's Time Stamp (ms).
 * LoRa  -> 0
 * MC    -> 5
 */
float RADIO::getTimeStamp(char buf[], int selector)
{
  return (Data.Parse(buf, selector));
}


/**
 * Parses and returns the radio transmission's Start or Stop variable.
 * 0.0 -> pause
 * 1.0 -> running
 */
float RADIO::getStartStop(char buf[])
{
	return (Data.Parse(buf,6));
}


/**
 * Parses and returns the radio transmission's targetThrottle
 */
float RADIO::getTargetThrottle(char buf[])
{
	return (Data.Parse(buf,7));
}


/**
 * Parses and returns the radio Target Latitude.
 */
float RADIO::getRadioTargetLat(char buf[])
{
  return (Data.Parse(buf,8)) / 10000.0;
}


/**
 * Parses and returns the radio Target Longitude.
 */
float RADIO::getRadioTargetLon(char buf[])
{
  return (Data.Parse(buf,9)) / 10000.0;
}


/**
 * Parses and returns the radio transmission's Craft ID.
 */
float RADIO::getCraftID(char buf[])
{
	return (Data.Parse(buf,10));
}


/**
 * Assigns correct pins to the radio output port. Tests connections and variables.
 */
void RADIO::initialize()
{
	
	// Assigns pin 4 to have an output singal connection to the LoRa's radio port.
	pinMode(RFM95_RST, OUTPUT);
	
	// Sends a high signal to the radio port for intialization.
	digitalWrite(RFM95_RST, HIGH);
	
	// Adjust the LED to be insync with radio trasmission.
	digitalWrite(RFM95_RST, LOW);
	
	// 10 millisecond delay to allow for radio setup to complete before next instruction.
	delay(10);
	
	// Turns the radio output high to compelte setup.
	digitalWrite(RFM95_RST, HIGH);
  
	// Checks for the creation of the radio object and its physical connection attribute.
	// If invalid connection, the program will stall and print an error message.
	if(!rf95.init()){
		// Serial.println("LoRa radio init failed");
		while (1);
	}
	// Valid connection, program proceeds as planned.
	else{
		// Serial.println("LoRa radio init OK!");
	}
	
	// Checks the radio objects set frequency. 
	// If invalid connection, the program will stall and print an error message.
	if(!rf95.setFrequency(RF95_FREQ)){
		// Serial.println("setFrequency failed");
		while (1);
	}
	// Valid connection, program proceeds as planned.
	else{
		// Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
	}
	// Sets the max power to be used to in the amplification of the signal being sent out.
	rf95.setTxPower(23, false);
	// Serial.println();
}


/**
 * Manages all radio comms either incoming or outgoing.
 */
void RADIO::manager()
{ 
	// Reads in radio transmission if available.
	Radio.radioReceive();
 
	// Checks for a specific Craft ID. '999.9' signals the start of operation.
	if(receivedID == 999.0 && !Radio.checkedIn){
    // Updates crafts states.
    RCstate = RUNNING;
    OperationMode = ROLLCALL;
    
		// Responds to Mission Control with the correct ID to signal this node is here and listening.
    delay(100);
		Radio.rollCall();
	}
 
	// After Roll Call is complete, Mission Control will broadcast the start signal. Appropriate delays are
	// distributed below to initally sync the network to a 5 second split. This makes for a 10 second revolution.
	//   
	// MS - starts instantly
	// EE - delays 5 seconds
	else if((OperationMode == STANDBY) && (RCstate == COMPLETE)){

    if(receivedID == 555.0){
      // Delays 5 seconds.
      delay(5000);

      OperationMode = NORMAL;
    }
	}
	// Each of the crafts have 5 seconds to broadcast.
	else if((millis() - start > 10000) && (OperationMode == NORMAL) && (RCstate == COMPLETE)){

		// Resets the counter. This disables broadcasting again until 15 seconds has passed.
		start = millis();
    
		// Sends the transmission via radio.
		Radio.broadcast();
	}
}


/**
 * Responsible for reading in signals over the radio antenna.
 */
void RADIO::radioReceive()
{
  // Checks if radio message has been received.
  if (rf95.available()){
  	// Creates a temporary varaible to read in the incoming transmission. 
  	uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  	
  	// Gets the length of the above temporary varaible.
  	uint8_t len = sizeof(buf);
      
    // Reads in the avaiable radio transmission, then checks if it is corrupt or complete.
    if(rf95.recv(buf, &len)) {
      // New info is being read in. 
      Data.newData = Data.YES;
  
      // Conversion from uint8_t to string. The purpose of this is to be able to convert to an 
      // unsigned char array for parsing. 
      String str = (char*)buf;
      char toParse[str.length()];
      str.toCharArray(toParse,str.length());

      // Used to display the received data in the GUI.
      radioInput = buf;
      
      // This whole section is comparing the currently held varaibles from the last radio update
      // to that of the newly received signal. Updates the craft's owned variables and copies
      // down the other nodes varaibles. If the timestamp indicates that this craft currently 
      // holds the most updated values for another node (ie: LoRa's time stamp is higher than the 
      // new signal's), it replaces those variables.
      
      // Reads in the time stamp for Mission Control's last broadcast.
      float tempMC = Radio.getTimeStamp(toParse, 5);
      
      // Compares the currently brought in time stamp to the one stored onboad.
      if(tempMC > Network.MC_TS){
        
        // If the incoming signal has more up-to-date versions, we overwrite our saved version with
        // the new ones.
        Network.MC_TS = tempMC;
        Network.StartStop = Radio.getStartStop(toParse);
        Network.TargetLat = Radio.getRadioTargetLat(toParse);
        Network.TargetLon = Radio.getRadioTargetLon(toParse);
        Network.TargetThrottle = Radio.getTargetThrottle(toParse);
      }

      // Reads in Craft ID to see where signal came from. 
      receivedID = Radio.getCraftID(toParse);
    }
	}
}


/**
 * Alters the craft ID of the radio transmission and broadcasts back to Mission Control.
 */
void RADIO::rollCall()
{
	// Updates the Craft_ID to Eagle Eye's specific ID #.
	Network.Craft_ID = 2.0;
	
	// Sends the transmission via radio.
	Radio.broadcast();
  
	// Updates Checked_In Status.
	checkedIn = true;

  // Updates craft states. 
  RCstate = COMPLETE;
  OperationMode = STANDBY;
}


/**
 * Responsible for sending out messages via the radio antenna.
 */
void RADIO::broadcast()
{
  // Updates the time object to hold the most current operation time.
  Network.L_TS = millis()/1000.0;
  
  // Updates the Networks struct to reflect the crafts most up-to-date postioning before
  // it broadcasts the signal on the network.
  Network.Altitude = Data.Local.Altitude;
  Network.Latitude = Data.Local.Latitude;
  Network.Longitude = Data.Local.Longitude;
  
  // Alter this line so when ground recieves, it resets Lora's LE to 0.
  Network.LE = Data.Local.LE;
  
  // Casting all float values to a character array with commas saved in between values
  // so the character array can be parsed when received by another craft.
  String temp = "";
  
  temp += Network.L_TS;
  temp += ",";
  temp += Network.Altitude;
  temp += ",";
  temp += Network.Latitude * 10000; // In order to pass more then 2 decimals, we multiply by 10000
                                    // on this end to preserve the wanted 6 bits (4 before, 2 after)
  temp += ",";
  temp += Network.Longitude * 10000; // In order to pass more then 2 decimals, we multiply by 10000
                                     // on this end to preserve the wanted 6 bits (4 before, 2 after)
  temp += ",";
  temp += Network.LE;
  temp += ",";
  temp += Network.MC_TS;
  temp += ",";
  temp += Network.StartStop;
  temp += ",";
  temp += Network.TargetLat * 10000;
  temp += ",";
  temp += Network.TargetLon * 10000;
  temp += ",";
  temp += Network.TargetThrottle;
  temp += ",";
  temp += Network.Craft_ID;

  // Copy contents. 
  radioOutput = temp;

  // New info is being read in. 
  Data.newData = Data.YES;
  
  // Converts from String to char array. 
  char transmission[temp.length()];
  temp.toCharArray(transmission, temp.length());
  
  // Sends message passed in as paramter via antenna.
  rf95.send(transmission, sizeof(transmission));
    
  // Pauses all operations until the micro controll has guaranteed the transmission of the
  // signal. 
  rf95.waitPacketSent();
}


/**
 * Returns the movement state of the craft in string format for UI and debugging. 
 */
String RADIO::getFunctionalSTATE(){
  float temp = Radio.Network.StartStop;

  if(temp == 0.0){
    return "Paused";
  }
  else if(temp == 1.0){
    return "Running";
  }
}


/**
 * Returns the operational state of the craft in string format for UI and debugging. 
 */
String RADIO::getOpSTATE(){
  float temp = Radio.OperationMode;

  if(temp == 0.0){
    return "NONE";
  }
  else if(temp == 1.0){
    return "ROLLCALL";
  }
  else if(temp == 2.0){
    return "STANDBY";
  }
  else if(temp == 3.0){
    return "NORMAL";
  }
}


/**
 * Returns the rollcall state of the craft in string format for UI and debugging. 
 */
String RADIO::getRCSTATE(){
  float temp = Radio.RCstate;

  if(temp == 0.0){
    return "NOTSTARTED";
  }
  else if(temp == 1.0){
    return "RUNNING";
  }
  else if(temp == 2.0){
    return "COMPLETE";
  }
}

