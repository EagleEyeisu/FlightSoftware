/*                        EAGLE EYE PROPRIETARY INFORMATION                          *
 *                                                                                   *
 * Purpose: Primary software for use on EAGLE EYE rotor craft. Controls and          *
 *          maintains motor speed, deploys parachute, reads and records atmospheric  *
 *          data.                                                                    *
 *                                                                                   *
 * Date:      Version:        Developer:                    Description:             *     
 * 10/17/16   1.0             Jared Danner/Wesley Carlton   Initial Build.           *                                                                                      *
*/

/********************General Over View**************************
 * The approach we took was to use the ESC exactly the same as *
 * if we were using an actual servo motor and not a brushless  *
 * motor. This is why below we convert the angle to an angle   *
 * before telling the ESC what to do.                          *
 ***************************************************************/
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

/*
 * Function is used in throttling up and down
 * the ESC's output power to the motor. The
 * function takes an input value and turns 
 * that number into an angle measure between
 * 0 and 180.
 */
void setSpeed(int speed){
  int angle = map(speed, 0, 100, 0, 180); //Porportionatly turns speed value of 0-100 into an angle measure between 0-180.
  ESC1.write(angle);                      //Pushes that angle measure to the ESC
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
  setSpeed(5);     //Begins program by sending what is eqivalent to 0% throttle.
  delay(8000);     

  for(speed = 0; speed <= 70; speed += 1) { //Cycles speed up to 70% power with appropriate time delay.
    setSpeed(speed);                        //Calls function setSpeed (above) and passes the variable speed to it.
    delay(200);                             //Delays the program for one fifth of a second.
  }
  
  delay(4000);                              //Holds at 70% power for 4 seconds.

  for(speed = 70; speed > 0; speed -= 1) {  //Cycles speed down to 0% power with appropriate time delay.
    setSpeed(speed);                        //Calls function setSpeed (above) and passes the variable speed to it.
    delay(200);                             //Delays the program for one fifth of a second.
  }

  delay(1000); //Delays program for 1 second until the loop() starts over again.
}
