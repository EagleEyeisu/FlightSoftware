/*                        EAGLE EYE PROPRIETARY INFORMATION                          *
 *                                                                                   *
 * Purpose: Primary software for use on EAGLE EYE rotor craft. Controls and          *
 *          maintains motor speed, deploys parachute, reads and records atmospheric  *
 *          data.                                                                    *
 *                                                                                   *
 * Date:      Version:        Developer:                    Description:             *     
 * 10/17/16   1.0             Jared Danner/Wesley Carleton  Initial Build.           *                                                                                      *
 */

/********************General Over View***************************
 * The approach we took was to use the ESC exactly the same as  *
 * if we were using an actual servo motor and not a brushless   *
 * motor. This means we sent it angle degrees between 0 and 180.*
 ****************************************************************/
#include <Servo.h> 

Servo ESC1; //Creates an Servo object

/*
 * Function used during the setup process  
 * to attach the ESC to the appropriate pin 
 * number on the Arduino
 */
void setup() {
  ESC1.attach(9); //Adds ESC to certain pin. arm();
}

/********************Main Program****************************
 * The program's core begins below where it sets the speed  *                       
 * of the ESC to 0% throttle. It than proceeds to increment *
 * the throttle up until it achieves 70% throttle. When     *
 * completed, the program than slowly increments the        *
 * throttle power sent to the ESC/motor back to 0.          *
 ************************************************************/
void loop() {
  int speed; //Creates speed variable

  //setSpeed(100); //Used to set throttle range of ESC
  //delay(5000);   //Used to set throttle range of ESC
  ESC1.write(9);   //Begins program by sending what is eqivalent to 0% throttle.
  delay(8000);     

  for(speed = 0; speed <= 126; speed += 1) {  //Cycles speed up to 70% power with appropriate time delay.
    ESC1.write(speed);                        //Calls function setSpeed (above) and passes the variable speed to it.
    delay(500);                               //Delays the program for one fifth of a second.
  }
  
  delay(4000);                                //Holds at 70% power for 4 seconds.

  for(speed = 126; speed > 0; speed -= 1) {   //Cycles speed down to 0% power with appropriate time delay.
    ESC1.write(speed);                        //Calls function setSpeed (above) and passes the variable speed to it.
    delay(500);                               //Delays the program for one fifth of a second.
  }

  delay(1000); //Delays program for 1 second until the loop() starts over again.
}
