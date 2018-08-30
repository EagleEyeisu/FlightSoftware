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

    // Responsible for all serial communication to python GUI.
    void serial_comms();

    // Parses passed in message by using commas as the identifiers.
    float Parse(char message[], int objective);

    void gui_input();

    // Automatic or Manual control of craft. 
    // 0 - Manual
    // 1 - Authority
    int authority_mode = 0;

};

#endif
