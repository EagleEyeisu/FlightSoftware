/*******************************EAGLE EYE PROPRIETARY INFORMATION**************************************
 *                                                                                                    *
 *Purpose: Primary software for use on EAGLE EYE rotor craft. Controls and                            *
 *         maintains motor speed, deploys parachute, reads and records atmospheric                    *
 *         data, maintains I2C communication.                                                         *
 *                                                                                                    *
 *Date:      Version:        Developer:        Description:                                           *
 *06/25/16   1.0            James Wingerter   Initial Build.                                          *
 *07/10/16   1.1            James Wingerter   Added sd memory, parachute deployment                   *
 *09/14/16   1.2            James Wingerter   Added saftey count to 4                                 *
 *09/19/16   1.2a           James Wingerter   Changed variables to print real numbers                 *
 *12/26/16   2.0            Jared Danner      Complete Rebuild. Includes motor, altitude calculation  *           
 *                                            update, I2C Communication with LoRa, Event Logging      *
 *02/27/17   2.0a           Wesley Carelton   Fixed I2C Software & Event Logging.                     *
 *                          Jared Danner      2 way I2C.                                              *
 *03/30/17   2.1            James Wingerter   Added Thermocouple Temperature readings                 *
 *04/03/17   2.1a           Jared Danner      Housekeeping. Cleaned up/Restructured.                  *
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
#include <Adafruit_MAX31855.h>
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
float PARACHUTE_ARM_HEIGHT = 7620.0;     //9144 m == 30,000 feet //7620 m == 25000 feet
float PARACHUTE_DEPLOY_HEIGHT = 6096.0;  //6096m == 20,000 feet  **must be less that arm height**

/****COMMUNICATION****/
boolean HABET_Connection = true;    //Status for Connection to HABET.
boolean DISPATCH_SIGNAL = false;    //If false: reviece from LoRa. If true: send to LoRa.
boolean newData = false;            //Status of event data.
boolean DETACH_REQUEST = false;     //If true, gets Go/NoGo from 9Dof.
int x;                              //Event Number.
int Flip = 0;                       //Used to tell which program cycle the communication flip happened on.

/****9DOF Board****/
int timeout = 0;
Adafruit_LSM9DS0     lsm(1000);  // Use I2C, ID #1000
Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());
sensors_vec_t   orientation;

/****MISC****/
time_t current_time;              //Time of events.

/*
 * Holds data values for the flight.
 */
struct flight_data{
  float Pressure;
  float Altitude;
  float Temperature_ext;
  float Roll;
  float Pitch;
  float Yaw;
};
struct flight_data data;



/*****************************************************************************************************
 * Method initializes the main hardware components. Only runs once, or until everything is initialized
 ****************************************************************************************************/
void setup(){
  Serial.begin(4800); //4800 is the standard use across all Eagle Eye Programs
  
  /****Initialize the Altimeter****/
  if(!bmp.begin()){ //BMP085 Error, check connection
    Serial.println("PROBLEM WITH PRESSURE SENSOR.");
    while(1);
  }
  else{
    Serial.println("Pressure Sensor Online.");
  }
  
  /****Parachute deployment Initialize****/
  //Set all the pins low so they do not toggle on Reset or Power on!
  digitalWrite(RELAY1, LOW);  //Sends a LOW signal
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
    Serial.println("9Dof Online.");
  }
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);

  /****Initialize Thermocouple****/
  delay(500);
  Serial.println("Thermocouple Amplifier Online.\n\n");
}

/*
 * MAIN PROGRAM CODE.
 */
void loop(void){
  Serial.println(".");
  getData();                   //Updates entire struct.
  Save(0,0,0);                 //Store Data to SD Card.
  parachute();                 //Handles all things parachute.
  //motor_Function();          //Handles motor function.
  BoardCommunication();        //Decides to Send or Recieve I2C information.
  Detach_Orientation();        //Handles detachment.
  DelayHandler();
}

/*
 * Updates values to current conditions.
 */
void getData(){
  sensors_event_t event;
  bmp.getEvent(&event);

  data.Pressure = event.pressure;
  data.Temperature_ext = thermocouple.readCelsius();
  data.Altitude = getAlt(event.pressure);
  data.Roll = getRoll();
  data.Pitch = getPitch();
  data.Yaw = getYaw();
  
  Serial.print("Pressure:    ");Serial.print(event.pressure);Serial.println(" hPa");
  Serial.print("Altitude:    ");Serial.print(data.Altitude);Serial.println(" m");
  Serial.print("Temperature: ");Serial.print(data.Temperature_ext);Serial.println(" C");
  Serial.print("Roll:  ");Serial.println(data.Roll);
  Serial.print("Pitch: ");Serial.println(data.Pitch);
  Serial.print("Yaw:   ");Serial.println(data.Yaw);
  Serial.println();
  AltPrevious = data.Altitude;
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
 * Stores Data from all sensors to SD Card.
 * I2C_Selector:
 * 0 - Does not involve I2C
 * 1 - Sending Data.
 * 2 - Recieved Data.
 * 3 - Local Data.
 * Parachute_Selector:
 * 0 - Does not involve Parachute.
 * 1 - Enable Chute.
 * 2 - Deploy Chute.
 * 3 - Reset saftey counter.
 * 9DOF_Selector
 * 0 - Does not involve Orientation.
 * 1 - Default Orientation.
 * 2 - Special Test Case(Deploy).
 * 3 - Craft Deploy Orientation.
 */
void Save(int I2C_Selector, int Parachute_Selector,int DOF_Selector){
  if(I2C_Selector != 0){
    Save_I2C(I2C_Selector);
  }
  else if(Parachute_Selector != 0){
    Save_Parachute(Parachute_Selector);
  }
  else if(DOF_Selector != 0){
    Save_DOF(DOF_Selector);
  }
  else{
    Save_Default();
  }
}

/*
 * Saves I2C Data.
 */
void Save_I2C(int I2C_Selector){
  EagleEyeData = SD.open("EventLog.txt", FILE_WRITE);
  if(I2C_Selector==1){  //SAVES SENDING DATA.
    EagleEyeData.println();
    EagleEyeData.print(x);
    EagleEyeData.print(" <-Sent to LORA at ALT: ");
    EagleEyeData.print(data.Altitude);
    EagleEyeData.print(" at flight TIME: ");
    EagleEyeData.println(current_time = now());
  }
  else if(I2C_Selector==2){//SAVES RECIEVED DATA.
    EagleEyeData.println();
    EagleEyeData.print(x);
    EagleEyeData.print(" <-LORA Event Logged at ALT: ");
    EagleEyeData.print(data.Altitude);
    EagleEyeData.print(" at flight TIME: ");
    EagleEyeData.println(current_time = now());
  }
  else{ //SAVES LOCAL DATA.
    EagleEyeData.println();
    EagleEyeData.print(x);
    EagleEyeData.print(" <-Event Logged at ALT: ");
    EagleEyeData.print(data.Altitude);
    EagleEyeData.print(" at flight TIME: ");
    EagleEyeData.println(current_time = now());
  }
  EagleEyeData.close();
}

/*
 * Saves Parachute Data.
 */
void Save_Parachute(int Parachute_Selector){
  EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
  if(Parachute_Selector==1){    //SAVES PARACHUTE ENABLEMENT.
    EagleEyeData.println();
    EagleEyeData.print("ENABLED: ");
    EagleEyeData.print(data.Altitude); 
    EagleEyeData.println(" meters");
  }
  else if(Parachute_Selector==2){ //SAVES PARACHUTE DEPLOYMENT.
    EagleEyeData.println();
    EagleEyeData.print("DEPLOYED: ");
    EagleEyeData.print(data.Altitude);
    EagleEyeData.println(" meters");
  }
  else{  //SAVES RESET OF SAFTEY COUNTRER.
    EagleEyeData.println();
    EagleEyeData.println("Saftey reset to 0.");  
  }
  EagleEyeData.close();
}

/*
 * Saves 9DOF Orientation Data.
 */
void Save_DOF(int DOF_Selector){
  if(DOF_Selector==1){
    EagleEyeData = SD.open("9Dof.txt", FILE_WRITE);
    EagleEyeData.println();
    EagleEyeData.print(data.Altitude);
    EagleEyeData.print(",");
    EagleEyeData.print(data.Roll);
    EagleEyeData.print(",");
    EagleEyeData.print(data.Pitch);
    EagleEyeData.print(",");
    EagleEyeData.println(data.Yaw);
    EagleEyeData.close();
  }
  else if(DOF_Selector==2){
    EagleEyeData = SD.open("DescentData.txt", FILE_WRITE);
    EagleEyeData.println();
    EagleEyeData.print("Parachute deployed at: ");
    EagleEyeData.print(data.Altitude);
    EagleEyeData.print("m, Roll: ");
    EagleEyeData.print(data.Roll);
    EagleEyeData.print(" Pitch: ");
    EagleEyeData.println(data.Pitch);
    EagleEyeData.close();
  }
  else{
    EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
    EagleEyeData.println();
    EagleEyeData.print("Parachute deployed at: ");
    EagleEyeData.print(data.Altitude);
    EagleEyeData.print("m, Roll: ");
    EagleEyeData.print(data.Roll);
    EagleEyeData.print(" Pitch: ");
    EagleEyeData.println(data.Pitch);
    EagleEyeData.close();
  }
}

/*
 * Saves struct data.
 */
void Save_Default(){
  EagleEyeData = SD.open("FltData.txt", FILE_WRITE); //USE THIS BUT EVENT LOG
  EagleEyeData.println();
  EagleEyeData.print(current_time = now());
  EagleEyeData.print(",");
  EagleEyeData.print(data.Altitude);
  EagleEyeData.print(",");
  EagleEyeData.print(data.Pressure);
  EagleEyeData.print(",");
  EagleEyeData.print(data.Temperature_ext);
  EagleEyeData.print(",");
  EagleEyeData.print(data.Roll);
  EagleEyeData.print(",");
  EagleEyeData.print(data.Pitch);
  EagleEyeData.print(",");
  EagleEyeData.println(data.Yaw);
  EagleEyeData.close();
}

/**
 * All parachute functions/decisions/everything.
 */
void parachute(){
  gyroParachute();
  if(!chute_enable && data.Altitude >= PARACHUTE_ARM_HEIGHT){    //9144 m == 30,000 feet
    saftey_counter++;
    if(saftey_counter >= 4){
      chute_enable = true;
      Serial.print("ENABLED: ");Serial.print(data.Altitude);Serial.println(" meters ");
      Save(0,1,0);
    }
    else if(data.Altitude <= PARACHUTE_ARM_HEIGHT){  //Resets saftey counter to 0
      saftey_counter = 0;
      Serial.println("Saftey reset to 0.");
      Save(0,3,0);
    }
  }
  if(!chute_deploy && chute_enable && data.Altitude <= PARACHUTE_DEPLOY_HEIGHT){  //6096m == 20,000 feet
    digitalWrite(RELAY1, HIGH);//Closes circuit. Provides power to deployment.
    chute_deploy = true;
    Serial.print("DEPLOYED: ");Serial.print(data.Altitude);Serial.println(" meters");
    Save(0,0,3);
    delay(2000);//Provides power for 2 seconds. Than cuts power and opens the circuit.
    digitalWrite(RELAY1, LOW);
    Save(0,2,0);
  }
}

/*
 * Experimental Deployment relying on orientation.
 */
void gyroParachute(){
  if(parachuteRedZone() && chute_enable && (data.Altitude <= PARACHUTE_DEPLOY_HEIGHT)){
    Save(0,0,2);
  }
}

/*
 * Helper to determine whether to send or recieve signal.
 */
void BoardCommunication(){
  if(!DISPATCH_SIGNAL){
    I2C(false,DISPATCH_SIGNAL,0);//Checks for incoming communication from LoRA.
  }
  if(Flip == 1){//Sends handshake to LoRa upon communication switch.
    Send_I2C(13);
    Flip++;
   }
   /*if(Serial.read() == 's' && DISPATCH_SIGNAL){
      I2C(false,true,0);//Used to send a test signal back to the LoRa.
   }*/
}

/*
 * Handles Event Logging. From LoRa and MEGA.
 *  10 - Motor Start
 *  11 - Brake Off
 *  12 - Brake On
 *  13 - Communication Switch
 */
void I2C(boolean Local,boolean Send,int System_Event){
  if(!Local){
    if(Send){ //SEND TO LORA
    Send_I2C(System_Event);
    Save(1,0,0);
    }
    else{ //RECIEVE FROM LORA
      Receive_I2C();
    }
  }
  else{
    x = System_Event;
    Save(3,0,0);
  }
}

/*
 * Sends byte over I2C Connection.
 */
void Send_I2C(int System_Event){
  x = System_Event;
  Wire.beginTransmission(2);
  Wire.write(x);
  Wire.endTransmission();
}

/*
 * Recieves byte over I2C Connection.
 */
void Receive_I2C(){
  Wire.onReceive(receiveEvent);
  if(newData){
    Save(2,0,0);
    newData = false;
  }
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
 * Handles Detachment Orientation Process.
 */
void Detach_Orientation(){
  if(DETACH_REQUEST){
    EagleEyeData = SD.open("9Dof.txt", FILE_WRITE);
    if((parachuteRedZone && chute_enable) || (chute_enable && timeout >= 20)){ //CHANGE BACK TO 20!!
      if(timeout >= 20){
        EagleEyeData.println("Timed out. LAUNCH");
        Serial.println("Timed out. LAUNCH");
      }
      Send_I2C(9);
      Serial.println("Sent Detach Signal");
      DETACH_REQUEST = false;
      timeout = 0;
      EagleEyeData.print("Detach: ");
      Save(0,0,1);
    }
    else{
      timeout++;
    }
    EagleEyeData.close();
  }
}

/*
 * Checks to see if the craft is in the right orientation to deploy.
 */
bool parachuteRedZone(){
  return !(data.Roll >= 135 && data.Pitch <= 45);
}

/*
 * Collects 9Dof roll.
 */
float getRoll(){
  if(ahrs.getOrientation(&orientation)){
    return (180 - abs(orientation.roll));
  }
}

/*
 * Collects 9Dof Pitch.
 */
float getPitch(){
  if(ahrs.getOrientation(&orientation)){
    return (abs(orientation.pitch));
  }
}

/*
 * Collects 9Dof Yaw.
 */
float getYaw(){
  if(ahrs.getOrientation(&orientation)){
    return (orientation.pitch);
  }
}


/*
 * Adjusts the delay to accomidate for motor when active.
 */
void DelayHandler(){
  if(cycle_Up || cycle_Down){       //While motor is spinning up.
    if(spin_Up || motor_Complete){  //If motor is at full power or the motor cycle is complete.
      delay(500);
      Save(0,0,1);
      delay(500);
    }
    else{
      delay(300); // (3/10)th second delay when motor is active
    }
  }
  else{
    delay(500);//Handles delay adjustments.
    Save(0,0,1);
    delay(500);
  }
}
