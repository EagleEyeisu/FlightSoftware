/*                        EAGLE EYE PROPRIETARY INFORMATION                          *
 *                                                                                   *
 * Purpose: Primary software for use on EAGLE EYE rotor craft. Controls and          *
 *          maintains motor speed, deploys parachute, reads and records atmospheric  *
 *          data.                                                                    *
 *                                                                                   *
 * Date:      Version:        Developer:                    Description:             *     
 * 10/17/16   1.0             Jared Danner/Wesley Carleton  Initial Build.           *                                                                                      
 * 11/16/16   1.1             Jared Danner                  Included Saftey ShutOff  *
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
  ESC1.attach(9);  //Adds ESC to certain pin. arm();
  Serial.begin(9600);// Starts the out put to the serial monitor.
}
/********************Main Program****************************
 * The program's core begins below where it sets the speed  *                       
 * of the ESC to 0% throttle. It than proceeds to increment *
 * the throttle up until it achieves 70% throttle. When     *
 * completed, the program than slowly increments the        *
 * throttle power sent to the ESC/motor back to 0.          *
 ************************************************************/
void loop(){
  int speed;              //Creates speed variable.
  //ESC1.write(180);      //Used to set throttle range of ESC.
  //Serial.println("180");
  //delay(5000);          //Used to set throttle range of ESC.
  ESC1.write(8);          //Begins program by sending what is eqivalent to 0% throttle.
  delay(8000);
  
  char killSwitch = 'k';  //Predefined character to intiate program shutdown.
  boolean shutOff = false;//True or false value that is triggered when the appropriate character is entered.
  char inputSignal;       //Used to hold character for shut down.
  int maxSpeedReached;    //Maximum speed reached during throttle up.
  
  inputSignal = Serial.read();                 //Reads input.
  if(inputSignal!=killSwitch){                 //Checks to see if shut down signal has been given.
    for(speed = 16; speed <= 126; speed += 1){ //Cycles speed up to 70% power with appropriate time delay.
      inputSignal = Serial.read();             //Reads in user input if given.
      if(inputSignal==killSwitch){             //If correct input is given 'k' program starts to shut down.
        shutOff = true;                        //Shut off value is set true.
        break;                                 //Breaks throttle up loop.
      }
      ESC1.write(speed);                       //Calls function setSpeed (above) and passes the variable speed to it.
      Serial.println(speed);                   //Prints speed variable to the serial monitor.
      delay(500);                              //Delays the program for one fifth of a second.
    }
    
    if(!shutOff){                              //Checks to see if shut down signal has been given.
      delay(4000);                             //Holds at 70% power for 4 seconds. If shut down signal is not given.
    }
    
    maxSpeedReached = speed-1;                   //Assigns maximum speed reached during throttle up;
    for(speed = maxSpeedReached; speed >= 16; speed -= 1) {  //Cycles speed down to 0% power with appropriate time delay.
      ESC1.write(speed);                                     //Calls function setSpeed (above) and passes the variable speed to it.
      Serial.println(speed);                                 //Prints speed variable to the serial monitor.
      delay(500);                                            //Delays the program for one fifth of a second.
    }
  }
  
 delay(10000000); //Delays program for a long time until the loop() starts over again.
}

 delay(1000); //Delays program for 1 second until the loop() starts over again.
}
