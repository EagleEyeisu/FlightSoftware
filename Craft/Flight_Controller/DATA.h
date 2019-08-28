/**
 * DATA.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef DATA_H
#define DATA_H

#include <Arduino.h>

class DATA
{
    public:
    // Constuctor.
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
    float get_i2c_craft_altitude(char buf[]);
    // Reads in the latitude via i2c.
    float get_i2c_craft_latitude(char buf[]);
    // Reads in the longitude via i2c.
    float get_i2c_craft_longitude(char buf[]);
    // Reads in the taret altitude via i2c.
    float get_i2c_target_altitude(char buf[]);
    // Reads in the target latitude via i2c.
    float get_i2c_target_latitude(char buf[]);
    // Reads in the taret longitude via i2c.
    float get_i2c_target_longitude(char buf[]);
    // Reads in the taret distance via i2c.
    float get_i2c_target_distance(char buf[]);
    // Reads in the speed via i2c.
    float get_i2c_craft_speed(char buf[]);
    // Reads in the flight mode of the craft.
    float get_i2c_authority_mode(char buf[]);
    // Reads in the manual flight mode's directional flight command.
    float get_i2c_manual_command(char buf[]);
    // Reads in the craft's anchor state.
    float get_i2c_craft_anchor(char buf[]);
    // Reads in the craft's throttle variable.
    float get_i2c_target_throttle(char buf[]);
    // Runs initialzation script for the barometer.
    void initialize();


    /*---------------------------------Variables---------------------------------*/

    // State of Data.
    // This enum is used in the UI display process. It will watch all the variables
    // and if any have changed compared to the last cycle, it will switch to yes. 
    // When it is in the YES state, the toScreen() method will print out the newly
    // accuried data to the serial monitor. The purpose of this is have a readable UI
    // without having it update every few milliseconds. (Potential future solution is
    // would be to use an external terminal such as SuperPuTTy)
    enum dataState {NO, YES};
    enum dataState new_data = YES;

    /*----------------------Network Information----------------------*/

    // Automatic or Manual control of craft. 
    // 0 - Manual
    // 1 - Auto
    float authority_mode = 0.0;
    // Directional command for when craft is manual state.
    float manual_direction = 0.0;
    // Anchor status of the craft. (Works as an E Brake)
    float anchor_status = 0.0;
    

    /*------------------------Mega Information------------------------*/

    // Altitude of the craft.
    float ftlctrl_altitude = 0.0;
    // Roll value of the craft.
    float ftlctrl_roll = 0.0;
    // Pitch value of the craft.
    float ftlctrl_pitch = 0.0;
    // Yaw value of the craft.
    float ftlctrl_yaw = 0.0;
    // External atmosphereic pressure.
    float ftlctrl_pressure = 0.0;
    // External temperature of craft.
    float ftlctrl_external_temperature = 0.0;


    /*------------------------LoRa Information------------------------*/

    // Altitude of the craft. (Brought over from LoRa)
    float radio_craft_altitude = 0.0;
    // Latitude of the craft. (Brought over from LoRa)
    float radio_craft_latitude = 0.0;
    // Longitude of the craft. (Brought over from LoRa)
    float radio_craft_longitude = 0.0; 
    // Target Altitude (Brought over from LoRa)
    float radio_target_altitude = 0.0;
    // Target Latitude (Brought over from LoRa)
    float radio_target_latitude = 0.0;
    // Target Longitude (Brought over from LoRa)
    float radio_target_longitude = 0.0;
    // Holds the craft's target throttle position. This is not what the craft is currently at, 
    // but what we want the craft's to have its upper limit be. For example, it will not be 
    // at a constant 40% if we set it to '40.0', but it will be able to iterate up and down from
    // that percentage of thrust. 
    float radio_target_throttle = 0.0;
    // Current distance to target in meters. (Brought over from LoRa)
    float radio_target_distance = 0.0;
    // Speed in meters per second. (Brought over from LoRa)
    float radio_craft_speed = 0.0;
};

#endif
