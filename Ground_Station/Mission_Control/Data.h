/**
 * Data.h is responsible for updating the programs main struct to the current most values.
 */
 
#ifndef DATA_h
#define DATA_h
 
#include <Arduino.h>

class DATA
{
    public:

    // Constructor
    DATA();
    // Sends appropriate infomation back to the GUI via serial. 
    void update_gui();
    // Reads in input from the GUI via serial. 
    void serial_input();
    // Parses passed in message by using commas as the identifiers.
    float Parse(int objective, String to_parse);

    /*---------------------------------Variables---------------------------------*/ 
    
    // Connection status to the GUI.
    bool gui_connection = false;
    // Timer used for sending serial updates to gui.
    unsigned long serial_timer = 0;
    // Set High for # (defined below) seconds on startup to tell other nodes in the system
    // that an unexpected power cycle has occurred and they need to clear 
    // their locally held variables.
    float node_reset = 0.0;
    // Number of seconds after startup that the reset_node will be held high.
    unsigned long startup_timer = 0;
    // Turns true after # seconds of running. Used to tell if the system started 
    // up correctly.
    bool system_boot_complete = false;
};

#endif
