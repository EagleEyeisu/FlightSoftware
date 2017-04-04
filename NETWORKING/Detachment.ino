#include <SPI.h>
#include <RH_RF95.h>

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LoRa_LED 13

int fail = 0;

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
void setup(){
  pinMode(LoRa_LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while(!Serial);
  Serial.begin(9600);
  
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  if(!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  else{
     Serial.println("LoRa radio init OK!");
  }
 
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  else{
    Serial.print("Freq set to: "); Serial.println(RF95_FREQ);
  }
  rf95.setTxPower(23, false);
  Serial.println();
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
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if(rf95.waitAvailableTimeout(10000)){ //Waits for response or timesout
      if(rf95.recv(buf, &len)){ //Processes Response
        Serial.print("Recieved: ");
        Serial.println((char*)buf);
        Serial.println(fail);
      }
      else{
        fail++;
        Serial.println("Receive failed");
        Serial.println(fail);
      }
    }
    else{
      fail++;
      Serial.println("Receive failed");
      Serial.println(fail);
    }
    Serial.println();
  }
  else if(rf95.available()){
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if(rf95.recv(buf, &len)){ //Processes Response
      Serial.print("Recieved: ");
      Serial.println((char*)buf);
      Serial.println(fail);
    }
    else{
      fail++;
      Serial.println("Receive failed");
      Serial.println(fail);
    }
  }
  delay(1000);
}
