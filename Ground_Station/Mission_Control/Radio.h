/**
 * Radio.h holds all declarations and varaibles related to the radio.ccp file.
 */

#ifndef RADIO_h
#define RADIO_h

#include <Arduino.h>

class RADIO
{
    public:
    // Constructor
    RADIO();

    /*----------------------------Method Declarations----------------------------*/
    
    /*------------------------------Packet Retreival-----------------------------*/
    // Returns packet time stamp.
    float get_radio_timestamp(String selector);
    // Returns craft's roll data.
    float get_radio_roll();
    // Returns craft's pitch data.
    float get_radio_pitch();
    // Returns craft's yaw data.
    float get_radio_yaw();
    // Returns craft's current movement state.
    float get_radio_craft_state();
    // Returns the craft's anchor status.
    float get_radio_manual_direction();
    // Returns the transmission's craft anchor variable.
    float get_radio_craft_anchor();
    // Returns the transmission's target throttle variable.
    float get_radio_target_throttle();

    
    /*------------------------------General Methods------------------------------*/
    
    // Runs initialzation script for the Radio.
    void initialize();
    // Passively watches for incoming radio transmissions from Mission Control and other crafts.
    void manager();
    // Constructs a normal netowrk packet.
    String construct_network_packet();
    // Sends the desired signal out over the radio antenna.
    void broadcast(String packet);
    // Receives incoming transmission.
    void radio_receive();
    // Checks if packet is valid or invalid. Error detection.
    bool validate_checksum();

    /*---------------------------------Variables---------------------------------*/

    /*-------------------------------System Config-------------------------------*/
    
    // Connection pins to radio.
    const byte RFM95_RST = 11;
    const byte RFM95_INT = 6;
    const byte RFM95_CS = 10;
    // Radio frequency used throught the Eagle Eye Program.
    #define RF95_FREQ 433.0
    // Craft ID (Mission Control LoRa.)
    float node_id = 1.0;
    // Holds the delay amount between this nodes broadcast window.
    float network_node_delay = 1000.0;
    // Timer is used to for the 10 second interval that the craft will broadcast when in normal.
    // operating mode. This value is in milliseconds.
    unsigned long broadcast_timer = 0;

    /*-------------------------------Network Meta Data-------------------------------*/
    
    // Holds the ID of the craft that just broadcasted. THIS IS ANOTHER NODE, NOT MISSION CONTROL.
    float received_id = 0.0;
    // Holds the reset bit used to clear the timestamp of the associated node.
    float received_reset = 0.0;
    // Holds the current received radio signal.
    String radio_input = "";
    // Holds the current sent radio signal.
    String radio_output = "";
    // Holds the most recent received singal's rssi value.
    float received_rssi = 0.0;

    /*-------------------------------Packet Data-------------------------------*/
    
    // Time stamp since craft's epoch.
    float craft_ts = 0.0;
    // Gyro data of craft.
    float roll = 0.0;
    float pitch = 0.0;
    float yaw = 0.0;
    // What the craft is doing. (Moving forward, turning right etc..).
    float craft_state = 0.0;
    
    // Time stamp since mission control's epoch.
    float mission_control_ts = 0.0;
    // What we want the craft to do.
    // 0 - Stopped
    // 1 - Forward
    // 2 - Backward
    // 3 - Left
    // 4 - Right
    float manual_direction = 0.0;
    // Anchor status of the craft. (Works as an E Brake)
    float anchor_status = 0.0;
    // Max throttle of craft's edfs.
    float target_throttle = 0.0;
};

#endif
