/**
 * MOTOR.h holds the code to operate the turbofans.
 */


#include <Arduino.h>
#include <Servo.h> 

#include "MOTOR.h"
#include "Radio.h"
#include "IMU.h"
#include "DATA.h"
#include "Globals.h"
 

/**
 * Constructor used to reference all other variables & functions.
 */
MOTOR::MOTOR()
{
 
}


/**
 * Based on the input given to the craft. This function calulates the desired
 * PDM pulse width to send to the motor.
 */ 
void MOTOR::manager()
{
    // Checks for flight mode "manual".
    if(Data.authority_mode == 0.0)
    {
        manual_flight();
    }
    // Checks for flight mode "auto".
    else if(Data.authority_mode == 1.0)
    {
        //auto_pilot();
    }
}

/**
 * Converts the inputted max throttle value to the apprpriate 
 * microsecond value. 
 */
int MOTOR::convert_throttle(int input)
{
    int throttle = input * 10;
    throttle = throttle + 980;
    return throttle;
}


/**
 * Translates the user's movement command into acutal movement.
 */
void MOTOR::manual_flight()
{   
    // The following are the commands and their representative integers.
    // 0 - No Movement.
    // 1 - Forward.
    // 2 - Backward.
    // 3 - Left.
    // 4 - Right.
    int command = Data.manual_direction;

    /**
     * Outer series of conditionals are focused on orienting the craft. Not propelling it.
     * Inner series are focused on movement.
     */ 

    // No Command / Stop Moving.
    if(command == 0)
    {
        // Check if the craft already in the No Command state.
        if(craft_state != STOP)
        {
            // Updates craft to none state.
            craft_state = STOP;
            // Shuts down EDFs.
            apply_break();
        }
    }
    // Move Forward.
    else if(command == 1)
    {
        // Check if the craft already in the moving forward state.
        if(craft_state != FORWARD)
        {
            // Updates the craft to the forward state.
            craft_state = FORWARD;
            // Shuts down EDFs.
            apply_break();
            // Throttles up the appropraite EDF's.
            move_forward();
        }
    }
    // Move Backward.
    else if(command == 2)
    {
        // Check if the craft already in the moving forward state.
        if(craft_state != BACKWARD)
        {
            // Updates the craft to the backward state.
            craft_state = BACKWARD;
            // Shuts down EDFs.
            apply_break();
            // Throttles up the appropraite EDF's.
            move_backward();
        }
    }
    // Turn Left.
    else if(command == 3)
    {
        // Check if the craft already in the turning left state.
        if(craft_state != LEFT)
        {
            // Updates the craft to the left state.
            craft_state = LEFT;
            // Shuts down EDFs.
            apply_break();
            // Zero point turn to the left.
            rotate_left();
        }
    }
    // Turn Right.
    else if(command == 4)
    {
        // Check if the craft is already in the turning right state.
        if(craft_state != RIGHT)
        {
            // Updates the craft to the right state.
            craft_state = RIGHT;
            // Shuts down EDFs.
            apply_break();
            // Zero point turn to the right.
            rotate_right();
        }
    }
}


/**
 * TO BE FULLY IMPLEMENTED @ A LATER DATE.
 */
void MOTOR::auto_pilot()
{
    // Movement of craft treated as a state machine. (STARTS AT STATE 1 EVERY CYCLE)
    //                      MOST IMPORTANT ---------------> LEAST IMPORTANT
    //
    //       State 1                State 2               State 3                State 3
    //   ---------------        ---------------        ---------------       ---------------
    //   |     No      |        |     Yaw     |        |   Forward   |       |   Vertical  |
    //   |  Movement   |   -->  |  Correction |   -->  | & Backward  |  -->  |  Correction |
    //   ---------------        ---------------        ---------------       ---------------

    // Checks for the need for Yaw Correction.
    if(Imu.turn_right || Imu.turn_left)
    {
        // Checks for the need to rotate Right.
        if(Imu.turn_right)
        {
            // Checks to see the crafts current movement.
            if(craft_state != RIGHT)
            {
                // Updates crafts current craft_state to reflect the change.
                craft_state = RIGHT;
                // Shuts down EDFs.
                apply_break();
                // Zero point turn to the right.
                rotate_right();
            }
        }
        // Checks for the need to rotate Left.
        else if(Imu.turn_left)
        {
            // Checks to see the crafts current movement.
            if(craft_state != LEFT)
            {
                // Updates crafts current craft_state to reflect the change.
                craft_state = LEFT;
                // Shuts down EDFs.
                apply_break();
                // Zero point turn to the left.
                rotate_left();
            }
        }
    }
    // Checks for the need to move the craft forward.
    else if(Imu.move_forward)
    {
        // Checks to see the crafts current movement.
        if(craft_state != FORWARD)
        {
            // Checks for the servo orientation. If previously rotationg, 
            // turns all servos to original position so all motors are
            // providing forward thrust.
            if((craft_state == RIGHT) || (craft_state == LEFT))
            {
                // Shuts down EDFs.
                apply_break();
                // Throttles up the appropraite EDF's.
                move_forward();
            }
            // Updates crafts current craft_state to reflect the change.
            craft_state = FORWARD;
        }
    }
}


/**
 * Tests connection to Turbofans.
 */
void MOTOR::initialize()
{
    // Initializes the crafts state to reflect its non moving state.
    craft_state = STOP;
    // Digitially connects the ESCs to their respective pins.
    motor_front_right.attach(A1);
    motor_front_left.attach(A2);
    motor_back_right.attach(A3);
    motor_back_left.attach(A4);
    //Makes sure all motors are set to below 0%
    motor_front_right.writeMicroseconds(700);
    motor_front_left.writeMicroseconds(700);
    motor_back_right.writeMicroseconds(700);
    motor_front_left.writeMicroseconds(700);
}


/**
 * Throttles up the front right and back left motors to rotate the craft left.
 */
void MOTOR::rotate_left()
{
    Serial.print("Turning Left: "); Serial.println(craft_state);
    // Ensures a max turning rate of 1%
    int throttle = 1010;//convert_throttle(1);
    // Ensures they have acheived the desired throttle and no higher.
    motor_front_right.writeMicroseconds(throttle);
    motor_back_left.writeMicroseconds(throttle);
}


/**
 * Throttles up the front left and back right motors to rotate the craft right.
 */
void MOTOR::rotate_right()
{
    Serial.print("Turning Left: "); Serial.println(craft_state); 
    // Ensures a max turning rate of 1%
    int throttle = 1010;//convert_throttle(1);
    // Ensures they have acheived the desired throttle and no higher.
    motor_front_left.writeMicroseconds(throttle);
    motor_back_right.writeMicroseconds(throttle);
}


/**
 * Acts as a break for the craft. When called, shuts off TurboFans.
 */
void MOTOR::apply_break()
{
  // Sends a PDM signal to the ESC's that will stop all rotation.
  motor_front_right.writeMicroseconds(900);
  motor_front_left.writeMicroseconds(900);
  motor_back_right.writeMicroseconds(900);
  motor_back_left.writeMicroseconds(900);
  // Allows the motors to stop before another potential command is sent to them.
  delay(50);
}


/**
 * Throttles up the front two motors to move the craft forwards
 */
void MOTOR::move_forward()
{
    Serial.print("Moving Forward: "); Serial.println(craft_state); 
    // Always throttles up from 0%. 
    current_throttle = 970;
    // Taks in a percentage value 0-100% from user and converts that to an appropriate milliseconds PWM value.
    int max_throttle = 1010; //convert_throttle(1);//Radio.target_throttle);
    // While less than max throttle, increment the throttle up. 
    while(max_throttle > current_throttle)
    {
        // Applies increment amount (1% throttle).
        current_throttle = current_throttle + INCREMENT_AMOUNT;
        // Debug messages.
        Serial.print("Current Throttle: "); Serial.println(current_throttle);
        Serial.print("Max Throttle: "); Serial.println(max_throttle);
        // Sends pwm waveform to the front 2 ESC's (motors).
        motor_front_left.writeMicroseconds(current_throttle);
        motor_front_right.writeMicroseconds(current_throttle);
        // The motors need a small amount of time to adjust to their new throttle before
        // being told another command.
        delay(INCREMENT_DELAY);
    }
    // Ensures they have acheived the desired throttle and no higher.
    motor_front_left.writeMicroseconds(current_throttle);
    motor_front_right.writeMicroseconds(current_throttle);
}


/**
 * Throttles up the back two motors to move the craft backwards.
 */
void MOTOR::move_backward()
{
    Serial.print("Moving Backward: "); Serial.println(craft_state); 
    // Always throttles up from 0%.
    current_throttle = 970;
    // Taks in a percentage value 0-100% from user and converts that to an appropriate milliseconds PWM value.
    int max_throttle = 1010;//Radio.target_throttle);
    // While less than max throttle, increment the throttle up. 
    while(max_throttle > current_throttle)
    {
        // Applies increment amount (1% throttle).
        current_throttle = current_throttle + INCREMENT_AMOUNT;
        // Debug messages.
        Serial.print("Current Throttle: "); Serial.println(current_throttle);
        Serial.print("Max Throttle: "); Serial.println(max_throttle);
        // Sends pwm waveform to the back 2 ESC's (motors).
        motor_back_left.writeMicroseconds(current_throttle);
        motor_back_right.writeMicroseconds(current_throttle);
        // The motors need a small amount of time to adjust to their new throttle before
        // being told another command.
        delay(INCREMENT_DELAY);
    }
    // Ensures they have acheived the desired throttle and no higher.
    motor_back_left.writeMicroseconds(current_throttle);
    motor_back_right.writeMicroseconds(current_throttle);
}


/**
 * Depending on craft operational state, returns the string associated
 * with that state instead of the numbers. This is purely for debugging
 * purposes. 
 */
String MOTOR::get_movement_state()
{  
    if(craft_state == 0)
    {
        return "STOP";
    }
    else if(craft_state == 1)
    {
        return "FORWARD";
    }
    else if(craft_state == 2)
    {
        return "BACKWARD";
    }
    else if(craft_state == 3)
    {
        return "LEFT";
    }
    else if(craft_state == 4)
    {
        return "RIGHT";
    }
}
