/**
 * IMU.h library file.
 */

#ifndef IMU_h
#define IMU_h

#include <Arduino.h>

class IMU
{
    public:
    // Constructor
    IMU();
    // Runs initialzation script for the IMU.
    void initialize();
    // Responsible relaying to the LoRa that the craft is in the correct orientation to detach.
    void manager();
    // Returns the crafts current Roll angle.
    float get_roll();
    // Returns the current Pitch angle.
    float get_pitch();
    // Returns the current Yaw angle.
    float get_yaw();
    // Returns the difference between target and current heading and switches turn booleans accordingly.
    void calculate_target_heading();
    // Returns difference between target and current altitudes and switches up/down booleans accordingly.
    void check_altitude_tolerance();
    // Compares current altitude against target altitude. Sets corresponding booleans.
    void check_distance_tolerance();


    /*---------------------------------Variables---------------------------------*/

    // Current angle to the target.
    float target_heading = 0.0;
    // Current bearing for craft.
    float craft_heading = 0.0;
    // Tolerance for heading angle.
    float target_heading_tolerance = 10.0;
    // Tolerance for difference in altitudes.
    float target_altitude_tolerance = 200.0;
    // Tolerance for distance to target.
    float target_distance_tolerance = 20.0;
    // Booleans to determine right/left turning action.
    bool turn_right = false;
    bool turn_left = false;
    // Booleans to determine upward/downward motion action.
    bool move_up = false;
    // Booleans to determine forward motion action.
    bool move_forward = true;  
};

#endif
