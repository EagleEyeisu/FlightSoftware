/****LIBRARIES****/
#include <SPI.h>
#include <RH_RF95.h>

/***RADIO COMMUNICATION****/
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LoRa_LED 13
#define RF95_FREQ 433.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup(){
  pinMode(LoRa_LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while(!Serial);
  Serial.begin(4800);
  
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
 
  if(!rf95.init()){
    Serial.println("LoRa radio init failed");
    while (1);
  }
  else{
     Serial.println("LoRa radio init OK!");
  }
  if(!rf95.setFrequency(RF95_FREQ)){
    Serial.println("setFrequency failed");
    while (1);
  }
  else{
    Serial.print("Freq set to: "); Serial.println(RF95_FREQ);
  }
  rf95.setTxPower(23, false);
  Serial.println("\n");
}
 
int16_t packetnum = 0;  // packet counter

void loop(){
  Serial.println(".");
  if(Serial.read() == 'R'){
    char radiopacket[10] = "READY?";
    Serial.print("Sending: "); Serial.println(radiopacket);
    rf95.send((uint8_t *)radiopacket, 20);
    rf95.waitPacketSent();
    digitalWrite(LoRa_LED, HIGH);
    delay(10);
    digitalWrite(LoRa_LED, LOW);
  }
  else if(rf95.available()){
    uint8_t buf[150];
    uint8_t len = sizeof(buf);
    if(rf95.recv(buf, &len)){ //Processes Response
      if(buf[1]=='G' && buf[2]=='P' && buf[3]=='G' && buf[4]=='G'){
        Serial.print("Recieved: ");
        Serial.println((char*)buf);
      }
    }
    else{
      Serial.println("Receive failed");
    }
  }
  delay(1000);
}
