/**
 * MOTOR.h holds the code to operate the turbofans.
 */

 
#ifndef MOTOR_h
#define MOTOR_h

#include <Arduino.h>
#include <Servo.h>

class MOTOR
{
    public:
    // Constructor
    MOTOR();
    // Runs initialzation script for the TurboFans.
    void initialize();
    // Based on the input given to the craft. This function calulates the desired
    // PDM pulse width to send to the motor.
    void manager();
    // Moves the servos in coordination to rotate the craft forwards.
    void move_forward();
    // Moves the servos in coordination to rotate the craft backwards.
    void move_backward();
    // Moves the servos in coordination to rotate the craft right.
    void rotate_right();
    // Moves the servos in coordination to rotate the craft left.
    void rotate_left();
    // Compares the current throttle status to the "desired" throttle.
    // Increments the PDM signal if the craft needs to throttle up.
    void spin_up();
    // Writes a PDM Waveform that is below the cutoff. This takes the throttle to
    // an instant 0%.
    void apply_break();
    // Depending on craft operational state, returns the string associated
    // with that state instead of the numbers. This is purely for debugging
    // purposes. 
    String get_movement_state();
    // Manual flight mode directed from ground station.
    void manual_flight();
    // Autonomous flight mode.
    void auto_pilot();
    // Converts throttle from 0-100% to 900+ value for ESC's.
    int convert_throttle(int input);


    /*---------------------------------Variables---------------------------------*/ 

    // Connection pins from Feather m0 to their respective ESC's.
    #define MOTOR_FRONT_RIGHT_PIN 1
    #define MOTOR_FRONT_LEFT_PIN 2
    #define MOTOR_BACK_RIGHT_PIN 3
    #define MOTOR_BACK_LEFT_PIN 4
    // Specific amount to increment/decrement the PWM waveform between each pulse. 
    // This ensures that each increment or decrement is suttle
    // and therefore does not burn the motors out.
    const byte INCREMENT_AMOUNT = 10;
    // Delay time between consecutive pulses to the motors.
    #define INCREMENT_DELAY 320
    // Creates the motor objects.
    Servo motor_front_right;
    Servo motor_front_left;
    Servo motor_back_right;
    Servo motor_back_left;
    // Throttle value of the craft at any given point in time.
    int current_throttle = 900;
    // This variables it the desired throttle percentage value that 
    // takes in a "percentage (0-100)" and calculates the 
    // appropriate PWM waveform. 
    int throttle_input = 0;
    // Movement state of the craft. This is what the craft is currently doing
    // and not necessarily what it wants to do.
    enum Orientation {NONE,FORWARD,BACKWARD,LEFT,RIGHT};
    enum Orientation craft_state = NONE;
};

#endif
