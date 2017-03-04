/*******************************EAGLE EYE PROPRIETARY INFORMATION***************************************
 *                                                                                                     *
 *Purpose: Primary software for use on LoRa Transmiter. Handles incoming/outgoing radio signals,       *
 *                 stores and transmits GPS location of craft, works as backup parachute deployment    *
 *                 and becomes the crafts HQ if the mega were to fail.                                 *
 *                                                                                                     *
 *Date:      Version:        Developer:        Description:                                            *
 *2/9/17     1.0             Jared Danner      Initial Build.                                          *
 *2/27/17    1.1             Wesley Carelton   Fixed I2C Software                                      *
 *                           Jared Danner      Added Parsing; Removed TinyGPS Function; Cleaned up     *
 *******************************************************************************************************/

//LIBRARIES
/****ALTITUDE****/
#include <SD.h>
#include <SoftwareSerial.h> 

/****FLIGHT******/
#include <Wire.h>

//CONSTANT VARIABLES
/****PINS********/
#define RELAY1  28    //Parachute Digital Pin to IN1

/****SD CARD*****/
File EagleEyeData;    //File used to store the flight data. Data will be written/saved to this file during flight operations
#define SD_PIN 10     //CHANGE THIS TO MEGA OR FEATHER 

/****PARACHUTE***/
boolean chute_enable = false;       //Status of chute readiness.
boolean chute_deploy = false;       //Status of chute deployment.
int saftey_counter = 0;             //Saftey counter.
int PARACHUTE_ARM_HEIGHT = 30000;    //9144 m == 30,000 feet
int PARACHUTE_DEPLOY_HEIGHT = 20000;  //6096m == 20,000 feet

/****COMMUNICATION****/
boolean HABET_Connection = true; //Status for Connection to HABET.
boolean DISPATCH_SIGNAL = true;

/****GPS****/
String NMEA;                     //NMEA that is read in from GPS
SoftwareSerial ss(3, 2);         //NEED TO UPDATE WIRES FOR MEGA.

/****COMMAND****/
boolean COMMAND_CENTER = false;
boolean no_Response = false;

/*
 * Holds data values of Pressure, Altitude, and Temperature
 */
struct flight_data{
  float Altitude;
  float Latitude;
  float Longitude;
  float Speed;
  unsigned long Time;
};

void setup(){
  Serial.begin(4800);

  /****Parachute deployment Initialize****/
  //Set all the pins low so they do not toggle on Reset or Power on!
  digitalWrite(RELAY1, HIGH);  //Sends a LOW signal
  pinMode(RELAY1, OUTPUT);     //Sets RELAY1 as output pin.
  Serial.println("Parachute Online.");

  /****Initialize GPS Module****/
  ss.begin(9600);
  
  /****Initialize SD Card reader****/
  Serial.println("SD Card Online.");
  pinMode(SD_PIN, OUTPUT);

  /****Initialize I2C Comms****/
  Wire.begin(2);    //Setting the address for this board.
  Serial.println("Comms Address Set.\n\n");
}

/*
 * MAIN PROGRAM
 */
void loop() {
  flight_data current = GPSData();                                  //Updates altitude using GPS.
  RADIO_Comm();                                                     //Radio communication.
  storeData(current.Altitude,current.Latitude,current.Longitude,current.Speed,current.Time); //Stores Data to SD Card.
  parachute(current.Altitude,current.Time);                         //Parachute functions such as enable, deploy, and saftey checks.
  smartdelay(1000);                                                 //Delays in the GPS data collection, not at end of cycle.
  TouchDown(current.Altitude,current.Time);                         //Signals Touchdown signal to MEGA and LoRa if true.
}

/*
 * Radio Communication into and out of the LoRa.
 */
void RADIO_Comm(){
  
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
    if(result>Alt-5.0 && result<Alt+5.0){
      Touchdown = true;
      MEGA_Comm(Alt,DISPATCH_SIGNAL,7,Time);
    }
  }
}
/*
 * Handles all parachute functions.
 */
void parachute(float Altitude){
  if(!chute_enable && Altitude >= PARACHUTE_ARM_HEIGHT){    //9144 m == 30,000 feet
    EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
    saftey_counter++;
    if(saftey_counter >= 4){
      chute_enable = true;
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
 * Responsible for updating and recieving information directly from GPS.
 */
struct flight_data GPS(){
  NMEA = "                                                        ";
  unsigned long start = millis();
  char Arr[150];
  int i = 0;
  int j = 0;
  do 
  {
    while (ss.available()){
      Arr[i] = ss.read();
      if(Arr[i]=='$'){
        dollar_counter++;
      }
      if(dollar_counter==1){
        NMEA[j] = Arr[i];
        j++;
      }
    }
  }while(millis() - start < 1000);
  
  flight_data data;
  data.Altitude = parse_GPS(NMEA);
  return data;
}

/*
 * Parsing method for GPS.
 */
float parse_GPS(String NMEA){
  int T_GoalNumber = 9;   //9th comma in the NMEA sentense signals the beginning of the altitude value.
  boolean T_Goal = false;
  int C = 0;
  String two = "                   ";
  int t = 0;
  for(int i=0;i<120;i++){
    if(NMEA[i]==','){
      C++;
    }
    else if(C==T_GoalNumber){
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
  float temp_Alt = atof(arr);
  Serial.println(temp_Alt);
  return temp_Alt;
}

/*
 * Responsible for updating and recieving information directly from GPS.
 */
struct flight_data GPSData(){
  flight_data data;
  if(gps.f_altitude()==0.000000){
    Serial.println("NO SIGNAL");
    data.Altitude = AltPrevious;
    data.Longitude = LonPrevious;
    data.Latitude = LatPrevious;
    data.Speed = SpePrevious;
    data.Time = TimePrevious;
    if(Fixed_Lost==0){
      Fixed_Lost++;
      MEGA_Comm(data.Altitude,DISPATCH_SIGNAL,3,data.Time);
    }
  }
  else{
    Serial.println("SIGNAL");
    Fixed_Lost = 0;
    float lon, lat;
    unsigned long age;
    gps.f_get_position(&lat, &lon, &age);  //Saves latitude, longitude, and age of data.
    data.Altitude = gps.f_altitude();
    data.Longitude = lon;
    data.Latitude = lat;
    data.Speed = gps.f_speed_mps();
    unsigned long junk, junk1;
    gps.get_datetime(&junk,&data.Time,&junk1);
    AltPrevious = data.Altitude;
    LonPrevious = data.Longitude;
    LatPrevious = data.Latitude;
    SpePrevious = data.Speed;
    TimePrevious = data.Time;
    
    Serial.print("Alt: ");
    Serial.println(data.Altitude,6);
    Serial.print("Lon: ");
    Serial.println(data.Longitude,6);
    Serial.print("Lat: ");
    Serial.println(data.Latitude,6);
    Serial.print("Speed: ");
    Serial.println(data.Speed,6);
    Serial.println();
  }
  return data;
}

/*
 * Used to store Data to SD card storage
 */
void storeData(float Altitude){
  EagleEyeData = SD.open("GPS_NMEA.txt", FILE_WRITE);
  EagleEyeData.print(Time);
  EagleEyeData.print(",");
  EagleEyeData.print(Lat);
  EagleEyeData.print(",");
  EagleEyeData.print(Lon);
  EagleEyeData.print(",");
  EagleEyeData.print(Alt);
  EagleEyeData.print(",");
  EagleEyeData.print(Spe);
  EagleEyeData.print(",");
  EagleEyeData.println(Time);
  EagleEyeData.close();
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
 *  9 - Test
 */
void MEGA_Comm(float Altitude, boolean Send, int LoRa_Event, unsigned long Time){
  EagleEyeData = SD.open("EventLog.txt", FILE_WRITE);
  if(Send){
    byte x = LoRa_Event;
    Wire.beginTransmission(1);
    Wire.write(x);
    Wire.endTransmission();
    EagleEyeData.print(IN[LoRa_Event]);
    EagleEyeData.print(" <-Sent to Mega at ALT: ");
    Serial.println(x);
  }
  else{
    EagleEyeData.print(LoRa_Event);
    EagleEyeData.print(" <-Event Logged at ALT: ");
  }
  EagleEyeData.print(Altitude);
  EagleEyeData.print(" at flight TIME: ");
  EagleEyeData.println(Time);
  EagleEyeData.close();
}
