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

    // Returns the transmission's Altitude.
    float getRadioAltitude(char buf[]);
      
    // Returns the transmission's CR variable.
    float getStartStop(char buf[]);

    // Returns the transmission's CS variable.
    float getRadioTargetThrottle(char buf[]);
      
    // Returns the transmission's craft ID.
    float getCraftID(char buf[]);
      
    // Returns the transmission's Latitude.
    float getRadioLatitude(char buf[]);
      
    // Returns the transmission's Longitude.
    float getRadioLongitude(char buf[]);
      
    // Returns the transmission's Latitude.
    float getLoRaEvent(char buf[]);

    // Parses and returns the radio Target Latitude.
    float getRadioTargetLat(char buf[]);

    // Parses and returns the radio Target Latitude.
    float getRadioTargetLon(char buf[]);

    // Returns the transmission's time stamp.
    float getTimeStamp(char buf[], int selector);

    // Runs initialzation script for the Radio.
    void initialize();

    // Passively watches for incoming radio transmissions from Mission Control and other crafts.
    void manager();

    // Receives incoming transmission.
    void radioReceive();

    // Responds to the RollCall signal sent from Mission Control.
    void rollCall();

    // Sends the desired signal out over the radio antenna.
    void broadcast();

    // Blinks the LED on the LoRa uC to show a signal has been received.
    void blink_led();

    // Compares current node against others. Prevents duplicates. 
    void nodeCheckIn();

    // Returns craft movement state in string format for UI. 
    String getFunctionalSTATE();

    // Returns craft operational state in string format for UI. 
    String getOpSTATE();

    // Returns craft Rollcall state in string format for UI. 
    String getRCSTATE();

    /*-----------------------------------------------------------------------------*/

    // Chip select pin for the radio.
    const byte RFM95_CS = 8;

    // Intialization pin for radio.
    const byte RFM95_INT = 7;

    // Reset pin onboard the radio.
    const byte RFM95_RST = 4;

    // Craft ID (Used to set start times)
    const byte NODE_ID = 1;

    // Interrupt pin to trigger RollCall.
    const byte INTERRUPT_ROLLCALL_PIN = 3;

    // Pins used to blink an LED to signal receival packet.
    const byte LED = 13;

    // Radio frequency used throught the Eagle Eye Program. CHECK WITH HABET BEFORE EACH FLIGHT!!!!!
    #define RF95_FREQ 433.0

    // Holds the ID of the craft that just broadcasted. THIS IS ANOTHER NODE, NOT MISSION CONTROL. 
    float received_id = 0.0;

    // List of nodes currently logged into network. 
    // MC - 1
    // EE - 2
    float nodeList[2] = {0.0, 1.0};

    // State of Radio program. 
    // ROLLCALL - Currently in RollCall process. 
    // STANDBY  - RollCall completed, waiting for user to send out start signal. 
    // NORMAL   - Radio is running in its normal operation state. 
    enum RadioStatus {NONE, ROLLCALL, STANDBY, NORMAL};
    enum RadioStatus operation_mode = NONE;

    // Holds the current received radio signal. 
    String radio_input = "";

    // Holds the current sent radio signal.
    String radio_output = "";

    // Stores all information related to the network of the Eagle Eye program.
    // This struct reads specific indexes and than rebroadcasts the updated transmission to
    // the other nodes in the network. 
    struct Network_Data {
    	

        /**
         * These variables are modified by the network admin operating the mission_control node. 
         */

        // Automatic or Manual control of craft. 
        // 0 - Manual
        // 1 - Authority
        float authority_mode = 0.0;


    	/**
    	 * This set of varaibles are accessed and overseen by the Eagle Eye craft.
    	 */
    	
    	// Each of these is defined in the Data.h struct. Refer to its documentation as needed.
    	float craft_ts = 0.0;
    	float craft_altitude = 0.0;
    	float craft_latitude = 0.0;
    	float craft_longitude = 0.0;
    	float craft_event = 0.0;
    	
    	/**
    	 * These variables are overseen by Mission Control.
    	 */
    	
        // Mission Control's ms Time stamp.
        float home_ts = 0.0;
  
        // This variable is the on / off switch for the craft as far as operational movement goes.
        // Having this be on the off state (0.0) does not stop data collection, only servo and 
        // motor movement. 
        float craft_anchor = 0.0;

        // User inputted target latitude for the craft. 
        float target_latitude = 0.0;

        // User inputted target longitude for the craft. 
        float target_longitude = 0.0;

        // Holds the craft's target throttle position. This is not what the craft is currently at, 
        // but what we want the craft's to have its upper limit be. For example, it will not be 
        // at a constant 40% if we set it to '40.0', but it will be able to iterate up and down from
        // that percentage of thrust. 
        float target_throttle = 0.0;

        // Movement of craft dicated by the driver while in manual mode. Independent of throttle.
        // 0 - Stopped
        // 1 - Forward
        // 2 - Right
        // 3 - Backward
        // 4 - Left
        // 5 - Down
        // 6 - Upward
        float target_direction = 0.0;
    	
    	/**
    	 * This varaible is updated by each craft right before the array is broadcasted.
    	 */
    	
    	// Craft_ID is used to tell which craft is currently broadcasting the signal. This allows
    	// for Mission Control to have a sense of if information is being relayed through nodes,
    	// or if we have a direct line of communication with each node.
    	float craft_id = 0.0;
    };
    struct Network_Data Network;

    // Timer is used to for the 10 second interval that the craft will broadcast when in normal. 
    // operating mode. This value is in milliseconds.  
    unsigned long start = 0;

    // Timer is used to for the 10 second interval that the craft will broadcast on for RollCall. 
    // This value is in milliseconds.  
    unsigned long rc_broadcast = 0;

    // Used to house node objects. 
    struct Network_Node {

        // General status of node.
        // 0 - Offline
        // 1 - Online
        // 2 - Error State
        float node_status = 0.0;

        // TBD @ a later date. Heart beat of node.
        float last_contact = 0.0;
    };
    // Node declarations.
    struct Network_Node mc_node;
    struct Network_Node ee_node;
    struct Network_Node relay_node;
};

#endif
