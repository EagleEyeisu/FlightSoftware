/**
 * MOTOR.h holds the code to operate the turbofans.
 */


#include "Globals.h"
#include "MOTOR.h"
#include "IMU.h"
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
 *    PDM pulse width to send to the motor.
 */ 
void MOTOR::manager()
{
  //Movement of craft treated as a state machine. (STARTS AT STATE 1 EVERY CYCLE)
  //
  //       State 1                State 2               State 3                State 3
  //   ---------------        ---------------        ---------------       ---------------
  //   |     No      |        |     Yaw     |        |   Forward   |       |   Vertical  |
  //   |  Movement   |   -->  |  Correction |   -->  | & Backward  |  -->  |  Correction |
  //   ---------------        ---------------        ---------------       ---------------
  
  //Checks for the need for Yaw Correction.
  if(turnRight || turnLeft){

    //Checks for the need to rotate Right.
    if(turnRight){

      //Checks to see the crafts current movement.
      if(state != RIGHT){

        //Updates crafts current state to reflect the change.
        state = RIGHT;

        //Pulls the PDM Waveform below its cutoff to shut the motors off.
        //   This is to prevent any extra force on the servo to Turbofan housing.
        applyBreak();

        //Rotates the craft clockwise.
        rotateRight();
      }
    }
    //Checks for the need to rotate Left.
    else if(turnLeft){

      //Checks to see the crafts current movement.
      if(state != LEFT){

        //Updates crafts current state to reflect the change.
        state = LEFT;

        //Pulls the PDM Waveform below its cutoff to shut the motors off.
        //   This is to prevent any extra force on the servo to Turbofan housing.
        applyBreak();

        //Rotates the craft counter clockwise.
        rotateLeft();
      }
    }
  }
  //Checks for the need to move the craft forward.
  else if(Imu.moveForward){

    //Checks to see the crafts current movement.
    if(state != FORWARD){

      //Checks for the servo orientation. If previously rotationg, 
      //   turns all servos to original position so all motors are
      //   providing forward thrust.
      if((state == RIGHT) || (state == LEFT)){

        //Pulls the PDM Waveform below its cutoff to shut the motors off.
        //   This is to prevent any extra force on the servo to Turbofan housing.
        applyBreak();

        //Rotates all servos back to their original position so that all turbofans
        //   are providing forward thrust.
        rotateLevel();
      }
      
      //Updates crafts current state to reflect the change.
      state = FORWARD;
    }
  }
  
  
  
  
  //The below code will be used for manual testing. 
  /*
  if(input < 1){
    throttle = 750;
  }
  if(input < 5){
    fraction = (input-1)/4.0;
    throttle = 760.0+(fraction*(800.0-760.0));
  }
  if(input <= 50){
    fraction = (input-5.0)/45.0;
    throttle = 800.0+(fraction*(450.0));
  }
  if(throttle > currentThrottle){
    Spin_Up();
  }
  else if(throttle < currentThrottle){
    Spin_Down();
  }
  */
}


/**
 * Tests connection to Turbofans.
 */
void MOTOR::initialize()
{
  //Initializes the crafts state to reflect its non moving state.
  state = NONE;
  
  //Digitially connects the ESCs to their respective pins.
  motor.attach(MOTOR_PIN);  
  
  //Begins program by sending a PDM waveform that is below its cutoff value. So 0% targetThrottle.
  motor.writeMicroseconds(900); 
  
  //Digitally connects the servos to their respective pins.
  servoRight.attach(SERVO_PIN1); // Adds servo to a certain pin
  servoLeft.attach(SERVO_PIN2); // Adds servo to a certain pin
  
}


/**
 * If true moves the servos in coordination to rotate the craft right.
 */
void MOTOR::rotateRight()
{
  //Updates the state machine to reflect the current movement.
  state = RIGHT;
  //Serial.println("Turning Right");
  
  //Applies those values to the servo. Rotates each side counter to each other.
  for (pos = pos; pos >= 0; pos -= 1){
    servoRight.write(pos);
    servoLeft.write(170-pos);
    delay(50);
  }

  //Spins motors back to constant turning speed of 5%. (set for 1% for testing purposes)
  //motor.writeMicroseconds(1000);
}


/**
 * If true moves the servos in coordination to rotate the craft left.
 */
void MOTOR::rotateLeft()
{
  //Updates the state machine to reflect the current movement.
  state = LEFT;
  //Serial.println("Turning Left"); 
  
  //Applies values to the servo. Rotates each side counter to each other.
  for (pos = pos; pos <= 170; pos += 1){
    servoLeft.write(pos);
    servoRight.write(170-pos);
    delay(50);
  }

  //Gives short delay to ensure servos reach destination.
  delay(250);

  //Spins motors back to constant turning speed of 5%. (set for 1% for testing purposes)
  //motor.writeMicroseconds(1000);
}


/**
 * Rotates the servos so all the turbofans are oriented forward.
 */
void MOTOR::rotateLevel()
{
  //Checks if current position is below 75 degrees.
  if (pos<75){

    //Iterates up to the 75 degree mark.
    while (pos<75){

      //Increments the position varaible.
      pos = pos + 1;

      //Writes the values to the servos.
      servoRight.write(pos);
      servoLeft.write(pos-170);
      delay(10);
    }
  }
  //Checks if motor position is above 75 degrees.
  else if (pos>75){

    //Iterates down to the 75th degree mark.
    while(pos>75){
      //Decrements the position variable.
      pos = pos - 1;
  
      //Writes the values to the servos.
      servoRight.write(pos);
      servoLeft.write(pos-170);
      delay(10);
    }
  }
}


/**
 * Acts as a break for the craft. When called, throttles down all TurboFans.
 */
void MOTOR::applyBreak()
{
  //Sends a PDM signal to the ESC's that will kill all rotation.
  //   Used to either stop the motors, or before every servo movement
  //   to decrease the amount of axial force applid to the servo/turbofan
  //   attachment.
  motor.writeMicroseconds(900);
  delay(100);
}


/**
 * Compares the current throttle status to the "desired" throttle.
 *    Increments the PDM signal if the craft needs to throttle up.
 */
 /*
void MOTOR::spinUp()
{
  while(throttle > currentThrottle){

    Serial.println(currentThrottle + INCREMENT_AMOUNT);
    motor.writeMicroseconds(currentThrottle + INCREMENT_AMOUNT);
    currentThrottle = currentThrottle + INCREMENT_AMOUNT;
     
    Serial.println(throttle);
    delay(INCREMENT_DELAY);
     
  }
    
  motor.writeMicroseconds(throttle);
  currentThrottle = throttle;
}


/**
 * Compares the current throttle status to the "desired" throttle.
 *    Decrements the PDM signal if the craft needs to throttle down.
 */
 /*
void MOTOR::spinDown()
{
  while(currentThrottle > throttle){

    Serial.println(currentThrottle - INCREMENT_AMOUNT);
    motor.writeMicroseconds(currentThrottle - INCREMENT_AMOUNT);
    currentThrottle = currentThrottle - INCREMENT_AMOUNT;
    
    Serial.println(throttle);
    delay(INCREMENT_DELAY);
  }
  
  motor.writeMicroseconds(throttle);
  currentThrottle = throttle;
}*/
