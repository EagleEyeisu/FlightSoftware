/*******************************EAGLE EYE PROPRIETARY INFORMATION***************************************
 *                                                                                                     *
 *Purpose: Primary software for use on LoRa Transmiter. Handles incoming/outgoing radio signals,       *
 *                 stores and transmits GPS location of craft, works as backup parachute deployment    *
 *                 and becomes the crafts HQ if the mega were to fail.                                 *
 *                                                                                                     *
 *Date:      Version:        Developer:        Description:                                            *
 *02/9/17    1.0             Jared Danner      Initial Build.                                          *
 *02/27/17   1.1             Wesley Carelton   Fixed I2C Software                                      *
 *                           Jared Danner      Added Parsing; Removed TinyGPS Function                 *
 *03/15/17   1.2             Jared Danner      Added Radio Capability; I2C fixes                       *
 *04/03/17   1.2a            Jared Danner      Housekeeping. Cleaned up/Restructured.                  *
 *4/04/17    1.2b            James Wingerter   Fixed deployment with new relay/OR/NOT config           *
 *******************************************************************************************************/

/****LIBRARIES****/
#include "SD.h"
#include "SoftwareSerial.h"
#include "Wire.h"
#include "RH_RF95.h"
 
/****FLIGHT******/
float AltPrevious = 0.0, LatPrevious = 0.0, LonPrevious = 0.0;  //These are the previous values(the last known GPS Data), they are used incase the GPS Signal stops.
int Program_Cycle = 0;
boolean HABET_Connection = true;   //Status for Connection to HABET.
boolean DISPATCH_SIGNAL = true;    //Status to send to mega.
boolean READY_FOR_DROP = false;    //Gets turned true by Mega deciding to drop based on the 9Dof.

//CONSTANT VARIABLES
/****PINS********/
#define RELAY1  12    //Parachute Digital Pin to IN1

/****SD CARD*****/
File EagleEyeData;    //File object used to store data during flight.
#define SD_PIN 10     //CHANGE THIS TO MEGA OR FEATHER 

/****PARACHUTE***/
boolean chute_enable = false;       //Status of chute readiness.
boolean chute_deploy = false;       //Status of chute deployment.
boolean EMERGENCY_DEPLOY = false;
int saftey_counter = 0;             //Saftey counter.
int PARACHUTE_ARM_HEIGHT = 120;     //9144 m == 25,000 feet
int PARACHUTE_DEPLOY_HEIGHT = 96;   //6096m == 20,000 feet

/****COMMUNICATION****/
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7                
#define LED 13                     //Used to flash LED upon message transmission.
#define RF95_FREQ 433.0            //Frequency used on LoRa. UPDATE WITH HABET
RH_RF95 rf95(8, 7);                //Directs the radio to read from a certain port.
boolean newData = false;           //Status of event data.
int x;                             //Recieved event number.
int Lost_Packet = 0;               //Keep track of how many packets are lost.
char NMEA_Sentence[150];
boolean NMEAorDROP = true;
boolean HandShakeHABET = false;
boolean HandShake = false;

/****GPS****/
SoftwareSerial ss(11, 10);         //Directs the GPS to read from certain wire ports
int GPS_Fix = 0;                   //Keeps track of how long the GPS fix has been lost. Used in event logging.

/*
 * Holds data values of Pressure, Altitude, and Temperature
 */
struct flight_data{
  float Altitude;
  float Latitude;
  float Longitude;
};

struct flight_data data;

void setup(){
  delay(1000);
  Serial.begin(4800);

  /****Initialization of Parachute System****/
  //Set all the pins low so they do not toggle on Reset or Power on!
  digitalWrite(RELAY1, LOW);  //Sends a LOW signal
  pinMode(RELAY1, OUTPUT);     //Sets RELAY1 as output pin.
  Serial.println("Parachute Online.");

  /****Initialization of GPS Module****/
  ss.begin(9600);
  Serial.println("GPS Online");
  
  /****Initialization of SD Card reader****/
  pinMode(SD_PIN, OUTPUT);
  if(!SD.begin(SD_PIN)){
    Serial.println("PROBLEM WITH SD CARD.");
    //while(1);
  }
  else{
    Serial.println("SD Card Online.");
  }

  /****Initialization of I2C Comms****/
  Wire.begin(2);    //Setting the address for this board.
  Serial.println("Comms Address Set.");

  /****Initialization of Radio****/
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  //Adjust the LED to be insync with radio trasmission.
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
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
void loop(){
  Program_Cycle++;
  Serial.println(".");
  Blink();
  GPSData();                    //Updates altitude using GPS.
  //Save(0,0);                    //Stores Data to SD Card.
  Radio_Comm();                 //Radio communication.
  parachute();                  //Parachute functions such as enable, deploy, and saftey checks.
  BoardCommunication(false);    //Decides to Send or Recieve I2C information.
}

/*
 * Radio Communication into and out of the LoRa.
 */
void Radio_Comm(){
  if(NMEAorDROP){
    Send_Packet();
  }
  else if(HandShakeHABET){
    Send_Packet();
  }
  else if(READY_FOR_DROP){//Sends drop signal to HABET.
    Send_Packet();
    READY_FOR_DROP = false;
    NMEAorDROP = true;
    HABET_Connection = false;
    x = 4;
    Save(1,0);
  }
  if(rf95.available()){//Checks for incoming message.
    Retrieve_Packet();
  }
}

/*
 * Sends character array over the Feather 32u4.
 */
void Send_Packet(){
  if(NMEAorDROP && (Program_Cycle%12==0)){
    //Serial.print("Sending: ");Serial.println(NMEA_Sentence);
    rf95.send(NMEA_Sentence, sizeof(NMEA_Sentence));
    rf95.waitPacketSent();
  }
  else if(HandShakeHABET){
    if(Program_Cycle>25){
      HandShakeHABET = false;
      //Serial.println("HandShakeHABET Timed Out.");
      //Serial.println("Asking Mega for Drop");
      BoardCommunication(true);//Triggers the switch in I2C and asks mega for Go/NoGo on drop.
    }
    if(Program_Cycle%4==0){
      char Detach[8] = "OKHABET";
      Serial.print("Sending: ");Serial.println(Detach);
      rf95.send(Detach, sizeof(Detach));
      rf95.waitPacketSent();
    }
  }
  else if(READY_FOR_DROP){
    char Detach[5] = "DROP";
    Serial.print("Sending: ");Serial.println(Detach);
    rf95.send(Detach, sizeof(Detach));
    rf95.waitPacketSent();
  }
  else if(EMERGENCY_DEPLOY){
    if(Program_Cycle>25){
      EMERGENCY_DEPLOY = false;
      //Serial.println("HandShake with GND Timed Out.");
    }
    if(Program_Cycle%5==0){
      char Detach[3] = "EP";
      //Serial.print("Sending: ");Serial.println(Detach);
      rf95.send(Detach, sizeof(Detach));
      rf95.waitPacketSent();
    }
  }
}

/*
 * Recieves incoming transmission over the Featehr 32u4.
 */
void Retrieve_Packet(){
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if(rf95.recv(buf, &len)){//Retrieves the incoming message
    if(buf[0]=='S' && buf[1]=='T' && buf[2]=='A' && buf[3]=='R' && buf[4]=='T' && buf[5]=='E' && buf[6]=='E'){
      Serial.println("Received 'STARTEE' from Habet.");
      NMEAorDROP = false;
      HandShakeHABET = true;
      Program_Cycle = 0;
    }
    else if(buf[0]=='O' && buf[1]=='K' && buf[2]=='E' && buf[3]=='E' && !HandShake){
      HandShake = true;
      Serial.println("Drop Handhsake Recieved.");
      NMEAorDROP = true;
      READY_FOR_DROP = false;
    }
    else if((buf[0]=='E' && buf[1]=='D' && buf[2]=='E' && buf[3]=='P' && buf[4]=='L' && buf[5]=='O' && buf[6]=='Y') || (buf[0]=='F' && buf[1]=='I' && buf[2]=='N' && buf[3]=='A' && buf[4]=='L')){
      EMERGENCY_DEPLOY = true;
      if(!chute_deploy){
        //Serial.println("EMERGENCY DEPLOY RECEIVED. DEPLOYING NOW.");
        digitalWrite(RELAY1, HIGH);//This is close the circuit providing power the chute deployment system
        chute_deploy = true;
        delay(1000);
        digitalWrite(RELAY1, LOW);//Run the current for 2 seconds, then open the circuit and stop the current
      }
    }
  }
}

/*
 * Blinks LED.
 */
void Blink(){
  digitalWrite(LED, HIGH);
  delay(10);
  digitalWrite(LED, LOW);
}

/*
 * Responsible for updating and recieving information directly from GPS.
 */
void GPSData(){
  new_NMEA();
  GPS_Fix = parse_NMEA(3);//Checks for fix
  if(GPS_Fix==0){         //no fix
    //Serial.println("NO SIGNAL");
    x = 3;
    Save(1,0);
    Revert_Struct();
  }
  else if(GPS_Fix==1){
    //Serial.println("SIGNAL");
    data.Altitude = parse_NMEA(0);
    data.Latitude = parse_NMEA(1);
    data.Longitude = parse_NMEA(2);
    if(data.Altitude<100.0 || data.Altitude>150000.0){ // Secondary saftey filter to not read the not wanted NMEA sentences
      Revert_Struct();
    }
    AltPrevious = data.Altitude;
    LonPrevious = data.Longitude;
    LatPrevious = data.Latitude;
  }
  else{//Wrong NMEA sentence, ingores it.
    Revert_Struct();
  }
  //Serial.print("Alt: ");Serial.println(data.Altitude,6);
  //Serial.print("Lon: ");Serial.println(data.Longitude,6);
  //Serial.print("Lat: ");Serial.println(data.Latitude,6);
  //Serial.println();
}

/*
 * Updates data struct with previous values.
 */
void Revert_Struct(){
  data.Altitude = AltPrevious;
  data.Longitude = LonPrevious;
  data.Latitude = LatPrevious;
}

/*
 * Responsible for updating and recieving information directly from GPS.
 */
void new_NMEA(){
  int i = 0;
  for(i=0;i<150;i++){
    NMEA_Sentence[i] = ' ';
    NMEA_Sentence[i] = ' ';
  }
  unsigned long start = millis();
  char Arr[150];
  i = 0;
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
        NMEA_Sentence[j] = Arr[i];
        j++;
      }
     i++;
     }
   }while(millis() - start < 1000);
}

/*
 * Parsing method for GPS.
 */
float parse_NMEA(int objective){
  int GoalNumber;//Target comma number
  if(objective==0){//ALTITUDE
    GoalNumber = 9;//9th comma
  }
  else if(objective==1){//LATITUDE
    GoalNumber = 2;     //2nd comma
  }
  else if(objective==2){//LONGITUDE
    GoalNumber = 4;     //4th comma
  }
  else if(objective==3){//FIX
    GoalNumber = 6;     //6th comma
  }
  int i;
  int t = 0;
  int Comma_Counter = 0;     //comma counter
  boolean Goal = false;      //True if the NMEA is reading the objective
  String two = "                   ";  //Temp string to capture wanted information
  for(i=0;i<120;i++){
    if(NMEA_Sentence[i]==','){//Checks for a comma in the NMEA
      Comma_Counter++;
    }
    else if(Comma_Counter==GoalNumber){//Once targetted comma is passed. Record until next comma
      if(NMEA_Sentence[i]!=','){
        two[t] = NMEA_Sentence[i];
        t++;
      }
    }
  }
  char arr[20];
  for(i=0;i<20;i++){
    arr[i]=two[i];
  }
  float temp = atof(arr);//Converts char array to float
  return temp;
}

/*
 * Stores Data from all sensors to SD Card.
 * I2C_Selector:
 * 0 - Does not involve I2C
 * 1 - Saves Event and Alttude.
 * Parachute_Selector:
 * 0 - Does not involve Parachute.
 * 1 - Enable Chute.
 * 2 - Deploy Chute.
 * 3 - Reset saftey counter.
 */
void Save(int I2C_Selector, int Parachute_Selector){
  if(I2C_Selector != 0){
    Save_I2C();
  }
  else if(Parachute_Selector != 0){
    Save_Parachute(Parachute_Selector);
  }
  else{
    Save_NMEA();
  }
}

/*
 * Saves I2C Data.
 */
void Save_I2C(){
  EagleEyeData = SD.open("EventLog.txt", FILE_WRITE);
  EagleEyeData.println();
  EagleEyeData.print(x);
  EagleEyeData.println(data.Altitude);
  EagleEyeData.close();
}

/*
 * Saves Parachute Data.
 */
void Save_Parachute(int Parachute_Selector){
  EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
  if(Parachute_Selector==1){    //SAVES PARACHUTE ENABLEMENT.
    EagleEyeData.print("ENABLED: ");
    EagleEyeData.println(data.Altitude); 
  }
  else if(Parachute_Selector==2){ //SAVES PARACHUTE DEPLOYMENT.
    EagleEyeData.print("DEPLOYED: ");
    EagleEyeData.println(data.Altitude);
  }
  else{  //SAVES RESET OF SAFTEY COUNTRER.
    EagleEyeData.println("Saftey reset to 0.");  
  }
  EagleEyeData.close();
}
 
/*
 * Stores GPS Data.
 */
void Save_NMEA(){
  EagleEyeData = SD.open("GPS_NMEA.txt", FILE_WRITE);
  EagleEyeData.print(data.Altitude,6);
  EagleEyeData.print(",");
  EagleEyeData.print(data.Latitude,6);
  EagleEyeData.print(",");
  EagleEyeData.print(data.Longitude,6);
  EagleEyeData.close();
}

/*
 * Handles all parachute functions.
 */
void parachute(){
  if(!chute_enable && data.Altitude >= PARACHUTE_ARM_HEIGHT){//9144 m == 30,000 feet
    saftey_counter++;
    if(saftey_counter >= 4){
      chute_enable = true;
      Send_I2C(1);
      //Serial.print("ENABLED: ");Serial.println(data.Altitude);
      Save(0,1);
    }
    else if(data.Altitude <= PARACHUTE_ARM_HEIGHT){//Resets saftey counter to 0
      saftey_counter = 0;
      //Serial.println("Saftey reset to 0.");
      Save(0,3);
    }
  }
  if(!chute_deploy && chute_enable && data.Altitude <= PARACHUTE_DEPLOY_HEIGHT){  //6096m == 20,000 feet
    digitalWrite(RELAY1, HIGH);//This is close the circuit providing power the chute deployment system
    chute_deploy = true;
    Save(1,0);
    Send_I2C(2);
    //Serial.print("DEPLOY: ");Serial.print(data.Altitude);Serial.println(" meters");
    delay(2000);
    digitalWrite(RELAY1, LOW);//Run the current for 2 seconds, then open the circuit and stop the current
    Save(0,2);
  }
}

/*
 * Helper to determine whether to send or recieve signal.
 */
void BoardCommunication(boolean DROP_SIGNAL){
  if(DROP_SIGNAL){
    Send_I2C(8);
    DISPATCH_SIGNAL = false;
    Serial.println("Receiving Mode");
  }
  if(!DISPATCH_SIGNAL){
    Receive_I2C();
  }
}

/*
 * Sends byte over I2C Connection.
 */
void Send_I2C(int System_Event){
  Wire.beginTransmission(1);
  Wire.write(System_Event);
  Wire.endTransmission();
}

/*
 * Recieves byte over I2C Connection.
 */
void Receive_I2C(){
  Wire.onReceive(receiveEvent);
  if(newData){
    Save(1,0);
    newData = false;
  }
}

/**
 * Helper for radio
 */
void receiveEvent(){
  //Serial.print("Event Received: ");
  x = Wire.read();    //Receive byte as an integer
  //Serial.println(x);
  if(x == 9){
    READY_FOR_DROP = true;
  }
  else if(x==13){
    Serial.println("Handshake Recieved");
  }
  newData = true;
}


