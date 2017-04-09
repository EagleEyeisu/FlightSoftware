/*******************************EAGLE EYE PROPRIETARY INFORMATION***************************************
 *                                                                                                     *
   Purpose: Primary software for use of the Ground Station for the Eagle Eye Project. Monitors
                    live data feed from the LORA on the Eagle Eye Craft. Has the ability to begin
                    the Detach process.
 *                                                                                                     *
  Date:      Version:        Developer:        Description:
  04/04/17   1.0             Jared Danner      Initial Build.
  04/05/17   1.0a            James Wingerter   Bug fixes, added global Send_Handshake boolean
 *******************************************************************************************************/

/****LIBRARIES****/
#include <SD.h>
#include <RH_RF95.h>

/****SYSTEM INFO****/
int Program_Cycle = 0;

/****SD CARD****/
File GSData;
#define SD_PIN 10

/****COMMUNICATION****/
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13
#define RF95_FREQ 433.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
boolean Drop_Signal = false;
boolean Send_Handshake = false;

/****GPS****/
char NMEA_Sentence[150];

/*
   Holds data values of NMEA Sentences.
*/
struct GPS_data {
  float Altitude;
  float Latitude;
  float Longitude;
  float Fix;
  float Sat_Count;
};
struct GPS_data data;

void setup() {
  delay(1000);
  Serial.begin(4800);

  /****Initialization of SD Card reader****/
  pinMode(SD_PIN, OUTPUT);
  if (!SD.begin(SD_PIN)) {
    Serial.println("PROBLEM WITH SD CARD.");
    //while(1);
  }
  else {
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
  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  else {
    Serial.println("LoRa radio init OK!");
  }
  //Checks for Frequency
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  else {
    Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  }
  rf95.setTxPower(23, false);
  Serial.println();
}

/*
   MAIN PROGRAM CODE.
*/
void loop() {
  Serial.println(".");
  Receive();
  Start_Drop();
  delay(1000);
}

/*
   Sends the "ARE YOU READY TO DROP" signal which starts the detach sequence.
*/
void Start_Drop() {
  if (Serial.read() == 'a') {
    Drop_Signal = true;
    Program_Cycle = 0;
  }
  if (Drop_Signal) { //SENDS STARTING SIGNAL TO HABET.
    Program_Cycle++;
    if (Program_Cycle % 5 == 0) { //Sends only 1 out of every 5 seconds.
      char Packet[10] = "START";
      Serial.print("Sending: "); Serial.println(Packet);
      rf95.send(Packet, sizeof(Packet));
      rf95.waitPacketSent();
    }
  }
  else if (Send_Handshake) { //SENDS FINAL HANDSHAKE TO HABET.
    Program_Cycle++;
    if (Program_Cycle > 25) {
      Send_Handshake = false;
      Serial.println("Final Handshake Timeout.");
    }
    if (Program_Cycle % 5 == 0) { //Sends only 1 out of every 5 seconds.
      char Packet[15] = "ENDHABET";
      Serial.print("Sending: "); Serial.println(Packet);
      rf95.send(Packet, sizeof(Packet));
      rf95.waitPacketSent();
      Send_Handshake = false;
    }
  }
}

/*
   Receives Data.
*/
void Receive() {
  if (rf95.available()) {
    uint8_t buf[150];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) { //Processes Response
      if (buf[1] == 'G' && buf[2] == 'P' && buf[3] == 'G' && buf[4] == 'G') { //RECEIVES GPGGA Sentence
        for (int i = 0; i < 150; i++) {
          NMEA_Sentence[i] = buf[i];
        }
        Save(1);
        Data_Parsing();
        Save(2);
      }
      else if (buf[0] == 'O' && buf[1] == 'K' && buf[2] == 'G' && buf[3] == 'N' && buf[4] == 'D') { //First handshake complete. Beyond point of no return.
        Serial.println("'OKGND' Received from HABET.");
        Drop_Signal = false;
      }
      else if (buf[0] == 'F' && buf[1] == 'I' && buf[2] == 'N' && buf[3] == 'A' && buf[4] == 'L') { //Ending Transmission from habet. Our signal that the drop cycle is complete.
        Send_Handshake = true;
        Program_Cycle = 0;
        Serial.println("DETACH CONFIRMED. Fallling... Standby for Altitude changes.");
      }
      else if (buf[0] == 'S' && buf[1] == 'T' && buf[2] == 'A' && buf[3] == 'R' && buf[4] == 'T' && buf[5] == 'E' && buf[6] == 'E') { //Receives 'STARTEE'. Milestone marker.
        Serial.println("HABET HAS SENT 'STARTEE'.");
      }
      else if (buf[0] == 'O' && buf[1] == 'K' && buf[2] == 'H' && buf[3] == 'A' && buf[4] == 'B' && buf[5] == 'E' && buf[6] == 'T') { //Receives 'OKHABET'. Milestone marker.
        Serial.println("EE HAS REPLIED TO HABET WITH 'OKHABET'.");
      }
      else if (buf[0] == 'D' && buf[1] == 'R' && buf[2] == 'O' && buf[3] == 'P') {
        Serial.println("EE HAS SENT DROP SIGNAL 'DROP' TO HABET.");
      }
      else if (buf[0] == 'O' && buf[1] == 'K' && buf[2] == 'E' && buf[3] == 'E') {
        Serial.println("HABET HAS REPLIED TO EE 'OKEE'.");
      }
    }
  }
}

/*
   Stores Data from NMEA.
   Data_Selector
   1 - Stores Full NMEA Sentence.
   2 - Stores Parsed Data.
*/
void Save(int Selector) {
  if (Selector == 1) {
    GSData = SD.open("NMEA.txt", FILE_WRITE);
    for (int i = 0; i < 150; i++) {
      //GSData.print(NMEA_Sentence[i]);
    }
    GSData.println();
    GSData.close();
  }
  else if (Selector == 2) {
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

/*
   Parses Data from crafts NMEA.
*/
void Data_Parsing() {
  data.Altitude = parse_NMEA(0);
  data.Latitude = parse_NMEA(1);
  data.Longitude = parse_NMEA(2);
  data.Fix = parse_NMEA(3);
  data.Sat_Count = parse_NMEA(4);

  Serial.print("Altitude: "); Serial.print(data.Altitude); Serial.println(" m");
  Serial.print("Latitude: "); Serial.println(data.Latitude);
  Serial.print("Latitude: "); Serial.println(data.Longitude);
  Serial.print("Fix: "); Serial.println(data.Fix);
  Serial.print("Sat: "); Serial.println(data.Sat_Count);
}

/*
   Parsing method for GPS.
*/
float parse_NMEA(int objective) {
  int GoalNumber;  //Target comma number
  if (objective == 0) { //ALTITUDE
    GoalNumber = 9;//9th comma
  }
  else if (objective == 1) { //LATITUDE
    GoalNumber = 2;     //2nd comma
  }
  else if (objective == 2) { //LONGITUDE
    GoalNumber = 4;     //4th comma
  }
  else if (objective == 3) { //FIX
    GoalNumber = 6;     //6th comma
  }
  else if (objective == 4) { //SATILITE COUNT
    GoalNumber = 7;     //7th comma
  }
  int i;
  int t = 0;
  int Comma_Counter = 0;     //comma counter
  boolean Goal = false;      //True if the NMEA is reading the objective
  String two = "                   ";  //Temp string to capture wanted information
  for (i = 0; i < 120; i++) {
    if (NMEA_Sentence[i] == ',') { //Checks for a comma in the NMEA
      Comma_Counter++;
    }
    else if (Comma_Counter == GoalNumber) { //Once targetted comma is passed. Record until next comma
      if (NMEA_Sentence[i] != ',') {
        two[t] = NMEA_Sentence[i];
        t++;
      }
    }
  }
  char arr[20];
  for (i = 0; i < 20; i++) {
    arr[i] = two[i];
  }
  float temp = atof(arr);//Converts char array to float
  return temp;
}
