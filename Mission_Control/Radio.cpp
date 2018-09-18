/**
 * Radio.ccp holds all functions related the radio port/module infused inside the LoRa FeatherWing 
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
 *    LoRa  -> 0
 *    MC    -> 5
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
float RADIO::getRadioTargetThrottle(char buf[])
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
	// Assigns pin 13 to have an output power connection to the LoRa's onboard LED.
	pinMode(LED, OUTPUT);
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
	if(!rf95.init())
    {
        // If invalid connection, the program will stall and pulse the onbaord led.
		while (1)
        {
            Radio.blink_led();
        }
	}
	// Checks the radio objects tuned frequency. 
	if(!rf95.setFrequency(RF95_FREQ))
    {
		// If invalid connection, the program will stall and pulse the onbaord led.
        while (1)
        {
            Radio.blink_led();
        }
	}
	// Sets the max power to be used to in the amplification of the signal being sent out.
	rf95.setTxPower(23, false);
}


/**
 * Manages all radio comms either incoming or outgoing.
 */
void RADIO::manager()
{
	// Reads in radio transmission if available.
	Radio.radioReceive();
    // Checks to see if its time to run Roll Call. Set via GUI. 
    if(operation_mode == Radio.ROLLCALL)
    {
        // Broadcasts startup packet.
        Radio.rollCall();
    }
	// After Roll Call is complete, Mission Control will broadcast the start signal.
	else if(operation_mode == Radio.STANDBY)
    {
        // Updates craft_id to the network start signal.
        Radio.Network.craft_id = 555.0;
	}
	// Each of the 2 crafts have 5 seconds to broadcast. That means each craft will broadcast every 10 seconds.
	else if((millis() - start >= 10000) && (operation_mode == Radio.NORMAL))
    {
		// Resets the counter. This disables broadcasting again until 10 seconds has passed.
		start = millis();
		// Sends the transmission via radio.
		Radio.broadcast();
        // Switch start signal to craft ID. Normal operations have begun. 
        if(Network.craft_id == 555.0)
        {
            Network.craft_id = 1.0;
    	}
    }
}


/**
 * Checks for response from node after rollcall broadcast. If not found, adds to network.
 */
void RADIO::nodeCheckIn()
{
    // Checks for response from node after rollcall broadcast.
    if(received_id != 0.0)
    {
        // Cycles through nodes that have already checked in.
        for(int i=0;i<2;i++)
        {
            // Checks to see if node has already checked in. Prevents duplicates.
            if(nodeList[i] == received_id)
            {   // If already found, discard repeated node.
                break;
            }
            else if(nodeList[i] == 0.0)
            {
                // If not found and an empty spot is found, it adds the node to the network. 
                nodeList[i] = received_id;
                break;
            }
        }
    }
}


/**
 * Responsible for reading in signals over the radio antenna.
 */
void RADIO::radioReceive()
{
    //Serial.println("Checking for packet...");
    // Checks if radio message has been received.
    if (rf95.available())
    {
        // Serial.println("Incoming packet...");
        // Creates a temporary varaible to read in the incoming transmission. 
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        // Gets the length of the above temporary varaible.
        uint8_t len = sizeof(buf);
        // Reads in the avaiable radio transmission.
        if(rf95.recv(buf, &len))
        {
            // Used to display the received data in the GUI.
            radio_input = buf;
            // Conversion from uint8_t to string. The purpose of this is to be able to convert to an 
            // unsigned char array for parsing. 
            String str = (char*)buf;
            char toParse[str.length()];
            str.toCharArray(toParse,str.length());

            // This whole section is comparing the currently held varaibles from the last radio update
            // to that of the newly received signal. Updates the LoRa's owned variables and copies
            // down the other nodes' varaibles. If the time LoRa currently holds the most updated values
            // for another node (LoRa's time stamp is higher than the new signal's), it replaces those vars.
          
            // Reads in the time stamp for Mission Control's last broadcast.
            float temp_LoRa = Radio.getTimeStamp(toParse, 0);
            // Compares the currently brought in time stamp to the one stored onboad.
            if(temp_LoRa > Radio.Network.craft_ts)
            {
                // If the incoming signal has more up-to-date versions, we overwrite our saved version with
                // the new ones.
                Network.craft_ts = temp_LoRa;
                Network.craft_altitude = Radio.getRadioAltitude(toParse);
                Network.craft_latitude = Radio.getRadioLatitude(toParse);
                Network.craft_longitude = Radio.getRadioLongitude(toParse);
                Network.craft_event = Radio.getLoRaEvent(toParse);
            }
            // Reads in Craft ID to see where signal came from. 
            received_id = Radio.getCraftID(toParse);
            // Compares the transmission's craftID to see if its a brand new craft. If so, it logs it. 
            Radio.nodeCheckIn();
        }
    }
}


/**
 * Alters the craft ID of the radio transmission and broadcasts back to Mission Control.
 */
void RADIO::rollCall()
{
	// Updates the Craft_ID to the network call in signal "999.0".
	Network.craft_id = 999.0;
    // Timer of 5 seconds.
    if(millis() - rc_broadcast >= 5000)
    {
        // Resets the counter. This disabled rollcall broadcasting again until 5 seconds has passed.
        rc_broadcast = millis();
        // Sends the transmission via radio.
        Radio.broadcast();
    }
}


/**
 * Creates an array to be sent out via Radio. Fills that array with correct values and returns it.
 */
void RADIO::broadcast()
{  
    // Updates the time object to hold the most current operation time.
    Network.home_ts = millis()/1000.0;
    // Casting all float values to a character array with commas saved in between values
    // so the character array can be parsed when received by another craft.
    String temp = "";
    temp += Network.craft_ts;
    temp += ",";
    temp += Network.craft_altitude;
    temp += ",";
    temp += Network.craft_latitude * 10000;
    temp += ",";
    temp += Network.craft_longitude * 10000;
    temp += ",";
    temp += Network.craft_event;
    temp += ",";
    temp += Network.home_ts;
    temp += ",";
    temp += Network.craft_anchor;
    temp += ",";
    temp += Network.target_latitude * 10000;
    temp += ",";
    temp += Network.target_longitude * 10000;
    temp += ",";
    temp += Network.target_throttle;
    temp += ",";
    temp += Network.craft_id;
    // Copy contents. 
    radio_output = temp;
    // Converts from String to char array. 
    char transmission[temp.length()];
    temp.toCharArray(transmission, temp.length());
    // Blinks LED onboard of LoRa to signal broadcast
    blink_led();
    blink_led();
    // Sends message passed in as paramter via antenna.
    rf95.send(transmission, sizeof(transmission));
    // Pauses all operations until the micro controll has guaranteed the transmission of the
    // signal. 
    rf95.waitPacketSent();
}


/*
 * Blinks LED.
 */
void RADIO::blink_led()
{
    // ON
    digitalWrite(LED, HIGH);
    delay(50);
    // OFF
    digitalWrite(LED, LOW);
}


/*
 * Blinks LED long duration.
 */
void RADIO::blink_led_long()
{
    // ON
    digitalWrite(LED, HIGH);
    delay(2000);
    // OFF
    digitalWrite(LED, LOW);
}
