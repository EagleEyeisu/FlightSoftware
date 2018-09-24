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

    // Rotates all servos to a specific inputted degree angle.
    void rotate_level(int target_degree);

    // Moves the servos in coordination to rotate the craft right.
    void rotate_right();

    // Moves the servos in coordination to rotate the craft left.
    void rotate_left();

    // Compares the current throttle status to the "desired" throttle.
    // Increments the PDM signal if the craft needs to throttle up.
    void spin_up();

    // Tethered Flight surpise for everyone who can't read code.
    void beam_me_up_scotty();

    // Compares the current throttle status to the "desired" throttle.
    // Decrements the PDM signal if the craft needs to throttle down.
    void spin_down();

    // Writes a PDM Waveform that is below the cutoff. This takes the throttle to
    // an instant 0%.
    void apply_break();

    // Depending on craft operational state, returns the string associated
    // with that state instead of the numbers. This is purely for debugging
    // purposes. 
    String get_movement_state();

    /*-----------------------------------------------------------------------------*/ 

    // Connection pins from Arduino MEGA to their respective ESC's.
    // The craft has four motors, but we can get away with only 
    // simulating one motor due to them otherwise being parallel
    // tasks and taking up memory.
    #define MOTOR_PIN 36

    // Connection pins from the Arduino MEGA to the servos. 
    // Like stated above for the turbofans, we have four 
    // physical serovs on the craft, but only need to 
    // simulate two due to each side acting in tandem.
    #define SERVO_PIN_LEFT 42
    #define SERVO_PIN_RIGHT 51

    // Specific amount to increment/decrement the PWM waveform between each pulse. 
    // This ensures that each increment or decrement is suttle
    // and therefore does not burn the motors out.
    const byte INCREMENT_AMOUNT = 10;

    // Delay time between consecutive pulses to the motors.
    #define INCREMENT_DELAY 320

    // Delay time between consecutive pulses to the servos.
    const byte SERVO_DELAY = 5;

    // Creates a Servo object
    Servo motor;

    // Creates a Servo object
    Servo servo_left;
    Servo servo_right;

    // Throttle value of the craft at any given point in time.
    double current_throttle = 900;

    // This variables it the desired throttle percentage value that 
    // takes in a "percentage (0-100)" and calculates the 
    // appropriate PWM waveform. 
    int throttle_input = 0;

    // The degree of the servo from 0 to 180.
    int servo_degree = 0; 

    enum Orientation {NONE,RIGHT,LEFT,FORWARD,UP,BREAK};
    enum Orientation craft_state = NONE;
};

#endif

