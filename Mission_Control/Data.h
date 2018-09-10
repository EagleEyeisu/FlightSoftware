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

    // Parses serial input and returns the authority mode.
    float get_serial_authority_mode(char buf[]);

    // Parses serial input and returns the user's manual direction.
    float get_serial_direction(char buf[]);

    // Parses serial input and returns the target throttle.
    float get_serial_target_throttle(char buf[]);

    // Parses serial input and returns the anchor status.
    float get_serial_craft_anchor(char buf[])
};

#endif
