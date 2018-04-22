/**
 * DATA.cpp contains the struct that holds all the Crafts situational information.
 */


#include "DATA.h"
#include "IMU.h"
#include "THERMO.h"
#include "I2C.h"
#include "MOTOR.h"
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

  //This is a junk variable because the Time is not of float format, therefore instead
  //   of chaning the entire structure of the method or making a duplicate, the varaible 
  //   is just set inside of the method. See the bottom end of Parse for more info. 
  float junk = Data.Parse(Comm.NDP,9);
}


/**
 * Updates the main struct for the craft. 
 */
void DATA::updateData()
{
  
  //Object used to store the store the data pulled from the BMP085.
  sensors_event_t event;
  
	//Creates new 'event' with the most current pressure sensor data.
	bmp.getEvent(&event);

  //MEGA DATA
	Local.Altitude = Data.getAltitude(event.pressure);
  Local.Latitude = Data.getGPSLatitude() / 10000.0;
  Local.Longitude = Data.getGPSLongitude() / 10000.0;
	Local.TempExt = Thermo.getTempExt();
	Local.Pressure = event.pressure;
	Local.Roll = Imu.getRoll();
	Local.Pitch = Imu.getPitch();
  Local.Yaw = Imu.getYaw();

  //LORA DATA
  Local.GPSAltitude = Data.getGPSAltitude();
  Local.GPSTargetAlt = Data.getGPSTargetAlt();
  Local.GPSTargetLat = Data.getGPSTargetLat() / 10000.0;
  Local.GPSTargetLon = Data.getGPSTargetLon() / 10000.0;
  Local.GPSTargetDistance = Data.getGPSTargetDistance();
  Local.GPSSpeed = Data.getGPSSpeed();
  Data.getGPSTime();

}

void DATA::toScreen()
{
  if(newData == YES){
    //Prints out data struct to the screen for debugging/following along purposes.
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("|                                MEGA DATA                                  |");
    Serial.println("|                                                                           |");
    Serial.print(  "|  Altitude:     "); Serial.print(Local.Altitude,2); Serial.print(" m");  Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Temperature:  "); Serial.print(Local.TempExt);    Serial.print(" C");  Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Pressure:     "); Serial.print(Local.Pressure);   Serial.print(" hPa");Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Roll:         "); Serial.print(Local.Roll);                            Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Pitch:        "); Serial.print(Local.Pitch);                           Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Yaw:          "); Serial.print(Local.Yaw);                             Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  LoRa Event:   "); Serial.print(Local.LE);                              Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Mega Event:   "); Serial.print(Local.ME);                              Serial.println("\t\t\t\t\t\t\t    |");
    Serial.println("|                                                                           |");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("|                                LORA DATA                                  |");
    Serial.println("|                                                                           |");
    Serial.print(  "|  Time:         "); Serial.print(Local.GPSTime);                                Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  GPS Altitude: "); Serial.print(Local.GPSAltitude,2);     Serial.print(" m");  Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Latitude:     "); Serial.print(Local.Latitude,5);                             Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Longitude:    "); Serial.print(Local.Longitude,5);                            Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  TargetLat:    "); Serial.print(Local.GPSTargetLat,5);                         Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  TargetLon:    "); Serial.print(Local.GPSTargetLon,5);                         Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Speed:        "); Serial.print(Local.GPSSpeed);          Serial.print(" mps");Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Distance:     "); Serial.print(Local.GPSTargetDistance); Serial.print(" m");  Serial.println("\t\t\t\t\t\t\t    |");
    Serial.println("|                                                                           |");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("|                                                                           |");
    Serial.println("|                               MOTOR DATA                                  |");
    Serial.print(  "|  State:        "); Serial.print(Movement.getSTATE());Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Servo Pos   : "); Serial.print(Movement.pos);       Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Move Forward: "); Serial.print(Imu.moveForward);    Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Turn Right:   "); Serial.print(Imu.turnRight);      Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Turn Left:    "); Serial.print(Imu.turnLeft);       Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Move Up:      "); Serial.print(Imu.moveUp);         Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Move Down:    "); Serial.print(Imu.moveDown);       Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Target Angle: "); Serial.print(Imu.ATT);            Serial.println("\t\t\t\t\t\t\t    |");
    Serial.print(  "|  Bearing:      "); Serial.print(Imu.bearing);        Serial.println("\t\t\t\t\t\t\t    |");
    Serial.println("|                                                                           |");
    Serial.println("-----------------------------------------------------------------------------");
  }
}


/**
 * Returns a parsed section of the read in parameter. The parameter 'objective' represents 
 *    the comma's position from the beginning of the character array.
 */
float DATA::Parse(String message, int objective)
{
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
  //  $,GPSAltitude, Latitude, Longitude, TargetAlt, TargetLat, TargetLon, TargetDistance, Speed, Time,$
  //         1           2         3          4          5          6            7           8      9 
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
  Serial.println("In Data");
	if(!bmp.begin()){
		Serial.println("PROBLEM WITH PRESSURE SENSOR.");
		
	}
	//Valid connection, program proceeds as planned.
	else{
		Serial.println("Pressure Sensor Online.");
	}
}
