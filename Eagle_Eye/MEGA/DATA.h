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
    float calculate_barometer_altitude(float _Pressure);

    // Reads in the altitude via i2c.
    float get_i2c_current_altitude();

    // Reads in the latitude via i2c.
    float get_i2c_current_latitude();

    // Reads in the longitude via i2c.
    float get_i2c_current_longitude();

    // Reads in the taret altitude via i2c.
    float get_i2c_destination_altitude();

    // Reads in the target latitude via i2c.
    float get_i2c_destination_latitude();

    // Reads in the taret longitude via i2c.
    float get_i2c_destination_longitude();

    // Reads in the taret distance via i2c.
    float get_i2c_destination_distance();

    // Reads in the speed via i2c.
    float get_i2c_current_speed();

    // Reads in the time of the LoRa via i2c.
    void get_i2c_current_timestamp();

    // Runs initialzation script for the barometer.
    void initialize();

    // State of Data.
    // This enum is used in the UI display process. It will watch all the variables
    // and if any have changed compared to the last cycle, it will switch to yes. 
    // When it is in the YES state, the toScreen() method will print out the newly
    // accuried data to the serial monitor. The purpose of this is have a readable UI
    // without having it update every few milliseconds. (Potential future solution is
    // would be to use an external terminal such as SuperPuTTy)
    enum dataState {NO, YES};
    enum dataState new_data = YES; // Initializes to yes to begin. Prints init UI.

    // Stores all of Eagle Eye's current flight data.
    // The difference between this struct and the one initalized in Radio.h is that this information
    // is saves gathered/used/saved locally. The Radio.h struct holds all the network information
    // being passed between crafts (nodes).
    struct Flight_Data
    {
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

        // Mega Event. This is assigned as needed throughout the program. Signals the Mega had a 
        // specific event.
        float craft_mega_event = 0.0;

        // lora

        // LoRa Event. This is assigned as needed throughout the program. Signals a specific event.
        float craft_lora_event = 0.0;

        // Altitude of the craft. (Brought over from LoRa)
        float GPSAltitude = 0.0;

        // Latitude of the craft. (Brought over from LoRa)
        float Latitude = 0.0;

        // Longitude of the craft. (Brought over from LoRa)
        float Longitude = 0.0; 

        // Target Altitude (Brought over from LoRa)
        float GPSTargetAlt = 0.0;

        // Target Latitude (Brought over from LoRa)
        float GPSTargetLat = 0.0;

        // Target Longitude (Brought over from LoRa)
        float GPSTargetLon = 0.0;

        // Current distance to target in meters. (Brought over from LoRa)
        float GPSTargetDistance = 0.0;

        // Speed in meters per second. (Brought over from LoRa)
        float GPSSpeed = 0.0;

        // Time of flight. (Brought over from LoRa)
        char GPSTime[10];
    };
    struct Flight_Data Local;
};

#endif

