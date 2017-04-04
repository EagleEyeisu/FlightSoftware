
/****LIBRARIES****/
#include <SD.h>
#include <SPI.h>
#include <RH_RF95.h>

/****SC CARD****/
File GSData;    //File object used to store data during flight.
#define SD_PIN 10

/***COMMUNICATION****/
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13
#define RF95_FREQ 433.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
boolean Drop_Sequence = false;

/****GPS****/
char NMEA_Sentence[150];

/*
 * Holds data values of NMEA Sentences.
 */
struct GPS_data{
  float Altitude;
  float Latitude;
  float Longitude;
  float Fix;
  float Sat_Count;
};
struct GPS_data data;

void setup(){
  delay(1000);
  Serial.begin(4800);
  while(!Serial);


  /****Initialization of SD Card reader****/
  pinMode(SD_PIN, OUTPUT);
  if(!SD.begin(SD_PIN)){
    Serial.println("PROBLEM WITH SD CARD.");
    //while(1);
  }
  else{
    Serial.println("SD Card Online.");
  }
  
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

/*
 * MAIN PROGRAM CODE.
 */
void loop() {
  Serial.println(".");
  Receive();
  Start_Drop();
  delay(1000);
}

/*
 * Sends the "ARE YOU READY TO DROP" signal which starts the detach sequence.
 */
void Start_Drop(){
  if(Serial.read() == 'BEGIN_DROP'){
    char Packet[10] = "START";
    Serial.print("Sending: "); Serial.println(Packet);
    rf95.send(Packet, sizeof(Packet));
    rf95.waitPacketSent();
    Drop_Sequence = true;
  }
}

/*
 * Receives Data from craft.
 */
void Receive(){
  if(rf95.available()){
    uint8_t buf[150];
    uint8_t len = sizeof(buf);
    if(rf95.recv(buf, &len)){ //Processes Response
      if(buf[1]=='G' && buf[2]=='P' && buf[3]=='G' && buf[4]=='G' && !Drop_Sequence){
        //Serial.print("Recieved: ");
        //Serial.println((char*)buf);
        for(int i=0;i<150;i++){
          NMEA_Sentence[i] = buf[i];
        }
        Save(1);
        Data_Parsing();
        Save(2);
      }
      else if(buf[0]=='D' && buf[2]=='T' && buf[5]=='H' && buf[7]=='D' && Drop_Sequence){
        Serial.println("DETACH CONFIRMED");
        Drop_Sequence = false;
      }
    }
    else{
      Serial.println("Receive failed");
    }
  }
}

/*
 * Parses Data from crafts NMEA.
 */
void Data_Parsing(){
  data.Altitude = parse_NMEA(0);
  data.Latitude = parse_NMEA(1);
  data.Longitude = parse_NMEA(2);
  data.Fix = parse_NMEA(3);
  data.Sat_Count = parse_NMEA(4);
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
  else if(objective==4){//SATILITE COUNT
    GoalNumber = 7;     //7th comma
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
 * Stores Data from NMEA.
 * Data_Selector
 * 1 - Stores Full NMEA Sentence.
 * 2 - Stores Parsed Data.
 */
void Save(int Selector){
  if(Selector == 1){
    GSData = SD.open("NMEA.txt", FILE_WRITE);
    for(int i=0;i<150;i++){
       //GSData.print(NMEA_Sentence[i]);
    }
    GSData.println();
    GSData.close();
  }
  else if(Selector == 2){
    GSData = SD.open("Parsed_Data.txt", FILE_WRITE);
    GSData.print(data.Altitude);
    GSData.print(",");
    GSData.print(data.Latitude);
    GSData.print(",");
    GSData.print(data.Longitude);
    GSData.print(",");
    GSData.print(data.Fix);
    GSData.print(",");
    GSData.print(data.Sat_Count);
    GSData.close();
  }
}
