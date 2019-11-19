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

    /*----------------------------Method Declarations----------------------------*/
    
    /*------------------------------Packet Retreival-----------------------------*/
    
    // Returns the packet's time stamp.
    float get_radio_timestamp(String selector);
    // Returns the user's given direction.
    float get_radio_manual_direction();

    /*------------------------------General Methods------------------------------*/
    
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

  	/*---------------------------------Variables---------------------------------*/ 
    
    /*-------------------------------System Config-------------------------------*/
	
    // Connection pins to radio.
    const byte RFM95_RST = 11;
    const byte RFM95_INT = 6;
    const byte RFM95_CS = 10;
    // Craft ID (Used to set start times)
    const byte NODE_ID = 2;
    // Radio frequency used throught the Eagle Eye Program.
    #define RF95_FREQ 433.0
    // Used in the computation of the radio system. 
    unsigned long broadcast_timer = 0;
    // Each device has # milliseconds to talk.
    float network_node_delay = 1000.0;

    /*-------------------------------Network Meta Data-------------------------------*/
    
    // Holds the ID of the craft that just broadcasted.  
    float received_id = 0.0;
	  // Holds the current received radio signal. 
    String radio_input = "";
    // Holds the current sent radio signal.
    String radio_output = "";
    // Holds the reset bit used to clear the timestamp of the associated node.
    float received_reset = 0.0;
    
    /*-------------------------------Packet Data-------------------------------*/

    // Epoch timer used to time stamp packets in the network. 
    float craft_ts = 0.0;
    
    // Mission Control's ms Time stamp.
    float mission_control_ts = 0.0;
    // Craft's target throttle position. This is not what the craft is currently at, 
    // but what we want the craft's to have as its upper limit be. For example, it will not be 
    // at a constant 40% if we set it to '40.0', but it will be able to iterate up to that percentage
    // of thrust.
    float target_throttle = 0.0;
};

#endif
