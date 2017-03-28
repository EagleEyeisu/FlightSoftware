/*******************************EAGLE EYE PROPRIETARY INFORMATION***************************************
 *                                                                                                     *
 *Purpose: Primary software for use on EAGLE EYE rotor craft. Controls and                             *
 *         maintains motor speed, deploys parachute, reads and records atmospheric                     *
 *         data.                                                                                       *
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

/****SENSORS*****/
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);   //Don't touch

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
boolean redZoneBool = false;

/****MISC****/
time_t current_time;              //Time of events.

/*
 * Holds data values of Pressure, Altitude, and Temperature
 */
struct flight_data{
  float Pressure;
  float Altitude;
  float Temperature;
};

/*
 * Method initializes the main hardware components. Only runs once, or until everything is initialized
 */
void setup() {
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
  digitalWrite(RELAY1, HIGH);  //Sends a LOW signal
  pinMode(RELAY1, OUTPUT);     //Sets RELAY1 as output pin.
  Serial.println("Parachute Online.");

  /****Initialize SD Card reader****/
  pinMode(SD_PIN, OUTPUT);
  if (!SD.begin(SD_PIN)){
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
  
}

/*
 * MAIN PROGRAM CODE. 
 */
void loop(void){
  //Serial.println(".");
  sensors_event_t event; //Creates event object to store pressure sensor data in.
  bmp.getEvent(&event);  //Updates event object with the current pressure sensor data.
  
  flight_data current = getData();                                    //Updates altitude, pressure, and tempurature.
  store_Data(current.Pressure, current.Temperature, current.Altitude);//Store Data to SD Card.
  parachute(current.Altitude);                                        //Handles all things parachute.
  //motor_Function(current.Altitude);                                 //Handles motor function.
  BoardCommunication(current.Altitude);                               //Decides to Send or Recieve I2C information.
  Orientation(current.Altitude);
  delay(500);//Handles delay adjustments.
  gyroParachute(current.Altitude)  
  delay(500)
}

/*
 * Processes data from the 9Dof board.
 */
void Orientation(float Altitude){
  sensors_vec_t   orientation;
  if(true){//ahrs.getOrientation(&orientation)
    float roll = 180 - abs(orientation.roll);
    //Serial.print(F("Orientation: "));
    //Serial.println(roll);
    //Serial.print(F(" "));
    float pitch = abs(orientation.pitch); 
    //Serial.println(pitch);
    
    EagleEyeData = SD.open("9Dof.txt", FILE_WRITE);
    if(DETACH_REQUEST){
      if((roll <= 90 && pitch <= 90) || timeout >= 2){ //CHANGE BACK TO 20!!
        if(timeout >= 0){
         Serial.println("   FUCK IT    ");
        }
        I2C(Altitude,false,DISPATCH_SIGNAL,9);
        DETACH_REQUEST = false;
        timeout = 0;
        EagleEyeData.print("Detach: ");
     }
     else{
       timeout++;
     }
   }
  EagleEyeData.print(Altitude);
  EagleEyeData.print(",");
  EagleEyeData.print(roll);
  EagleEyeData.print(",");
  EagleEyeData.println(pitch);
  EagleEyeData.close();
  }
}

/*
 * Updates values to current conditions.
 */
struct flight_data getData(){
  /****Get a new sensor event****/
  sensors_event_t event;
  bmp.getEvent(&event);
  
  /****Display atmospheric pressue in hPa****/
  Serial.print("Pressure:    ");
  Serial.print(event.pressure);
  Serial.println(" hPa ");
   
  /****First we get the current temperature from the BMP180****/
  float temperature;
  bmp.getTemperature(&temperature);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C ");

  /****Calculate Altitude from Pressure****/
  float Altitude = getAlt(event.pressure);
  AltPrevious = Altitude;
  Serial.print("Altitude:    ");
  Serial.print(Altitude);
  Serial.println(" m\n");

  /****Save current Temp, Alt, and Pressure to our data struct****/
  flight_data data;
  data.Pressure = event.pressure;
  data.Temperature = temperature;
  data.Altitude = Altitude;
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
void store_Data(float Pressure, float Temperature, float Altitude){
  EagleEyeData = SD.open("FltData.txt", FILE_WRITE); //USE THIS BUT EVENT LOG
  EagleEyeData.print(current_time = now());
  EagleEyeData.print(",");
  EagleEyeData.print(Altitude);

  EagleEyeData.print(",");
  EagleEyeData.print(Pressure);

  EagleEyeData.print(",");
  EagleEyeData.println(Temperature);
  EagleEyeData.close();
}

/**
 * All parachute functions/decisions/everything.
 */
void parachute(float Altitude){
  if(!chute_enable && Altitude >= PARACHUTE_ARM_HEIGHT){    //9144 m == 30,000 feet
    saftey_counter++;
    if(saftey_counter >= 4){
      chute_enable = true;
      Serial.print("Chute enabled at ");  
      Serial.print(Altitude);
      Serial.println(" meters ");
      EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
      EagleEyeData.print("Chute enabled at ");
      EagleEyeData.print(Altitude); 
      EagleEyeData.println(" meters ");
      EagleEyeData.close();
    }
    else if(Altitude <= PARACHUTE_ARM_HEIGHT){  //Resets saftey counter to 0
      saftey_counter = 0;
      Serial.println("Saftey reset to 0.");
      EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
      EagleEyeData.println("Saftey reset to 0."); 
      EagleEyeData.close();  
    }
  }
  
  if(!chute_deploy && chute_enable && Altitude <= PARACHUTE_DEPLOY_HEIGHT){  //6096m == 20,000 feet
    digitalWrite(RELAY1, LOW);                //This is close the circuit providing power the chute deployment system
    chute_deploy = true;
    Serial.print("Chute deployed at ");
    Serial.print(Altitude);
    Serial.println(" meters");
    delay(2000);


    
    digitalWrite(RELAY1, HIGH);               //Run the current for 2 seconds, then open the circuit and stop the current
    EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
    EagleEyeData.print("Chute deployed at ");
    EagleEyeData.print(Altitude);
    EagleEyeData.println(" meters");
    EagleEyeData.close();
  }
  
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
    Serial.println("Sent Handshake");
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
  if(!Local){                                      //FIGURE OUT WHERE TO UPDATE
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
 * Adjusts the delay to accomidate for motor when active.
 */
void DelayHandler(){
  if(cycle_Up || cycle_Down){       //While motor is spinning up.
    if(spin_Up || motor_Complete){  //If motor is at full power or the motor cycle is complete.
      delay(1000);
    }
    else{
      delay(300); // (3/10)th second delay when motor is active
    }
  }
  else{
    delay(1000);  //One second delay between recordings
  }
}

bool parachuteRedZone(float roll, float pitch) {
  return (roll >= 135 && pitch <= 45);
}
void gyroParaccute(float altitude) {
  sensors_vec_t   orientation;
  float roll = 180 - abs(orientation.roll);
  float pitch = abs(orientation.pitch); 
  if (!parachuteRedZone(roll,pitch)) {
    redZoneBool = true
  }
  else {
    redZoneBool = false
  }
  if (redZoneBool == true && enable == true && parachute(altitude)) {
    
  }
  
  


  
}

