/*******************************EAGLE EYE PROPRIETARY INFORMATION***************************************
 *                                                                                                     *
 *Purpose: Primary software for use on EAGLE EYE rotor craft. Controls and                             *
 *         maintains motor speed, deploys parachute, reads and records atmospheric                     *
 *         data, maintains I2C communication.                                                          *
 *                                                                                                     *
 *Date:      Version:        Developer:        Description:                                            *
 *6/25/16    1.0             James Wingerter   Initial Build.                                          *
 *7/10/16    1.1             James Wingerter   Added sd memory, parachute deployment                   *
 *9/14/16    1.2             James Wingerter/Jared Danner - Added saftey count to 4                    *
 *9/19/16    1.3             Jared Danner      Changed variables to print real numbers                 *
 *12/26/16   2.0             Jared Danner      Complete Rebuild. Includes motor, altitude calculation  *           
 *                                             update, I2C Communication with LoRa, Event Logging      *
 *2/27/17    2.1             Wesley Carelton   Fixed I2C Software & Event Logging.                     *
 *                           Jared Danner      2 way I2C.                                              *
 *3/30/17    2.2             James Wingerter   Added Thermocouple Temperature readings                 *
 *******************************************************************************************************/

/****LIBRARIES****/
#include <TimeLib.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Simple_AHRS.h>
#include <Servo.h>
#include <SD.h> 
#include "Adafruit_MAX31855.h"
#include <SPI.h>

/****FLIGHT******/
float AltPrevious = 0.0;

//CONSTANT VARIABLES
/****PINS********/
#define RELAY1  28    //Parachute Digital Pin to IN1

/****MOTOR*******/
//1% = 880       30% = 1180
//5% = 930       35% = 1230
//10% = 980      40% = 1280
//15% = 1030     45% = 1330
//20% = 1080     50% = 1380
//25% = 1130     
#define MAX_SIGNAL 1180     //Change to desired max power. Percentages given right above
#define MIN_SIGNAL 700      //Dont't touch
#define MOTOR_PIN 49        //Don't touch unless switching to UNO than change to 9

#define INCREMENT_AMOUNT 10    //Don't touch
#define INCREMENT_DELAY 320    //Don't touch
#define FULL_POWER_DELAY 4000  //Changes how long you want to the motor to stay at a contant rate once you are at full power

Servo motor;                     //Creates an Servo object used to control motor.
int speed;                       //Speed variable
int speedPrevious;               //Holds the speed value that the previous cycle 
boolean spin_Up = true;          //True if spin up cycle was reached.
boolean cycle_Up = false;        //True while motor needs to throttle upward.
boolean cycle_Down = false;      //True while motor needs to throttle down.
boolean motor_Complete = false;  //True when motor cycle is complete. 
boolean motor_Start = false;     //Triggers the beginning of the motor process.
boolean break_Status = false;    //Status of break.

/****THERMOCOUPLE PINS****/
#define MAXDO   32
#define MAXCS   30
#define MAXCLK  31

/****SENSORS*****/
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);   //Don't touch
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);           //Don't touch

/****SD CARD*****/
File EagleEyeData;                  //File object used to store data during flight.
#define SD_PIN 53                   //Digital connection pin for SD card. 

/****PARACHUTE****/
boolean chute_enable = false;        //Status of chute readiness.
boolean chute_deploy = false;        //Status of chute deployment.
int saftey_counter = 0;              //Saftey counter.
int PARACHUTE_ARM_HEIGHT = 7620;     //9144 m == 25,000 feet
int PARACHUTE_DEPLOY_HEIGHT = 6096;  //6096m == 20,000 feet

/****COMMUNICATION****/
boolean HABET_Connection = true;    //Status for Connection to HABET.
boolean DISPATCH_SIGNAL = false;    //If false: reviece from LoRa. If true: send to LoRa.
boolean newData = false;            //Status of event data.
boolean DETACH_REQUEST = false; //If true, gets Go/NoGo from 9Dof.
int x;                              //Event Number.
int Flip = 0;                       //Used to tell which program cycle the communication flip happened on.

/****9DOF Board****/
int timeout = 0;
Adafruit_LSM9DS0     lsm(1000);  // Use I2C, ID #1000
Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());

/****MISC****/
time_t current_time;              //Time of events.

/*
 * Holds data values of Pressure, Altitude, and Temperature
 */
struct flight_data{
  float Pressure;
  float Altitude;
  float Temperature_ext;
  float roll;
  float pitch;
  float yaw;
};


/*****************************************************************************************************
 * Method initializes the main hardware components. Only runs once, or until everything is initialized
 ****************************************************************************************************/
void setup(){
  Serial.begin(4800); //4800 is the standard use across all Eagle Eye Programs
  
  /****Initialize the Altimeter****/
  if(!bmp.begin()){ //BMP085 Error, check connection
    Serial.println("PROBLEM WITH PRESSURE SENSOR.");
    //while(1);
  }
  else{
    Serial.println("Pressure Sensor Online.");
  }
  
  /****Parachute deployment Initialize****/
  //Set all the pins low so they do not toggle on Reset or Power on!
  digitalWrite(RELAY1, HIGH);  //Sends a LOW signal
  pinMode(RELAY1, OUTPUT);     //Sets RELAY1 as output pin.
  Serial.println("Parachute Online.");
  
  /****Initialize SD Card reader****/
  pinMode(SD_PIN, OUTPUT);
  if(!SD.begin(SD_PIN)){
    Serial.println("PROBLEM WITH SD CARD.");
    //while(1);
  }
  else{
    Serial.println("SD Card Online.");
  }
  
  /****Initialize Motor Pin****/
  motor.attach(MOTOR_PIN);     //Attaches ESC to Arduino
  Serial.println("Motor Online.");

  /****Initialize I2C Comms****/
  Wire.begin(1);  //Setting the address for this board.
  Serial.println("Comms Address Set.");

  /****Initialize 9DOF Board****/
  if(!lsm.begin()){
    Serial.print("PROBLEM WITH 9DOF");
    while(1);
  }
  else{
    Serial.print("9Dof Online.\n\n");
  }
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);

  /****Initialize Thermocouple****/
  // wait for MAX chip to stabilize
  delay(500);
  Serial.println("Thermocouple Amplifier Online");
}

/*******************************************************************
 *************************MAIN PROGRAM CODE*************************
 *******************************************************************/
void loop(void){
  //Serial.println(".");
  flight_data current = getData();           //Updates altitude, pressure, and tempurature.
  store_Data(current.Pressure,current.Temperature_ext,current.Altitude,current.roll,current.pitch,current.yaw);  //Store Data to SD Card.
  parachute(current.Altitude);                                            //Handles all things parachute.
  //motor_Function(current.Altitude);                                     //Handles motor function.
  BoardCommunication(current.Altitude);                                   //Decides to Send or Recieve I2C information.
  Orientation(current.Altitude);
  DelayHandler(current.Altitude);
}

/*******************************************************************
*******************************************************************/

/*
 * Updates values to current conditions.
 */
struct flight_data getData(){
  /****Get a new sensor event****/
  sensors_event_t event;
  bmp.getEvent(&event);
  float temperature_ext = thermocouple.readCelsius();

  float roll = getRoll();
  float pitch = getPitch();
  float yaw = getYaw();
  
  /****Display atmospheric pressue in hPa****/
  //Serial.print("Pressure:    ");
  //Serial.print(event.pressure);
  //Serial.println(" hPa ");

  /****Display Temperature in Celsius****/
     if (isnan(temperature_ext)) {
     //Serial.println("0.00");
   } else {
     //Serial.print("C = "); 
     //Serial.println(c);
   }
   
  /****First we get the current temperature from the BMP180****/
  //bmp.getTemperature(&temperature);
  //Serial.print("Temperature: ");
  //Serial.print(temperature);
  //Serial.println(" C ");

  /****Calculate Altitude from Pressure****/
  float Altitude = getAlt(event.pressure);
  AltPrevious = Altitude;
  //Serial.print("Altitude:    ");
  //Serial.print(Altitude);
  //Serial.println(" m\n");

  /****Save current Temp, Alt, and Pressure to our data struct****/
  flight_data data;
  data.Pressure = event.pressure;
  data.Temperature_ext = temperature_ext;
  data.Altitude = Altitude;
  data.roll = roll;
  data.pitch = pitch;
  data.yaw = yaw;
  return data;
}

/*
 * Method used to derive Altitude from Pressure.
 * Initally hPa, than converted to mPa for calculations.
 */
float getAlt(float inPressure){
  float pressure = inPressure/10.0;
  float alt;
  float leftTop;
  float rightTop;      //All variables used to calculate altitude
  float bottom;
  float topTotal;
    
  if(pressure < 2.55){                           //ABOVE 25,000m
    leftTop = -47454.8;
    rightTop = pow(pressure, 0.087812) - 1.65374;
    bottom = pow(pressure, 0.087812);
    topTotal = leftTop * rightTop;
    alt = (topTotal / bottom);
  }
  else if(67.05 > pressure && pressure > 2.55){  //ABOVE 11,000m and BELOW 25,000m
    rightTop = -6369.43;
    leftTop = log(pressure) - 4.85016;
    alt =  leftTop * rightTop;
  }
  else{                                           //BELOW 11,000m (Pressure > 67.05)
    leftTop = 44397.5;
    rightTop = 18437 * pow(pressure, 0.190259);
    alt = leftTop - rightTop;
  }
  return alt;
}

/*
 * Used to store Data to SD card storage
 */
void store_Data(float Pressure, float Temperature_ext, float Altitude, float roll, float pitch, float yaw){
  EagleEyeData = SD.open("FltData.txt", FILE_WRITE); //USE THIS BUT EVENT LOG
  EagleEyeData.print(current_time = now());
  EagleEyeData.print(",");
  EagleEyeData.print(Altitude);

  EagleEyeData.print(",");
  EagleEyeData.print(Pressure);

  EagleEyeData.print(",");
  EagleEyeData.println(Temperature_ext);
  EagleEyeData.close();


  EagleEyeData = SD.open("9Dof.txt", FILE_WRITE);
  EagleEyeData.print(roll);
  EagleEyeData.print(",");
  EagleEyeData.println(pitch);
  EagleEyeData.close();
  EagleEyeData.println(yaw);
  EagleEyeData.close();
  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print(" Pitch: ");
  Serial.print(pitch);
  Serial.print(" Yaw: ");
  Serial.print(yaw);
  Serial.println();
}

/**
 * All parachute functions/decisions/everything.
 */
void parachute(float Altitude){
  gyroParachute(Altitude);
  EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
  if(!chute_enable && Altitude >= PARACHUTE_ARM_HEIGHT){    //9144 m == 30,000 feet
    saftey_counter++;
    if(saftey_counter >= 4){
      chute_enable = true;
      Serial.print("ENABLED: ");
      Serial.print(Altitude);
      Serial.println(" meters ");
      EagleEyeData.print("ENABLED: ");
      EagleEyeData.print(Altitude); 
      EagleEyeData.println(" meters ");
    }
    else if(Altitude <= PARACHUTE_ARM_HEIGHT){  //Resets saftey counter to 0
      saftey_counter = 0;
      Serial.println("Saftey reset to 0.");
      EagleEyeData.println("Saftey reset to 0."); 
    }
  }
  if(!chute_deploy && chute_enable && Altitude <= PARACHUTE_DEPLOY_HEIGHT){  //6096m == 20,000 feet
    digitalWrite(RELAY1, LOW);                //This is close the circuit providing power the chute deployment system
    chute_deploy = true;
    Serial.print("DEPLOYED: ");
    Serial.print(Altitude);
    Serial.println(" meters");
    Save_Orientation(Altitude);
    delay(2000);                              //Run the current for 2 seconds, then open the circuit and stop the current
    digitalWrite(RELAY1, HIGH);
    EagleEyeData.print("DEPLOYED: ");
    EagleEyeData.print(Altitude);
    EagleEyeData.println(" meters");
  }
  EagleEyeData.close();
}

/*
 * Helper to determine whether to send or recieve signal.
 */
void BoardCommunication(float Altitude){
  if(!DISPATCH_SIGNAL){
    I2C(Altitude,false,DISPATCH_SIGNAL,0);   //Checks for incoming communication from LoRA.
  }
  if(Flip == 1){ //Sends handshake to LoRa upon communication switch.
    byte y = 13;
    Wire.beginTransmission(2);
    Wire.write(y);
    Wire.endTransmission();
    Flip++;
   }
   /*if(Serial.read() == 's' && DISPATCH_SIGNAL){
      I2C(Altitude,false,true,0);                 //Used to send a test signal back to the LoRa.
   }*/
}

/*
 * Handles Event Logging. From LoRa and MEGA.
 *  10 - Motor Start
 *  11 - Brake Off
 *  12 - Brake On
 *  13 - Communication Switch
 */
void I2C(float Altitude,boolean Local,boolean Send,int System_Event){
  EagleEyeData = SD.open("EventLog.txt", FILE_WRITE);
  if(!Local){
    if(Send){ //SEND TO LORA
    byte x = System_Event;
    Wire.beginTransmission(2);
    Wire.write(x);
    Wire.endTransmission();
    Serial.println("sent");
    EagleEyeData.print(System_Event);
    EagleEyeData.print(" <-Sent to LORA at ALT: ");
    EagleEyeData.print(Altitude);
    EagleEyeData.print(" at flight TIME: ");
    EagleEyeData.println(current_time = now());
    }
    else{ //RECIEVE FROM LORA
      Wire.onReceive(receiveEvent);
      if(newData){
        EagleEyeData.println();
        EagleEyeData.print(x);
        EagleEyeData.print(" <-LORA Event Logged at ALT: ");
        EagleEyeData.print(Altitude);
        EagleEyeData.print(" at flight TIME: ");
        EagleEyeData.println(current_time = now());
        newData = false;
      }
    }
  }
  else{
    EagleEyeData.print(System_Event);
    EagleEyeData.print(" <-Event Logged at ALT: ");
    EagleEyeData.print(Altitude);
    EagleEyeData.print(" at flight TIME: ");
    EagleEyeData.println(current_time = now());
  }
  EagleEyeData.close();
}

/**
 * Helper for radio
 */
void receiveEvent(){
  Serial.print("Event Recieved: ");
  x = Wire.read();    //Receive byte as an integer
  Serial.println(x);  //Print the integer
  if(x == 8){
    DISPATCH_SIGNAL = true;
    DETACH_REQUEST = true;
    Serial.print("Sending Mode");
    Flip = 1;
  }
  else if(x==4){
    HABET_Connection = false;
    Serial.println("Detached");
  }
  newData = true;
}

/*
 * Processes data from the 9Dof board.
 */
void Orientation(float Altitude){
  if(DETACH_REQUEST){                    //LoRa's asked us for detach.
    Detach_Orientation(Altitude);        //Handles Detachment Orientation Process.
  }
  else{
    Save_Orientation(Altitude);          //Saves the Orientation.
  }
}

/*
 * Saves Orientation Data to SD Card.
 */
void Save_Orientation(float Altitude){
  float roll = getRoll();
  float pitch = getPitch();
  
  EagleEyeData = SD.open("9Dof.txt", FILE_WRITE);
  EagleEyeData.print(Altitude);
  EagleEyeData.print(",");
  EagleEyeData.print(roll);
  EagleEyeData.print(",");
  EagleEyeData.println(pitch);
  EagleEyeData.close();
  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print(" Pitch: ");
  Serial.print(pitch);
  Serial.println();
}

/*
 * Handles Detachment Orientation Process.
 */
void Detach_Orientation(float Altitude){
  EagleEyeData = SD.open("9Dof.txt", FILE_WRITE);
  if((parachuteRedZone && chute_enable) || (chute_enable && timeout >= 20)){ //CHANGE BACK TO 20!!
    if(timeout >= 20){
      EagleEyeData.println("Timed out. LAUNCH");
    }
    I2C(Altitude,false,DISPATCH_SIGNAL,9);
    DETACH_REQUEST = false;
    timeout = 0;
    EagleEyeData.print("Detach: ");
  }
  else{
    timeout++;
  }
  Save_Orientation(Altitude);
  EagleEyeData.close();
}

/*
 * Checks to see if the craft is in the right orientation to deploy.
 */
bool parachuteRedZone(float roll, float pitch){
  return !(roll >= 135 && pitch <= 45);
}

/*
 * Stuff I don't understand. I hope this is what you guys wanted.
 */
void gyroParachute(float Altitude){
  float roll = getRoll();
  float pitch = getPitch();
  if(parachuteRedZone(roll,pitch) && chute_enable && (Altitude <= PARACHUTE_DEPLOY_HEIGHT)){
    EagleEyeData = SD.open("DescentData.txt", FILE_WRITE);
    EagleEyeData.print("Parachute deployed at: ");
    EagleEyeData.print(Altitude);
    EagleEyeData.print("m, Roll: ");
    EagleEyeData.print(roll);
    EagleEyeData.print(" Pitch: ");
    EagleEyeData.println(pitch);
    EagleEyeData.close();
  }
}

/*
 * Collects 9Dof roll.
 */
float getRoll(){
  sensors_vec_t   orientation;
  if(ahrs.getOrientation(&orientation)){
    return (180 - abs(orientation.roll));
  }
}

/*
 * Collects 9Dof Pitch.
 */
float getPitch(){
  sensors_vec_t   orientation;
  if(ahrs.getOrientation(&orientation)){
    return (abs(orientation.pitch));
  }
}
float getYaw(){
  sensors_vec_t   orientation;
  if(ahrs.getOrientation(&orientation)){
    return (orientation.pitch);
  }
}


/*
 * Adjusts the delay to accomidate for motor when active.
 */
void DelayHandler(float Altitude){
  if(cycle_Up || cycle_Down){       //While motor is spinning up.
    if(spin_Up || motor_Complete){  //If motor is at full power or the motor cycle is complete.
      delay(500);
      Orientation(Altitude);
      delay(500);
    }
    else{
      delay(300); // (3/10)th second delay when motor is active
    }
  }
  else{
    delay(500);//Handles delay adjustments.
    Orientation(Altitude);
    delay(500);
  }
}
