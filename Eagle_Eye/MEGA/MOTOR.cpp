/**
 * MOTOR.h holds the code to operate the turbofans.
 */


#include "Globals.h"
#include "MOTOR.h"
#include "IMU.h"
#include "DATA.h"
#include <Arduino.h>
#include <Servo.h> 
 

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
    // The turn (right, left) variables are what the craft needs to do to correct its course. 
    // The craft_state variable (left, right, etc..) is the action that the craft it currently preforming. 

    // Checks for a positive anchor status. (E BREAK)
    if(Data.Local.craft_anchor_status == true)
    {
        // Checks if the break is already applied. If so, no need to apply it again.
        if(craft_state != BREAK)
        {
            craft_state = BREAK;
            // Pulls the PDM Waveform below its cutoff to shut the motors off.
            // This is to prevent any extra force on the servo to Turbofan housing.
            apply_break();
            delay(100);
        }
    }
    // Achor is not dropped. Craft is free to move.
    else
    {
        // Checks for flight mode "manual".
        if(Data.Local.authority_mode == 0.0)
        {
            manual_flight();
        }
        // Checks for flight mode "auto".
        else if(Data.Local.authority_mode == 1.0)
        {
            auto_pilot();
        }
    }
}


/**
 * Handles manual flight. Slimmed down version of auto pilot that uses explicit direction commands
 * vs a lot of complicated math.
 */
void MOTOR::manual_flight()
{   
    int command = Data.Local.craft_manual_direction;

    // The following are the commands and their representative integers.
    // 0 - No Movement.
    // 1 - Forward.
    // 2 - Left.
    // 3 - Right.
    // 4 - Up.

    // First series of conditionals are focused on orienting the craft. Not propelling it.
    // Second series are focused on movement.

    // No Command / Stop Moving.
    if(command == 0)
    {
        if(craft_state != NONE)
        {
            craft_state = NONE;
            apply_break();
            rotate_level(90);
        }
    }
    // Move Forward.
    else if(command == 1)
    {
        if(craft_state != FORWARD)
        {
            craft_state = FORWARD;
            apply_break();
            rotate_level(0);
        }
        else if(craft_state == FORWARD)
        {
        	if (Data.Local.craft_anchor_status == false)
        	{
        		spin_up();
        	}
        }
    }
    // Turn Left.
    else if(command == 2)
    {
        if(craft_state != LEFT)
        {
            craft_state = LEFT;
            apply_break();
            rotate_left();
            delay(100);
        }
        else if(craft_state == LEFT)
        {
            if (Data.Local.craft_anchor_status == false)
        	{
        		spin_up();
        	}
        }
    }
    // Turn Right.
    else if(command == 3)
    {
        if(craft_state != RIGHT)
        {
            craft_state = RIGHT;
            apply_break();
            rotate_right();
            delay(100);
        }
        else if(craft_state == RIGHT)
        {
            if (Data.Local.craft_anchor_status == false)
        	{
        		spin_up();
        	}
        }
    }
    // Turn Right.
    else if(command == 4)
    {
        if(craft_state != UP)
        {
            craft_state = UP;
            apply_break();
            rotate_level(90);
            delay(100);
        }
        else if(craft_state == UP)
        {
            // I hope this works. This is probably where we break the craft.
            if (Data.Local.craft_anchor_status == false)
        	{
        		spin_up();
        	}
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
                // Pulls the PDM Waveform below its cutoff to shut the motors off.
                // This is to prevent any extra force on the servo to Turbofan housing.
                apply_break();
                // Rotates the craft clockwise.
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
                // Pulls the PDM Waveform below its cutoff to shut the motors off.
                // This is to prevent any extra force on the servo to Turbofan housing.
                apply_break();
                // Rotates the craft counter clockwise.
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
            //   providing forward thrust.
            if((craft_state == RIGHT) || (craft_state == LEFT))
            {
                // Pulls the PDM Waveform below its cutoff to shut the motors off.
                // This is to prevent any extra force on the servo to Turbofan housing.
                apply_break();
                // Rotates all servos back to their original position so that all turbofans
                // are providing forward thrust.
                rotate_level(0);
            }
            // Updates crafts current craft_state to reflect the change.
            craft_state = FORWARD;
        }
    }

    // Checks for the need to move up or down
    else if(Imu.move_up)
    {
        // Pulls the PDM Waveform below its cutoff to shut the motors off.
        // This is to prevent any extra force on the servo to Turbofan housing.
        apply_break();

        // Checks to see the crafts current movement
        if(craft_state != UP)
        {
            // Checks for if craft is already moving up. 
            if(Imu.move_up)
            {
                // Rotates all servos back to their original position so that all turbofans
                // are pointing up.
                rotate_level(90);
                spin_up();
                // Updates craft's current craft_state to reflect the change
                craft_state = UP;
            }  
        }
    }
}


/**
 * Tests connection to Turbofans.
 */
void MOTOR::initialize()
{
    // Initializes the crafts state to reflect its non moving state.
    craft_state = NONE;
    // Digitially connects the ESCs to their respective pins.
    motor.attach(MOTOR_PIN);
    // Begins program by sending a PDM waveform that is below its cutoff value. So 0% targetThrottle.
    motor.writeMicroseconds(900);
    // Digitally connects the servos to their respective pins.
    servo_left.attach(SERVO_PIN_LEFT);
    servo_right.attach(SERVO_PIN_RIGHT);
    // Initializes all EDF's to an upright position. 
    servo_left.write(90);
    servo_right.write(90);
    servo_degree = 90;
}


/**
 * If true moves the servos in coordination to rotate the craft right.
 */
void MOTOR::rotate_right()
{
    // Serial.println("Turning Right: ");
    // Serial.println(state);
    // Applies those values to the servo. Rotates each side counter to each other.
    for(servo_degree = servo_degree; servo_degree > 0; servo_degree -= 1)
    {
        Serial.println(servo_degree);
        servo_left.write(servo_degree);
        servo_right.write(servo_degree);
        delay(SERVO_DELAY);
    }
}


/**
 * If true moves the servos in coordination to rotate the craft left.
 */
void MOTOR::rotate_left()
{
    // Serial.println("Turning Left: ");
    // Serial.println(state); 
    // Applies values to the servo. Rotates each side counter to each other.
    for(servo_degree = servo_degree; servo_degree < 170; servo_degree += 1)
    {
        Serial.println(servo_degree);
        servo_left.write(servo_degree);
        servo_right.write(servo_degree);
        delay(SERVO_DELAY);
    }
}


/**
 * Rotates the servos so all the turbofans are oriented forward.
 * Input is the degree angle (whole number) between 0 and 180. 
 */
void MOTOR::rotate_level(int target_degree)
{
    // Checks if current position is below the given degree.
    if(servo_degree < target_degree)
    {
        // Iterates up to the degree mark.
        while(servo_degree < target_degree)
        {
            // Increments the position varaible.
            servo_degree = servo_degree + 1;

            // Writes the values to the servos.
            Serial.println(servo_degree);
            servo_left.write(180-servo_degree);
            servo_right.write(servo_degree);
            delay(SERVO_DELAY);
        }
    }
    // Checks if motor position is above the given degree.
    else if(servo_degree > target_degree)
    {
        // Iterates down to the degree mark.
        while(servo_degree > target_degree)
        {
            // Decrements the position variable.
            servo_degree = servo_degree - 1;

            // Writes the values to the servos.
            Serial.println(servo_degree);
            servo_left.write(180-servo_degree);
            servo_right.write(servo_degree);
            delay(SERVO_DELAY);
        }
    }
}


/**
 * Acts as a break for the craft. When called, shuts off TurboFans.
 */
void MOTOR::apply_break()
{
  // Sends a PDM signal to the ESC's that will kill all rotation.
  // Used to stop the motors before every servo movement
  // to decrease the amount of axial force applied to the servo/turbofan
  // attachment.
  motor.writeMicroseconds(900);
  delay(100);
}


/**
 * Compares the current throttle status to the "desired" throttle.
 * Increments the PDM signal if the craft needs to throttle up.
 */
void MOTOR::spin_up()
{
    float max_throttle = Data.Local.lora_target_throttle;
    while(max_throttle > current_throttle)
    {
        current_throttle = current_throttle + INCREMENT_AMOUNT;
        Serial.println(current_throttle);
        motor.writeMicroseconds(current_throttle);

        Serial.println(max_throttle);
        delay(INCREMENT_DELAY);
    }
    motor.writeMicroseconds(current_throttle);
}


/**
 * Tethered Flight surprise.
 */
void MOTOR::beam_me_up_scotty()
{
    float max_throttle = Data.Local.lora_target_throttle;
    while(max_throttle > current_throttle)
    {
        current_throttle = current_throttle + INCREMENT_AMOUNT;
        Serial.println(current_throttle);
        motor.writeMicroseconds(current_throttle);

        Serial.println(max_throttle);
        delay(INCREMENT_DELAY);
    }
    motor.writeMicroseconds(current_throttle);
}


/**
 * Compares the current throttle status to the "desired" throttle.
 * Decrements the PDM signal if the craft needs to throttle down.
 */
void MOTOR::spin_down()
{
    float max_throttle = 900.00;
    while(current_throttle >= max_throttle)
    {
        current_throttle = current_throttle - INCREMENT_AMOUNT * 2;
        Serial.println(current_throttle);
        motor.writeMicroseconds(current_throttle);

        Serial.println(max_throttle);
        delay(INCREMENT_DELAY);
    }
    // Ensures shutoff.
    motor.writeMicroseconds(900);
}


/**
 * Depending on craft operational state, returns the string associated
 * with that state instead of the numbers. This is purely for debugging
 * purposes. 
 */
String MOTOR::get_movement_state()
{  
    int temp_state = Movement.craft_state;

    if(temp_state == 0)
    {
        return "NONE";
    }
    else if(temp_state == 1)
    {
        return "RIGHT";
    }
    else if(temp_state == 2)
    {
        return "LEFT";
    }
    else if(temp_state == 3)
    {
        return "FORWARD";
    }
    else if(temp_state == 4)
    {
        return "UP";
    }
    else if(temp_state == 5)
    {
        return "BREAK";
    }
}
