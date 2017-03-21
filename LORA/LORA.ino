/*******************************EAGLE EYE PROPRIETARY INFORMATION***************************************
 *                                                                                                     *
 *Purpose: Primary software for use on LoRa Transmiter. Handles incoming/outgoing radio signals,       *
 *                 stores and transmits GPS location of craft, works as backup parachute deployment    *
 *                 and becomes the crafts HQ if the mega were to fail.                                 *
 *                                                                                                     *
 *Date:      Version:        Developer:        Description:                                            *
 *2/9/17     1.0             Jared Danner      Initial Build.                                          *
 *2/27/17    1.1             Wesley Carelton   Fixed I2C Software                                      *
 *                           Jared Danner      Added Parsing; Removed TinyGPS Function                 *
 *3/15/17    1.2             Jared Danner      Added Radio Capability; I2C fixes                       *
 *******************************************************************************************************/

/****LIBRARIES****/
#include <SD.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <RH_RF95.h>
 
/****FLIGHT******/
float Alts[20];
boolean Touchdown = false;
float AltPrevious, LatPrevious, LonPrevious;  //These are the previous values(the last known GPS Data), they are used incase the GPS Signal stops.
unsigned long TimePrevious;

//CONSTANT VARIABLES
/****PINS********/
#define RELAY1  28    //Parachute Digital Pin to IN1

/****SD CARD*****/
File EagleEyeData;    //File object used to store data during flight.
#define SD_PIN 10     //CHANGE THIS TO MEGA OR FEATHER 

/****PARACHUTE***/
boolean chute_enable = false;       //Status of chute readiness.
boolean chute_deploy = false;       //Status of chute deployment.
int saftey_counter = 0;             //Saftey counter.
int PARACHUTE_ARM_HEIGHT = 9144;    //9144 m == 30,000 feet
int PARACHUTE_DEPLOY_HEIGHT = 6096; //6096m == 20,000 feet

/****COMMUNICATION****/
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7                
#define LED 13                     //Used to flash LED upon message transmission.
#define RF95_FREQ 433.0            //Frequency used on LoRa. UPDATE WITH HABET
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Directs the radio to read from a certain port.
boolean HABET_Connection = true;   //Status for Connection to HABET.
boolean DISPATCH_SIGNAL = true;    //Status to send to mega.
boolean READY_FOR_DROP = false;    //Gets turned true by Mega deciding to drop based on the 9Dof.
boolean newData = false;           //Status of event data.
int x;                             //Recieved event number.
int Lost_Packet = 0;               //Keep track of how many packets are lost.

/****GPS****/
String NMEA;                     //NMEA that is read in from GPS.
SoftwareSerial ss(11, 10);         //Directs the GPS to read from certain wire ports
int Fixed_Lost = 0;              //Keeps track of how long the GPS fix has been lost. Used in event logging.

/*
 * Holds data values of Pressure, Altitude, and Temperature
 */
struct flight_data{
  float Altitude;
  float Latitude;
  float Longitude;
  unsigned long Time;
};

void setup(){
  delay(1000);
  Serial.begin(4800);

  /****Initialization of Parachute System****/
  //Set all the pins low so they do not toggle on Reset or Power on!
  digitalWrite(RELAY1, HIGH);  //Sends a LOW signal
  pinMode(RELAY1, OUTPUT);     //Sets RELAY1 as output pin.
  Serial.println("Parachute Online.");

  /****Initialization of GPS Module****/
  ss.begin(9600);
  Serial.println("GPS Online");
  
  /****Initialization of SD Card reader****/
  Serial.println("SD Card Online.");
  pinMode(SD_PIN, OUTPUT);

  /****Initialization of I2C Comms****/
  Wire.begin(2);    //Setting the address for this board.
  Serial.println("Comms Address Set.");

  /****Initialization of certain Variables****/
  AltPrevious=0.0; LatPrevious=0.0; LonPrevious=0.0; TimePrevious=0.0;

  /****Initialization of Radio****/
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  //Adjust the LED to be insync with radio trasmission.
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  //Checks for Creation of Radio Object.
  if(!rf95.init()){
    Serial.println("LoRa radio init failed");
    while (1);
  }
  else{
    Serial.println("LoRa radio init OK!");
  }
  //Checks for Frequency
  if(!rf95.setFrequency(RF95_FREQ)){
    Serial.println("setFrequency failed");
    while (1);
  }
  else{
    Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  }
  rf95.setTxPower(23, false);
  Serial.println();
}

/*
 * MAIN PROGRAM
 */
void loop() {
  flight_data current = GPSData();                                             //Updates altitude using GPS.
  Radio_Comm(current.Altitude,current.Time);                                   //Radio communication.
  storeData(current.Altitude,current.Latitude,current.Longitude,current.Time); //Stores Data to SD Card.
  parachute(current.Altitude,current.Time);                                    //Parachute functions such as enable, deploy, and saftey checks.
  TouchDown(current.Altitude,current.Time);                                    //Signals Touchdown signal to MEGA and LoRa if true.
  BoardCommunication(current.Altitude,current.Time,false);                     //Decides to Send or Recieve I2C information.
}

/*
 * Radio Communication into and out of the LoRa.
 */
void Radio_Comm(float Altitude,float Time){
  //Serial.println(".");
  if(READY_FOR_DROP){
    char data[10] = "DROP";
    Serial.print("Sending drop signal: "); Serial.println(data);
    rf95.send(data, sizeof(data));       //Sends packet
    rf95.waitPacketSent();
    digitalWrite(LED, HIGH);
    delay(10);                           //Allows for light to turn on log enough to be seen.
    digitalWrite(LED, LOW);
    HABET_Connection = false;
    I2C(Altitude,true,DISPATCH_SIGNAL,4,Time);
  }
  if (rf95.available()){                 //Checks if there is a incoming message
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);           //Temporary variable to hold the message
    
    if(rf95.recv(buf, &len)){            //Retrieves the incoming message
      digitalWrite(LED, HIGH);
      delay(10);                         //Allows for light to turn on log enough to be seen.
      digitalWrite(LED, LOW);
      Serial.print("Recieved: ");
      Serial.println((char*)buf);
      if(true){ //Will be switched to only trigger on specific message
        //BoardCommunication(Altitude,Time,true); //Triggers the switch in I2C and asks mega for Go/NoGo on drop.
      }
    }
    else{
      Lost_Packet++;
      Serial.println("Packet Lost");
    }
    Serial.println();
  }
}

/*
 * Responsible for updating and recieving information directly from GPS.
 */
struct flight_data GPSData(){
  flight_data data;
  new_NMEA();
  Fixed_Lost = parse_NMEA(4); //Checks for fix
  if(Fixed_Lost==0){//no fix
    Serial.println("NO SIGNAL");
    I2C(data.Altitude,true,DISPATCH_SIGNAL,3,data.Time);
    data.Altitude = AltPrevious;
    data.Longitude = LonPrevious;
    data.Latitude = LatPrevious;
    data.Time = TimePrevious;
    Serial.print("Alt: ");
    Serial.println(data.Altitude,6);
    Serial.print("Lon: ");
    Serial.println(data.Longitude,6);
    Serial.print("Lat: ");
    Serial.println(data.Latitude,6);
    Serial.println();
  }
  else{
    Serial.println("SIGNAL");
    Fixed_Lost = 0;
    data.Altitude = parse_NMEA(0);
    data.Latitude = parse_NMEA(1);
    data.Longitude = parse_NMEA(2);
    data.Time = parse_NMEA(3);
    if(data.Altitude<100.0 || data.Altitude>150000.0){ // Secondary saftey filter to not read the not wanted NMEA sentences
      data.Altitude = AltPrevious;
      data.Longitude = LonPrevious;
      data.Latitude = LatPrevious;
      data.Time = TimePrevious;
    }
    AltPrevious = data.Altitude;
    LonPrevious = data.Longitude;
    LatPrevious = data.Latitude;
    TimePrevious = data.Time;
    
    Serial.print("Alt: ");
    Serial.println(data.Altitude,6);
    Serial.print("Lon: ");
    Serial.println(data.Longitude,6);
    Serial.print("Lat: ");
    Serial.println(data.Latitude,6);
    Serial.println();
  }
  return data;
}

/*
 * Responsible for updating and recieving information directly from GPS.
 */
void new_NMEA(){
  NMEA = "                                                              ";
  unsigned long start = millis();
  char Arr[150];
  int i = 0;
  int j = 0;
  int dollar_counter=0;
  do 
  {
    while(ss.available()){
     Arr[i] = ss.read();
     if(Arr[i]=='$'){
       dollar_counter++;
     }
     if(dollar_counter==1){
       NMEA[j] = Arr[i];
       j++;
     }
     i++;
    }
  }while(millis() - start < 1000);
  Serial.println(NMEA);
}

/*
 * Parsing method for GPS.
 */
float parse_NMEA(int objective){
  int GoalNumber;  //Target comma number
  if(objective == 0){ //ALTITUDE
    GoalNumber = 9; //9th comma
  }
  else if(objective == 1){ //LATITUDE
    GoalNumber = 2; //2nd comma
  }
  else if(objective == 2){ //LONGITUDE
    GoalNumber = 4; //4th comma
  }
  else if(objective == 3){ //TIME
    GoalNumber = 1; //1st comma
  }
  else if(objective == 4){
    GoalNumber = 6; //6th comma
  }
  
  boolean Goal = false;   //True if the NMEA is reading the objective
  int Comma_Counter = 0;  //comma counter
  String two = "                   ";  //Temp string to capture wanted information
  int t = 0;
  for(int i=0;i<120;i++){
    if(NMEA[i]==','){  //Checks for a comma in the NMEA
      Comma_Counter++;
    }
    else if(Comma_Counter==GoalNumber){  //Once targetted comma is passed. Record until next comma
      if(NMEA[i]!=','){
        two[t] = NMEA[i];
        t++;
      }
    }
  }
  char arr[20];
  for(int i=0;i<20;i++){
    arr[i]=two[i];
  }
  float temp = atof(arr);  //Converts char array to float
  return temp;
}

/*
 * Used to store Data to SD card storage
 */
void storeData(float Alt,float Lat,float Lon,unsigned long Time){
  EagleEyeData = SD.open("GPS_NMEA.txt", FILE_WRITE);
  EagleEyeData.print(Time);
  EagleEyeData.print(",");
  EagleEyeData.print(Lat);
  EagleEyeData.print(",");
  EagleEyeData.print(Lon);
  EagleEyeData.print(",");
  EagleEyeData.print(Alt);
  EagleEyeData.print(",");
  EagleEyeData.println(Time);
  EagleEyeData.close();
}

/*
 * Handles all parachute functions.
 */
void parachute(float Altitude,float Time){
  if(!chute_enable && Altitude >= PARACHUTE_ARM_HEIGHT){    //9144 m == 30,000 feet
    EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
    saftey_counter++;
    if(saftey_counter >= 4){
      chute_enable = true;
      I2C(Altitude,false,DISPATCH_SIGNAL,1,Time);
      Serial.print("Chute enabled at ");  
      Serial.println(Altitude);
      EagleEyeData.print("Chute enabled at: ");
      EagleEyeData.println(Altitude); 
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
    I2C(Altitude,true,DISPATCH_SIGNAL,2,Time);
    Serial.print("Chute deployed at: ");
    Serial.println(Altitude);
    delay(2000);
    digitalWrite(RELAY1, HIGH);               //Run the current for 2 seconds, then open the circuit and stop the current
    EagleEyeData.print("Chute deployed at: ");
    EagleEyeData.println(Altitude);
  }
  EagleEyeData.close();
}

/*
 * Checks to see if touchdown has occured.
 */
void TouchDown(float Alt, unsigned long Time){
  Alts[20] = 0;
  int i;
  for(i=19;i>0;i--){
    Alts[i] = Alts[i+1];
  }
  Alts[0] = Alt;
  boolean fullArr = true;
  for(i=0;i<20;i++){
    if(Alts[i]==0.0){
      fullArr = false;
    }
  }
  if(fullArr == true){
    float sum;
    for(i=0;i<20;i++){
      sum += Alts[i];
    }
    float result = sum/20.0;
    if(result>Alt-5.0 && result<Alt+5.0 && !Touchdown){
      Touchdown = true;
      I2C(Alt,false,DISPATCH_SIGNAL,7,Time);
    }
  }
}

/*
 * Helper to determine whether to send or recieve signal.
 */
void BoardCommunication(float Altitude,unsigned long Time, boolean DROP_SIGNAL){
  if(DROP_SIGNAL){
    I2C(Altitude,false,DISPATCH_SIGNAL,8,Time);
    DISPATCH_SIGNAL = false;
    Serial.println("Receiving Mode");
    DROP_SIGNAL = false;
  }
  if(!DISPATCH_SIGNAL){
    I2C(Altitude,false,DISPATCH_SIGNAL,0,Time);
  }
}

/**
 * Handles Event Logging. Sends MEGA milestone updates/errors.
 *  LORA EVENTS
 *  0 - Chute Disabled
 *  1 - Chute Enabled
 *  2 - Chute Deployed
 *  3 - GPS Offline
 *  4 - Detached
 *  5 - Abort Detach
 *  6 - Radio Connection Lost
 *  7 - TouchDown
 *  8 - Switch Communication Direction
 *  9 - HABET DROP REQUEST
 */
void I2C(float Altitude,boolean Local,boolean Send,int System_Event,unsigned long Time){
  EagleEyeData = SD.open("EventLog.txt", FILE_WRITE);
  if(!Local){
    if(Send){ //SEND TO MEGA
      byte x = System_Event;
      Wire.beginTransmission(1);
      Wire.write(x);
      Wire.endTransmission();
      EagleEyeData.print(System_Event);
      EagleEyeData.print(" <-Sent to Mega at ALT: ");
      EagleEyeData.print(Altitude);
      EagleEyeData.print(" at flight TIME: ");
      EagleEyeData.println(Time);
    }
    else{ //RECIEVE FROM MEGA
      Wire.onReceive(receiveEvent);
      if(newData){
        EagleEyeData.println();
        EagleEyeData.print(x);
        EagleEyeData.print(" <-MEGA Event Logged at ALT: ");
        EagleEyeData.print(Altitude);
        EagleEyeData.print(" at flight TIME: ");
        EagleEyeData.println(Time);
        newData = false;
      }
    }
  }
  else{
    EagleEyeData.print(System_Event);
    EagleEyeData.print(" <-Event Logged at ALT: ");
    EagleEyeData.print(Altitude);
    EagleEyeData.print(" at flight TIME: ");
    EagleEyeData.println(Time);
  }
  EagleEyeData.close();
}

/**
 * Helper for radio
 */
void receiveEvent(){
  Serial.print("Event Received: ");
  x = Wire.read();    //Receive byte as an integer
  if(x == 9){
    READY_FOR_DROP = true;
  }
  Serial.println(x);  //Print the integer
  newData = true;
}
