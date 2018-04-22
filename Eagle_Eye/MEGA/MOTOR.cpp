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

  //rotateLevel(180);
  // The turn (right, left) variables are what the craft needs to do to correct its course. 
  // The state variable (left, right, etc..) is the action that the craft it currently preforming. 

  
  //Checks for the need for Yaw Correction.
  if(Imu.turnRight || Imu.turnLeft){

    //Checks for the need to rotate Right.
    if(Imu.turnRight){

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
    else if(Imu.turnLeft){

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
        rotateLevel(0);
      }
      
      //Updates crafts current state to reflect the change.
      state = FORWARD;
    }
  }
 
  //checks for the need to move up or down
  else if(Imu.moveUp || Imu.moveDown){

    //Pulls the PDM Waveform below its cutoff to shut the motors off.
    //   This is to prevent any extra force on the servo to Turbofan housing.
    applyBreak();

    //checks to see the crafts current movement
    if(state != UP){
      
      //Checks for if craft is already moving up. 
      if(Imu.moveUp){

        //Rotates all servos back to their original position so that all turbofans
        //  are pointing up.
        rotateLevel(90);
    
        //spinUp();

        //updates crafts current state to reflect the change
        state = UP;
      }  
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
  servoLeft.attach(SERVO_PINLeft); // Adds servo to a certain pin
  servoRight.attach(SERVO_PINRight); // Adds servo to a certain pin

  servoLeft.write(90);
  servoRight.write(90);
  pos = 90;

}


/**
 * If true moves the servos in coordination to rotate the craft right.
 */
void MOTOR::rotateRight()
{
  //Updates the state machine to reflect the current movement.
  state = RIGHT;
  //Serial.println("Turning Right: ");
  //Serial.println(state);
  
  //Applies those values to the servo. Rotates each side counter to each other.
  for (pos = pos; pos > 0; pos -= 1){
    Serial.println(pos);
    servoLeft.write(pos);
    servoRight.write(pos);
    delay(SERVO_DELAY);
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
  //Serial.println("Turning Left: ");
  //Serial.println(state); 
  
  //Applies values to the servo. Rotates each side counter to each other.
  for (pos = pos; pos < 170; pos += 1){
    Serial.println(pos);
    servoLeft.write(pos);
    servoRight.write(pos);
    delay(SERVO_DELAY);
  }

  //Spins motors back to constant turning speed of 5%. (set for 1% for testing purposes)
  //motor.writeMicroseconds(1000);
}


/**
 * Rotates the servos so all the turbofans are oriented forward.
 *    Input is the degree angle (whole number) between 0 and 180. 
 */
void MOTOR::rotateLevel(int degree)
{
  //Checks if current position is below the given degree.
  if (pos<degree){
    
    //Iterates up to the degree mark.
    while (pos<degree){
      //Increments the position varaible.
      pos = pos + 1;
      
      //Writes the values to the servos.
      Serial.println(pos);
      servoLeft.write(180-pos);
      servoRight.write(pos);
      delay(SERVO_DELAY);
    }
  }
  //Checks if motor position is above the given degree.
  else if (pos>degree){
    
    //Iterates down to the degree mark.
    while(pos>degree){
      //Decrements the position variable.
      pos = pos - 1;
      
      //Writes the values to the servos.
      Serial.println(pos);
      servoLeft.write(180-pos);
      servoRight.write(pos);
      delay(SERVO_DELAY);
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

void MOTOR::spinUp()
{
  throttle = 1050;
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
 
void MOTOR::spinDown()
{
  throttle = 900;
  while(currentThrottle > throttle){

    Serial.println(currentThrottle - INCREMENT_AMOUNT);
    motor.writeMicroseconds(currentThrottle - INCREMENT_AMOUNT);
    currentThrottle = currentThrottle - INCREMENT_AMOUNT;
    
    Serial.println(throttle);
    delay(INCREMENT_DELAY);
  }
  
  motor.writeMicroseconds(throttle);
  currentThrottle = throttle;
}


/**
 * Depending on craft operational state, returns the string associated
 *    with that state instead of the numbers. This is purely for debugging
 *    purposes. 
 */
String MOTOR::getSTATE(){
  
  int temp = Movement.state;

  if(temp == 0){
    return "NONE";
  }
  else if(temp == 1){
    return "RIGHT";
  }
  else if(temp == 2){
    return "LEFT";
  }
  else if(temp == 3){
    return "FORWARD";
  }
  else if(temp == 4){
    return "UP";
  }
  else if(temp == 5){
    return "DOWN";
  }
}
