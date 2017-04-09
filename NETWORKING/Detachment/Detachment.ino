/****LIBRARIES****/
#include <SD.h>
#include <RH_RF95.h>
#include <Servo.h>

/****SYSTEM INFO****/
int Program_Cycle = 0;
boolean FirstStage = false;
boolean SecondStage = false;
boolean ThirdStage = false;

/****COMMUNICATION****/
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13
#define RF95_FREQ 433.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
boolean HandShakeGND = false;

/****SERVO****/
Servo detach_servo;

void setup() {
  delay(3000);
  Serial.begin(4800);

  /****Initialization of Servo****/
  detach_servo.attach(11);
  detach_servo.write(90);
  
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
  delay(1000);
  Receive();
  Start_Drop();
  Serial.println(".");
}

/*
 * Receives Data. 
 */
void Receive(){
  if(rf95.available()){
    uint8_t buf[20];
    uint8_t len = sizeof(buf);
    if(rf95.recv(buf, &len)){ //Processes Response
      if(buf[0]=='S' && buf[1]=='T' && buf[2]=='A' && buf[3]=='R' && buf[4]=='T'){
        Serial.println("Received: START from GND");
        HandShakeGND = true;
        Program_Cycle = 0;
      }
      else if(buf[0]=='O' && buf[1]=='K' && buf[2]=='H' && buf[3]=='A' && buf[4]=='B' && buf[5]=='E' && buf[6]=='T'){
        FirstStage=false;
        Serial.println("Received HandShake from EE");
      }
      else if(buf[0]=='D' && buf[1]=='R' && buf[2]=='O' && buf[3]=='P'){
        Serial.println("Received DROP FROM EE");
        SecondStage=true;
        Program_Cycle = 0;
      }
      else if(buf[0]=='E' && buf[1]=='N' && buf[2]=='D' && buf[3]=='H' && buf[4]=='A' && buf[5]=='B' && buf[6]=='E' && buf[7]=='T'){
        ThirdStage=false;
        Serial.println("FINAL Handshake Received from GND.");
      }
    }
  }
}

/*
 * Releases the EagleEyeCraft from HABET.
 */
void Release(){
  for(int i=90;i<160;i++){
    detach_servo.write(i);
    delay(10);
  }
  Serial.println("Detached");
}

/*
 * Handles sending.
 */
void Start_Drop(){
  if(HandShakeGND){//Sends first Handshake with GND. 
    Program_Cycle++;
    if(Program_Cycle>25){
      HandShakeGND = false;
      Serial.println("HandShake 1 with GND Timed out.");
      FirstStage = true;
      delay(5000);
      Program_Cycle = 0;
    }
    if(Program_Cycle%5==0){//Sends every 5 seconds.
      char Packet[10] = "OKGND";
      Serial.print("Sending: "); Serial.println(Packet);
      rf95.send(Packet, sizeof(Packet));
      rf95.waitPacketSent();
    }
  }
  else if(FirstStage){//SENDS SIGNAL TO EE TO ASK FOR DROP
    Program_Cycle++;
    if(Program_Cycle%5==0){
      char Packet[10] = "STARTEE";
      Serial.print("Sending: "); Serial.println(Packet);
      rf95.send(Packet, sizeof(Packet));
      rf95.waitPacketSent();
    }
  }
  else if(SecondStage){//SENDS HANDSHAKE WITH EE TO END DROP SIGNAL
    Program_Cycle++;
    if(Program_Cycle>25){
      SecondStage = false;
      Serial.println("Handshake with EE Timed Out.");
      ThirdStage=true;
      delay(5000);
      Program_Cycle = 0;
      Release();
    }
    if(Program_Cycle%5==0){
      char Packet[10] = "OKEE";
      Serial.print("Sending: "); Serial.println(Packet);
      rf95.send(Packet, sizeof(Packet));
      rf95.waitPacketSent();
    }
  }
  else if(ThirdStage){
    if(Program_Cycle%5==0){
      char Packet[10] = "OKEE";
      Serial.print("Sending: "); Serial.println(Packet);
      rf95.send(Packet, sizeof(Packet));
      rf95.waitPacketSent();
    }
  }
}

