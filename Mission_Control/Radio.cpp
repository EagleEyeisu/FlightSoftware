/**
 * Radio.ccp holds all functions related the radio port/module infused inside the LoRa FeatherWing 
 * development micro controller.
 */

#include <Arduino.h>
#include "Radio.h"
#include "Data.h"
#include "keyP.h"
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
	// If invalid connection, the program will stall and print an error message.
	if(!rf95.init())
    {
		Serial.println("LoRa radio init failed");
		while (1);
	}
	// Valid connection, program proceeds as planned.
	else
    {
		Serial.println("LoRa radio init OK!");
	}
	
	// Checks the radio objects set frequency. 
	// If invalid connection, the program will stall and print an error message.
	if(!rf95.setFrequency(RF95_FREQ))
    {
		Serial.println("setFrequency failed");
		while (1);
	}
	// Valid connection, program proceeds as planned.
	else
    {
		Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
	}
	// Sets the max power to be used to in the amplification of the signal being sent out.
	rf95.setTxPower(23, false);
	Serial.println();
}


/**
 * Manages all radio comms either incoming or outgoing.
 */
void RADIO::manager()
{
	// Reads in radio transmission if available.
	Radio.radioReceive();

    // Checks to see if its time for Roll Call. This gets updated in 
    if(Key.pressedKey == 9 || RCstate == RUNNING)
    {
        if(Key.pressedKey == 9)
        {
            // Updates RollCallStatus to running. 
            RCstate = RUNNING;
            // Updates overall radio state.
            OperationMode = ROLLCALL;
            // Calls function.
            Radio.rollCall();
        }
        // Checks for the stop signal to RollCall. 
        else if(Key.pressedKey == 8)
        {
            // Updates RollCallStatus to complete.
            RCstate = COMPLETE;
            // Updates overall radio state to standby. Not waiting for user to send start signal. 
            OperationMode = STANDBY;
        }
        // This else statement appears to have duplicate code from the above if statement. 
        // This is needed because it allows the program to run in RollCall mode without
        // being directly triggered, while checking for RollCall responses. 
        else
        {
            //Calls function. 
            Radio.rollCall();
        }
    }
 
	// After Roll Call is complete, Mission Control will broadcast the start signal. Appropriate delays are
	// distributed below to initally sync the network to a 5 second split. This makes for a 10 second revolution.
	// MC - starts instantly
	// EE - delays 5 seconds                                     //Key press 7 sends network start signal to nodes. 
	else if((RCstate == COMPLETE) && (Key.pressedKey == 7 && OperationMode == STANDBY))
    {
        // Updates Craft_ID to the network start signal. 
        Radio.Network.Craft_ID = 555.0;
        // Updates radio state.
        OperationMode = NORMAL;	
	}
	// Each of the 2 crafts have 5 seconds to broadcast. That means each craft will broadcast every 10 seconds.
	else if((millis() - start >= 10000) && (RCstate == COMPLETE) && (OperationMode == NORMAL))
    {
		// Resets the counter. This disabled broadcasting agian until 10 seconds has passed.
		start = millis();
		// Sends the transmission via radio.
		Radio.broadcast();
        // Switch start signal to craft ID. Normal operations have begun. 
        if(Network.Craft_ID == 555.0)
        {
            Network.Craft_ID = 1.0;
    	}
    }
}


/**
 * Checks for response from node after rollcall broadcast. If not found, adds to network.
 */
void RADIO::nodeCheckIn()
{
    // Checks for response from node after rollcall broadcast.
    if(receivedID != 0.0)
    {
        // Cycles through nodes that have already checked in.
        for(int i=0;i<10;i++)
        {
            if(nodeList[i] == receivedID)
            {
                break;
            }
            else if(nodeList[i] == 0.0)
            {
                // New info is being read in. 
                Data.newData = Data.YES;
                // If not found and an empty spot is found, it adds the node to the network. 
                nodeList[i] = receivedID;
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
    // Checks if radio message has been received.
    if (rf95.available())
    {
        // Creates a temporary varaible to read in the incoming transmission. 
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        // Gets the length of the above temporary varaible.
        uint8_t len = sizeof(buf);
        // Reads in the avaiable radio transmission.
        if(rf95.recv(buf, &len))
        {
            // New info is being read in. 
            Data.newData = Data.YES;
            //Used to display the received data in the GUI.
            radioInput = buf;
            Serial.println(radioInput);
            // Conversion from uint8_t to string. The purpose of this is to be able to convert to an 
            // unsigned char array for parsing. 
            String str = (char*)buf;
            char toParse[str.length()];
            str.toCharArray(toParse,str.length());

            // This whole section is comparing the currently held varaibles from the last radio update
            // to that of the newly received signal. Updates the LoRa's owned variables and copies
            // down the other nodes varaibles. If the time LoRa currently holds the most updated values
            // for another node (LoRa's time stamp is higher than the new signal's), it replaces those vars.
          
            // Reads in the time stamp for Mission Control's last broadcast.
            float temp_LoRa = Radio.getTimeStamp(toParse, 0);
          
            // Compares the currently brought in time stamp to the one stored onboad.
            if(temp_LoRa > Radio.Network.L_TS)
            {
                // If the incoming signal has more up-to-date versions, we overwrite our saved version with
                // the new ones.
                Network.L_TS = temp_LoRa;
                Network.Altitude = Radio.getRadioAltitude(toParse);
                Network.Latitude = Radio.getRadioLatitude(toParse);
                Network.Longitude = Radio.getRadioLongitude(toParse);
                Network.LE = Radio.getLoRaEvent(toParse);
            }

            // Reads in Craft ID to see where signal came from. 
            receivedID = Radio.getCraftID(toParse);

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
	Network.Craft_ID = 999.0;
    // Timer of 5 seconds. 
    if(millis() - RCBroadcast >= 5000)
    {
        // Resets the counter. This disabled rollcall broadcasting again until 5 seconds has passed.
        RCBroadcast = millis();
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
    Network.MC_TS = millis()/1000.0;
    // Casting all float values to a character array with commas saved in between values
    // so the character array can be parsed when received by another craft.
    String temp = "";

    temp += Network.L_TS;
    temp += ",";
    temp += Network.Altitude;
    temp += ",";
    temp += Network.Latitude * 10000;
    temp += ",";
    temp += Network.Longitude * 10000;
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
    // Blinks LED onboard of LoRa to signal keypad interaction. 
    blinkLED();
    // Sends message passed in as paramter via antenna.
    rf95.send(transmission, sizeof(transmission));
    // Pauses all operations until the micro controll has guaranteed the transmission of the
    // signal. 
    rf95.waitPacketSent();
}


/*
 * Blinks LED.
 */
void RADIO::blinkLED()
{
    // ON
    digitalWrite(LED, HIGH);
    delay(10);
    // OFF
    digitalWrite(LED, LOW);
}


/**
 * Returns the movement state of the craft in string format for UI and debugging. 
 */
String RADIO::getFunctionalSTATE()
{
    float temp = Radio.Network.StartStop;
    if(temp == 0.0)
    {
    return "Paused";
    }
    else if(temp == 1.0)
    {
    return "Running";
    }
}


/**
 * Returns the operational state of the craft in string format for UI and debugging. 
 */
String RADIO::getOpSTATE(){
    float temp = Radio.OperationMode;

    if(temp == 0.0)
    {
        return "NONE";
    }
    else if(temp == 1.0)
    {
        return "ROLLCALL";
    }
    else if(temp == 2.0)
    {
        return "STANDBY";
    }
    else if(temp == 3.0)
    {
        return "NORMAL";
    }
}


/**
 * Returns the rollcall state of the craft in string format for UI and debugging. 
 */
String RADIO::getRCSTATE()
{
    float temp = Radio.RCstate;

    if(temp == 0.0)
    {
        return "NOTSTARTED";
    }
    else if(temp == 1.0)
    {
        return "RUNNING";
    }
    else if(temp == 2.0)
    {
        return "COMPLETE";
    }
}