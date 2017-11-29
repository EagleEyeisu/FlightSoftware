/**
 * DATA.cpp contains the struct that holds all the Crafts situational information.
 */


#include "DATA.h"
#include "IMU.h"
#include "THERMO.h"
#include "I2C.h"
#include "Globals.h"
#include <Arduino.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_MAX31855.h>


/**
 * Constructor used to reference all other variables & functions.
 */
DATA::DATA()
{
	
}


//Reads in the altitude from the CAN Network.
float DATA::getGPSAltitude(){
  return Data.Parse(Comm.NDP,1);
}


//Reads in the latitude from the CAN Network.
float DATA::getGPSLatitude(){
  return Data.Parse(Comm.NDP,2);
}


//Reads in the longitude from the CAN Network.
float DATA::getGPSLongitude(){
  return Data.Parse(Comm.NDP,3);
}


//Reads in the target latitude from the CAN Network.
float DATA::getGPSTargetAlt(){
  return Data.Parse(Comm.NDP,4);
}


//Reads in the target latitude from the CAN Network.
float DATA::getGPSTargetLat(){
  return Data.Parse(Comm.NDP,5);
}


//Reads in the taret longitude from the CAN Network.
float DATA::getGPSTargetLon(){
  return Data.Parse(Comm.NDP,6);
}


//Reads in the target Distance from the CAN Network.
float DATA::getGPSTargetDistance(){
  return Data.Parse(Comm.NDP,7);
}


//Reads in the speed from the CAN Network. (meters per second)
float DATA::getGPSSpeed(){
  return Data.Parse(Comm.NDP,8);
}


//Reads in the time of the LoRa from the CAN Network.
void DATA::getGPSTime(){
  float junk = Data.Parse(Comm.NDP,9);
}


/**
 * Updates the main struct for the craft. 
 */
void DATA::manager()
{
  
  //Object used to store the store the data pulled from the BMP085.
  sensors_event_t event;
  
	//Creates new 'event' with the most current pressure sensor data.
	bmp.getEvent(&event);

  //ORIGINAL CRAFT INFORMATION.
	Local.Altitude = Data.getAltitude(event.pressure);
  Local.Latitude = Data.getGPSLatitude();
  Local.Longitude = Data.getGPSLongitude();
	Local.TempExt = Thermo.getTempExt();
	Local.Pressure = event.pressure;
	Local.Roll = Imu.getRoll();
	Local.Pitch = Imu.getPitch();
  Local.Yaw = Imu.getYaw();

  //LORA CRAFT INFORMATION
  Local.GPSAltitude = Data.getGPSAltitude();
  Local.GPSTargetAlt = Data.getGPSTargetAlt();
  Local.GPSTargetLat = Data.getGPSTargetLat();
  Local.GPSTargetLon = Data.getGPSTargetLon();
  Local.GPSTargetDistance = Data.getGPSTargetDistance();
  Local.GPSSpeed = Data.getGPSSpeed();
  Data.getGPSTime();

  
	//Local.LE and Local.ME update on their own throughout 
	//   the program & are reset to 0 after being saved.

	//Prints out data struct to the screen for debugging/following alone purposes.
  Serial.println("     MEGA INFORMATION     ");
	Serial.print("Altitude:    "); 		Serial.print(Local.Altitude,2);     Serial.println(" m");
	Serial.print("Temperature: "); 		Serial.print(Local.TempExt);        Serial.println(" C");
	Serial.print("Pressure:    "); 		Serial.print(Local.Pressure);       Serial.println(" hPa");
	Serial.print("Roll:        "); 	  Serial.println(Local.Roll);
	Serial.print("Pitch:       "); 		Serial.println(Local.Pitch);
	Serial.print("Yaw:         ");    Serial.println(Local.Yaw);
  Serial.print("LoRa Event:  ");    Serial.println(Local.LE);
  Serial.print("Mega Event:  ");    Serial.println(Local.ME);

  Serial.println("     LORA INFORMATION     ");
  Serial.print("Time:        ");    Serial.println(Local.GPSTime);
  Serial.print("Altitude:    ");    Serial.print(Local.GPSAltitude,2);     Serial.println(" m");
  Serial.print("Latitude:    ");    Serial.println(Local.Latitude,6);
  Serial.print("Longitude:   ");    Serial.println(Local.Latitude,6);
  Serial.print("TargetLat:   ");    Serial.println(Local.GPSTargetLat,6);
  Serial.print("TargetLon:   ");    Serial.println(Local.GPSTargetLon,6);
  Serial.print("Speed:       ");    Serial.print(Local.GPSSpeed);          Serial.println(" mps");
  Serial.print("Distance:    ");    Serial.print(Local.GPSTargetDistance); Serial.println(" m");

  Serial.println("     MOTOR INFORMATION    ");
  Serial.print("Move Forward: ");   Serial.println(Imu.moveForward);
  Serial.print("Turn Right:   ");   Serial.println(Imu.turnRight);
  Serial.print("Turn Left:    ");   Serial.println(Imu.turnLeft);
  Serial.print("Move Up:      ");   Serial.println(Imu.moveUp);
  Serial.print("Move Down:    ");   Serial.println(Imu.moveDown);
  Serial.print("Target Angle: ");   Serial.println(Imu.ATT);
  Serial.println("-------------------------------------------");
}


/**
 * Returns a parsed section of the read in parameter. The parameter 'objective' represents 
 *    the comma's position from the beginning of the character array.
 */
float DATA::Parse(String message, int objective){

  Serial.println(message);

  //Example GPS Transmission. (GGA)
  //
  //  $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
  //
  //Example Radio Transmission. 
  //
  //                  LORA                              HABET                         MISSION CONTROL                   CRAFT ID
  //  Time(ms),Altitude,Latitude,Longitude,LE, | Time(ms),Release Status, | Time(ms),Command Sent,Command Received, | Signal Origin
  //
  //Example I2C Transmission
  //
  //                                   CONTROLLER ACCESS NETWORK PROTOCOL PACKET
  //  $,GPSAltitude, Latitude, Longitude, TargetLat, TargetLon, TargetDistance, Roll, Pitch, Yaw, Speed, Time,$
  //         1           2         3          4           5           6           7     8     9     10    11
  //
  //
  //The number of commas that the program needs to pass before it started parsing the data.
  int goalNumber = objective;
  
  //Used to iterate through the character arrays.
  int i = 0;
  
  //This iterator is used to pull the wanted part of the 'message' from the entire array.
  //   Used to gather information such as how long the new parsed section is.
  int tempIter = 0;
  
  //Counts the commas as the character array is iterated over.
  int commaCounter = 0;
  
  //This turns true when the correct number of commas has been achieved, which signals that the following 
  //   section is the part that the program wants to parse from the entire sentence.
  bool Goal = false;
  
  //Temporary string used to hold the newly parsed array.
  char tempArr[10];

  for(int z = 0;z<10;z++){
    tempArr[z] = ' ';
  }
  
  //Iterators over the entire array.
  for(i=0;i<120;i++){
    
    //Checks to see if the current iterator's position is a comma. 
    if(message[i] == ','){
      
      //If so, it iterators the comma counter by 1.
      commaCounter++;
      //Serial.println("comma");
    }
    //Checks to see if the desired amount of commas has been passed. 
    else if(commaCounter == goalNumber){
      
      //Checks to see if the iterator's position is a comma, used to cause a stop in parsing.
      if(message[i] != ','){
        
        //Copies the message's character to the temporary array.
        tempArr[tempIter] = message[i];
        //Serial.print(tempArr[t]);
        tempIter++;
      }
    }
  }
  //Serial.print(" ");
  //Serial.println(tempIter);
  //Charater array used with a fitted length of the parsed section.
  char arr[tempIter];

  for(int x=0;x<tempIter;x++){
    arr[x] = ' ';
  }

  //Iterates through the temporary array copying over the info to the variable which will be returned.
  for(i=0;i<tempIter;i++){
    
    //Copying of the information between arrays.
    arr[i] = tempArr[i];
    //Serial.print(arr[i]);
  }
  //Serial.println();
  //Used below to detect the parsing of the time value.
  bool TimeDetected = false;
  
  //Cycles through to check for if the time value is being parsed.
  //   The time value is saved as a short char array, where as all
  //   other parsed values are converted to floats before being 
  //   returned.
  for(i=0;i<tempIter;i++){

    //The time character array will have colons located in it.
    //   No other value being parsed should contain this character.
    //   Therefore, if ones detected, I set a value to true.
    if(arr[i]==':'){

      TimeDetected = true;
    }
  }

  //Time value has been detected.
  if(TimeDetected){

    //Resets the variable for redundancy.
    TimeDetected = false;

    //Copies over arr to time.
    for(i=0;i<tempIter;i++){
      
      Data.Local.GPSTime[i] = arr[i];
    }
    //Serial.println(Data.Local.GPSTime);
    
  }
  else{

    //Converts the final array to a float and returns.
    return atof(arr);
  }
}


/**
 * Derives Crafts altitude based on current atmosphereic pressure.
 *    DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD TO BY JAMES/JARED.
 */
float DATA::getAltitude(float _Pressure)
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
void DATA::initialize()
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
