/**
 * GPS.cpp is repsonsible for capturing telemetry data.
 */


#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <math.h>
 
#include "GPS.h"
#include "DATA.h"
#include "Globals.h"


/**
 * Constructor used to reference all other variables & functions.
 */
GPS::GPS()
{
	
}


/**
 * Responsible for reading in a new and full NMEA sentence from the GPS module.
 */
void GPS::manager()
{
    // Forces the radio to wait at least a second for an incoming packet.
    if(millis() - gps_block > 5000)
    {
        // Reset timer.
        gps_block = millis();
        // Pull new data from GPS.
        retrieve_gps_data();
        // Checks for a valid satellite fix.
        if(fixation_monitor())
        {
            // If valid gps fix, take that gps data and assign it to the
            // correct onboard variables.
            store_data();
        }
        else
        {
            // If no fixation, reverts current values to
            // that of the previous cycle. This prevents the
            // craft from thinking its currently at 0 degrees Lat & Lng
            // and 0m in Altitude (which is bad).
            revert_gps_data();
        }
    }
}


/**
 * Checks for a valid gps satellite fix.
 * True = Yes. False = No.
 */
bool GPS::fixation_monitor()
{
    // Check for a GPS fix. (Does it have a signal from the satellites)
    if(gps.satellites.value() != 0 && gps_fix == false)
    {
        // GPS fix has been aquired.
        gps_fix = true;
        // Trigger onboard event detection.
        Data.craft_event = 1.0;
    }
    // Checks the correctness of the gps data. (Worthless if less than 5)
    if(gps.satellites.value() <  5)
    {
        // If the # of satellites drops to zero. GPS fix has been lost.
        if(gps.satellites.value() == 0)
        {
            // Triggers onboard event detection.
            Data.craft_event = 2.0;
            // Updates the fix status.
            gps_fix = false;
        }
    }
    // True = connected, False otherwise.
    return gps_fix;
}


/**
 * Pulls current data from GPS module.
 */
void GPS::retrieve_gps_data()
{
    // Opens up a serial connection between the micro controller and
    // the GPS breakout board at a certain baudrate.
    Serial1.begin(9600);
    // Reads in GPS data via serial port for 1 second.
    unsigned long gps_read_in_timer = millis();
    do
    {
        // Reads in data while it is available.
        while (Serial1.available())
        {
            // Stores the brought in data to the gps object.
            gps.encode(Serial1.read());
        }
    // Count down timer for 1 second.
    } while (millis() - gps_read_in_timer < 1000);
}


/**
 * Takes the new gps data and assigns it to the correct variables.
 */
void GPS::store_data()
{
    // Updates all struct variables with the most current sensor data.
    sprintf(gps_time, "%02d:%02d:%02d ", gps.time.hour(), gps.time.minute(), gps.time.second());
    gps_altitude = gps.altitude.meters();
    gps_latitude = gps.location.lat();
    gps_longitude = gps.location.lng();
    gps_satillite_count = gps.satellites.value();
    gps_speed = gps.speed.mps(); 
    gps_distance = calculate_target_distance();
    // Replaces the old backup values with the new values.
    previous_altitude = gps_altitude;
    previous_latitude = gps_latitude;
    previous_longitude = gps_longitude;
    previous_distance = gps_distance;
}


/**
 * Calculates the distance to the GPS Target in meters.
 */
float GPS::calculate_target_distance()
{

    float distance = (float)TinyGPSPlus::distanceBetween(gps.location.lat(), 
                                                         gps.location.lng(), 
                                                         target_latitude, 
                                                         target_longitude);
    return distance;
}


/**
 * Updates the current struct with the previous cycles values. Used incase of 
 * GPS loss or sensor error so the craft doesn't instantly think its at 0m altitude.
 */
void GPS::revert_gps_data()
{
    // Reverts values to that of the previous cycle.
    gps_altitude = previous_altitude;
    gps_latitude = previous_latitude;
    gps_longitude = previous_longitude;
    gps_speed = 0.0;
    gps_distance = previous_distance;
}
