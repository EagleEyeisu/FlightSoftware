/**
 * Data.cpp contains the struct that holds all the Crafts situational information.
 */


#include "Data.h"


/**
 * Updates the main struct for the craft. 
 */
void Data::Data_Manager()
{
	//Creates new 'event' with the most current pressure sensor data.
	bmp.getEvent(&event);

	data.Altitude = getAltitude(event.pressure);
	data.TempExt = getTempExt();
	data.Pressure = event.pressure;
	data.Roll = getRoll();
	data.Pitch = getPitch();
	data.Yaw = getYaw();
	//data.LE and data.ME update on their own throughout the program & are reset to 0 after being saved.

	//Prints out data struct to the screen for debugging/following alone purposes.
	Serial.print("Altitude:    "); 		Serial.print(data.Altitude); Serial.println(" m");
	Serial.print("Temperature: "); 		Serial.print(data.TempExt); Serial.println(" C");
	Serial.print("Pressure:    "); 		Serial.print(data.Pressure); Serial.println(" hPa");
	Serial.print("Roll:  "); 			Serial.println(data.Roll);
	Serial.print("Pitch: "); 			Serial.println(data.Pitch);
	Serial.print("Yaw:   "); 			Serial.println(data.Yaw);
	Serial.print("LoRa Event: "); 		Serial.println(data.LE);
	Serial.print("Mega Event: "); 		Serial.println(data.ME);
	Serial.print("-------------------------------------------");
	Serial.println();
}


/**
 * Derives Crafts altitude based on current atmosphereic pressure.
 *    DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD TO BY JAMES/WESLY/JARED.
 */
float Data::getAltitude(float _Pressure)
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
void Data::Pressure_Initialize()
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