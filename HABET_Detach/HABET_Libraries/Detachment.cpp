/**
 * Detachment.cpp is repsonsible for releasing the craft when certain senarios occur.
 */


#include "Detachment.h"


/**
 * Assigns correct pins to the servo output port & initializes its position.
 */
void Detachment::Servo_Initialize()
{
	//Assigns pin '11' to the phyiscal servo.
	detach_servo.attach(11);
	
	//Assigns the starting position for the servo.
	detach_servo.write(90);
}


/**
 * Releases the craft when the conditions are met.
 */
void Detachment::Servo_Manager()
{
	//Checks certain varaibles that correspond with triggering the detachment of Eagle Eye from HABET.
	if((Network.Command_Sent ==  || Network.LE == 2) && Network.Release_Status == 0){
	
		//Swings the servo from a 90 degree angle to a 160 degree angle.
		//   Releases the craft from HABET.
		for(int i=90;i<160;i++){
			
			//Increments the angle upward towards 160.
			Detachment.write(i);
			
			//Delays 10ms or 1/100th of a second to allow the servo to move before
			//   before it receives its next move command.
			delay(10);
			
		}
		
		Serial.println("Detached");
	}
}


