/**
 * DATA.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef DATA_H
#define DATA_H

#include <Arduino.h>

class DATA
{
    public:

    // Constuctor
    DATA();

    // Parses passed in message by using commas as the identifiers.
    float Parse(char message[], int objective);

    // Responsible for pulling current sensor data from peripherals.
    void update_data();

    // Prints all info to the screen in the organized fasion. 
    void to_screen();

    // Reads in external pressure and calculates altitude.
    float calculate_barometer_altitude();

    // Captures a barometric pressure sample and sets the pressure variable.
    void set_pressure();

    // Reads in the altitude via i2c.
    float get_i2c_current_altitude();

    // Reads in the latitude via i2c.
    float get_i2c_current_latitude();

    // Reads in the longitude via i2c.
    float get_i2c_current_longitude();

    // Reads in the taret altitude via i2c.
    float get_i2c_target_altitude();

    // Reads in the target latitude via i2c.
    float get_i2c_target_latitude();

    // Reads in the taret longitude via i2c.
    float get_i2c_target_longitude();

    // Reads in the taret distance via i2c.
    float get_i2c_destination_distance();

    // Reads in the speed via i2c.
    float get_i2c_current_speed();

    // Reads in the flight mode of the craft.
    float get_i2c_authority_mode();

    // Reads in the manual flight mode's directional flight command.
    float get_i2c_manual_command();

    // Reads in the craft's anchor state.
    float get_i2c_craft_anchor();

    // Runs initialzation script for the barometer.
    void initialize();

    /*-----------------------------------------------------------------------------*/

    // State of Data.
    // This enum is used in the UI display process. It will watch all the variables
    // and if any have changed compared to the last cycle, it will switch to yes. 
    // When it is in the YES state, the toScreen() method will print out the newly
    // accuried data to the serial monitor. The purpose of this is have a readable UI
    // without having it update every few milliseconds. (Potential future solution is
    // would be to use an external terminal such as SuperPuTTy)
    enum dataState {NO, YES};
    enum dataState new_data = YES;

    // Stores all of Eagle Eye's current flight data.
    // The difference between this struct and the one initalized in Radio.h is that this information
    // is saves gathered/used/saved locally. The Radio.h struct holds all the network information
    // being passed between crafts (nodes).
    struct Flight_Data
    {

        /*----------------------Network Information----------------------*/

        // Automatic or Manual control of craft. 
        // 0 - Manual
        // 1 - Auto
        float authority_mode = 0.0;


        /*------------------------Craft Information------------------------*/

        // Directional command for when craft is manual state.
        float craft_manual_direction = 0.0;

        // Anchor status of the craft. (Works as an E Brake)
        float craft_anchor_status = 0.0;
        

        /*------------------------Mega Information------------------------*/

        // Altitude of the craft.
        float mega_altitude = 0.0;

        // Roll value of the craft.
        float mega_roll = 0.0;

        // Pitch value of the craft.
        float mega_pitch = 0.0;

        // Yaw value of the craft.
        float mega_yaw = 0.0;

        // External atmosphereic pressure.
        float mega_pressure = 0.0;

        // External temperature of craft.
        float mega_external_temperature = 0.0;


        /*------------------------LoRa Information------------------------*/

        // Altitude of the craft. (Brought over from LoRa)
        float lora_current_altitude = 0.0;

        // Latitude of the craft. (Brought over from LoRa)
        float lora_current_latitude = 0.0;

        // Longitude of the craft. (Brought over from LoRa)
        float lora_current_longitude = 0.0; 

        // Target Altitude (Brought over from LoRa)
        float lora_target_altitude = 0.0;

        // Target Latitude (Brought over from LoRa)
        float lora_target_latitude = 0.0;

        // Target Longitude (Brought over from LoRa)
        float lora_target_longitude = 0.0;

        // Holds the craft's target throttle position. This is not what the craft is currently at, 
        // but what we want the craft's to have its upper limit be. For example, it will not be 
        // at a constant 40% if we set it to '40.0', but it will be able to iterate up and down from
        // that percentage of thrust. 
        float lora_target_throttle = 0.0;

        // Current distance to target in meters. (Brought over from LoRa)
        float lora_destination_distance = 0.0;

        // Speed in meters per second. (Brought over from LoRa)
        float lora_current_speed = 0.0;
    };
    struct Flight_Data Local;
};

#endif
