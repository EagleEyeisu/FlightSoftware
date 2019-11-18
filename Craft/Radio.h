/**
 * Radio.h holds all declarations and varaibles related to the radio.ccp file.
 */

#ifndef Radio_h
#define Radio_h

#include <Arduino.h>

class RADIO
{
  	public:
    // Constructor
    RADIO();
    // Returns the transmission's craft's altitude.
    float get_radio_craft_altitude(char buf[]);
    // Returns the transmission's Latitude.
    float get_radio_craft_latitude(char buf[]);
    // Returns the transmission's longitude.
    float get_radio_craft_longitude(char buf[]);
    // Returns the transmission's craft event.
    float get_radio_craft_event(char buf[]);
    // Returns the transmission's craft anchor variable.
    float get_radio_craft_anchor(char buf[]);
    // Returns the transmission's craft ID.
    float get_radio_craft_id(char buf[]);
    // Returns the transmission's target throttle variable.
    float get_radio_target_throttle(char buf[]);
    // Parses and returns the radio Target Latitude.
    float get_radio_target_latitude(char buf[]);
    // Parses and returns the radio Target Longitude.
    float get_radio_target_longitude(char buf[]);
    // Returns the transmission's time stamp.
    float get_radio_timestamp(char buf[], String selector);
    // Returns the craft's anchor status.
    float get_radio_manual_direction(char buf[]);
    // Returns the transmission's reset flag.
    float get_radio_node_reset(char buf[]);
    // Returns the transmission's craft ID.
    float get_radio_node_id(char buf[]);
    // Runs initialzation script for the Radio.
    void initialize();
    // Passively watches for incoming radio transmissions from Mission Control and other crafts.
    void manager();
    // Receives incoming transmission.
    void radio_receive();
    // Constructs a netowrk packet.
    String construct_network_packet();
    // Sends the desired signal out over the radio antenna.
    void broadcast(String packet);
    // Checks if packet is valid or invalid. Error detection.
    bool validate_checksum();
    // Blinks the LED on the LoRa uC (quick blink).
    void blink_led();


  	/*---------------------------------Variables---------------------------------*/ 
	
    // Chip select pin for the radio.
    const byte RFM95_CS = 10;
    // Intialization pin for radio.
    const byte RFM95_INT = 6;
    // Reset pin onboard the radio.
    const byte RFM95_RST = 11;
    // Craft ID (Used to set start times)
    const byte NODE_ID = 2;
    // Pins used to blink an LED to signal a radio packet has been received.
    const byte LED = 13;
    // Radio frequency used throught the Eagle Eye Program.
    #define RF95_FREQ 433.0
    // Holds the ID of the craft that just broadcasted.  
    float received_id = 0.0;
	  // Holds the current received radio signal. 
    String radio_input = "";
    // Holds the current sent radio signal.
    String radio_output = "";
    // Holds the reset bit used to clear the timestamp of the associated node.
    float received_reset = 0.0;
    
    /**
     * These variables are overseen by Mission Control.
     */
    // Mission Control's ms Time stamp.
    float mission_control_ts = 0.0;
    // Craft's target throttle position. This is not what the craft is currently at, 
    // but what we want the craft's to have as its upper limit be. For example, it will not be 
    // at a constant 40% if we set it to '40.0', but it will be able to iterate up to that percentage
    // of thrust.
    float target_throttle = 0.0;

    /**
     * Overseen by the Eagle Eye craft.
     */
    // Epoch timer used to time stamp packets in the network. 
    float craft_ts = 0.0;
    /**
     * This varaible is updated by each craft right before the array is broadcasted.
     */
    // Craft_ID is used to tell which craft is currently broadcasting the signal. This allows
    // for Mission Control to have a sense of if information is being relayed through nodes,
    // or if we have a direct line of communication with each node.
    float craft_id = 0.0;

    // Used in the computation of the radio system. 
    unsigned long broadcast_timer = 0;
    // Each device has # milliseconds to talk.
    float network_node_delay = 1500.0;
};

#endif
