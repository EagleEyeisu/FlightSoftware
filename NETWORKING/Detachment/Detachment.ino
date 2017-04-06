/****LIBRARIES****/
#include <SD.h>
#include <RH_RF95.h>
#include <Servo.h>

/****COMMUNICATION****/
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13
#define RF95_FREQ 433.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
boolean Drop_Sequence = false;
boolean HandShake_GND = false;
boolean Complete_Drop = false;
boolean Sent = false;

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
      if(buf[0]=='S' && buf[1]=='T' && buf[2]=='A' && buf[4]=='T'){
        Serial.println("Receved start");
        Drop_Sequence = true;
      }
      else if(buf[0]=='D' && buf[1]=='R' && buf[2]=='O' && buf[3]=='P'){
        Serial.println("Receved drop");
        Release();
        Drop_Sequence = false;
        Complete_Drop = true;
      }
      else if(buf[0]=='C' && buf[2]=='M' && buf[4]=='L' && buf[7]=='E'){
        Serial.println("Receved complete");
        Complete_Drop = false;
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
  if(Drop_Sequence && !Sent){
    Sent = true;
    char Packet[10] = "READY?";
    Serial.print("Sending: "); Serial.println(Packet);
    rf95.send(Packet, sizeof(Packet));
    rf95.waitPacketSent();
  }
  else if(Complete_Drop){
    char Packet[10] = "END";
    Serial.print("Sending: "); Serial.println(Packet);
    rf95.send(Packet, sizeof(Packet));
    rf95.waitPacketSent();
  }
}

