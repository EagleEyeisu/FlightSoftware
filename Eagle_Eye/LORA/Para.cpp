/**
 * Para.cpp handels the enablement/deployment of the parachute
 *    onboard the Eagle Eye Craft.
 */


#include "Para.h"
#include "Data.h"
#include "I2C.h"
#include <Arduino.h>


/**
 * Constructor used to reference all other variables & functions.
 */
Para::Para()
{
	
}
 
 
/**
 * Assigns correct pin to connect to the Parachute's Relay.
 */
void Para::Initialize()
{
	//Sets the pin RELAY1 to LOW to induce a open circuit therfore preventing a premature deployment
	//  in the next line of code.
	digitalWrite(RELAY1, LOW);
	
	//Assigns RELAY1 as the output communication pin to the Parachute.
	pinMode(RELAY1, OUTPUT);
	Serial.println("Parachute Online.");
}


/**
 * Primary purpose is passively be aware of the crafts situation, altitude, and orientation 
 *    and arm/deploy the parachute at the right conditions.
 */
void Para::Manager(class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in)
{
  Data = Data_in;
  GPS = GPS_in;
  I2C = I2C_in;
  //Para = Para_in;
  Radio = Radio_in;
  Save = Save_in;
  
	//Passively observes the change in altitude and when above a cset altitude, ENABLES the chute.
	if (!chute_enable && Data.Local.Altitude >= PARACHUTE_ARM_HEIGHT) {  
		
		//Counter used to make sure the craft stays above the arm height for a certain amount of time before it 
		//   arms. Purpose is to prevent the craft rising just above the height and arming and than dropping below
		//   and deploying while the craft is still going up. (would cause problems)
		saftey_counter++;
		
		//Checks for the saftey counter described above to reach a certain amount.
		if (saftey_counter >= 4) {
			
			//ENABLES the parachute.
			chute_enable = true;
			Serial.print("ENABLED: "); Serial.print(Data::Local.Altitude); Serial.println(" meters ");
			
			//Sends ENABLEMENT event to Mega.
			I2C::Transfer(1);
		
			//Assigned the LoRa Event to reflect the chute being ENABLED.
			Data.Local.LE = 1;
		}
		
		//Checks for the craft to drop below the required enable height.
		else if (Data.Local.Altitude <= PARACHUTE_ARM_HEIGHT) {
			
			//Resets saftey counter to 0. 
			saftey_counter = 0;
			Serial.println("Saftey reset to 0.");
			
			//Assigned the LoRa Event to reflect the chute being DISABLED.
			Data.Local.LE = 0;
		}
	}
	
	//Passively observes the change in altitude and when dropping below a set altitude, DEPLOYS the parachute.
	if (!chute_deploy && chute_enable && Data.Local.Altitude <= PARACHUTE_DEPLOY_HEIGHT) {
		
		
		//Closes the circuit, therefore providing power to the deploy the parachute.
		digitalWrite(RELAY1, HIGH);
		chute_deploy = true;
		
		//Sends DEPLOYMENT event to Mega.
		I2C::Transfer(2);
		
		//Assigns the LoRa Event to reflect the chute being DEPLOYED.
		Data.Local.LE = 2;
		
		Serial.print("DEPLOYED: "); Serial.print(Data::Local.Altitude); Serial.println(" meters");
		
		
		//Provides power for 2 seconds. Than cuts power and opens the circuit.
		delay(2000);
		digitalWrite(RELAY1, LOW);
	}
}
