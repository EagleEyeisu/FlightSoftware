/**
 * DATA.cpp contains the struct that holds all the Crafts situational information.
 */


#include <Arduino.h>
#include "DATA.h"


/**
 * Constructor used to reference all other variables & functions.
 */
DATA::DATA()
{
	
}


/**
 * Updates the main struct for the craft. 
 */
void DATA::manager()
{
	//Creates new 'event' with the most current pressure sensor data.
	bmp.getEvent(&event);

	Local.Altitude = getAltitude(event.pressure);
	Local.TempExt = getTempExt();
	Local.Pressure = event.pressure;
	Local.Roll = getRoll();
	Local.Pitch = getPitch();
	Local.Yaw = getYaw();
	//Local.LE and Local.ME update on their own throughout the program & are reset to 0 after being saved.

	//Prints out data struct to the screen for debugging/following alone purposes.
	Serial.print("Altitude:    "); 		Serial.print(Local.Altitude); Serial.println(" m");
	Serial.print("Temperature: "); 		Serial.print(Local.TempExt); Serial.println(" C");
	Serial.print("Pressure:    "); 		Serial.print(Local.Pressure); Serial.println(" hPa");
	Serial.print("Roll:  "); 			Serial.println(Local.Roll);
	Serial.print("Pitch: "); 			Serial.println(Local.Pitch);
	Serial.print("Yaw:   "); 			Serial.println(Local.Yaw);
	Serial.print("LoRa Event: "); 		Serial.println(Local.LE);
	Serial.print("Mega Event: "); 		Serial.println(Local.ME);
	Serial.print("-------------------------------------------");
	Serial.println();
}


/**
 * Derives Crafts altitude based on current atmosphereic pressure.
 *    DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD TO BY JAMES/JARED.
 */
float getAltitude(float _Pressure)
{
	//Converts the incoming pressure (hPa) into (mPa).
	float pressure = _Pressure / 10.0;
	
	//Altitude value produced from the equations below.
	float alt;
	
	//These variables represent parts of the equation. Think of it like this...
	//   (following symbols are not representative of what happens below)   
	//
	//                   leftTop +- rightTop
	//                   -------------------        (leftTop and rightTop combined = topTotal)
	//                         bottom
	float leftTop;
	float rightTop;
	float bottom;
	float topTotal;

	//ABOVE 25,000m
	if (pressure < 2.55) {                         
		leftTop = -47454.8;
		rightTop = pow(pressure, 0.087812) - 1.65374;
		bottom = pow(pressure, 0.087812);
		topTotal = leftTop * rightTop;
		alt = (topTotal / bottom);
	}
	
	//Between 25,000m and 11,000m
	else if(67.05 > pressure && pressure > 2.55){
		rightTop = -6369.43;
		leftTop = log(pressure) - 4.85016;
		alt =  leftTop * rightTop;
	}
	
	//BELOW 11,000m (Pressure > 67.05)
	else {                                          
		leftTop = 44397.5;
		rightTop = 18437 * pow(pressure, 0.190259);
		alt = leftTop - rightTop;
	}
	
	return alt;
}


/**
 * Tests connection to Barometer.
 */
void Data::initialize()
{
	//If invalid connection, the program will stall and print an error message.
	if(!bmp.begin()){
		Serial.println("PROBLEM WITH PRESSURE SENSOR.");
		while(1);
	}
	//Valid connection, program proceeds as planned.
	else{
		Serial.println("Pressure Sensor Online.");
	}
}