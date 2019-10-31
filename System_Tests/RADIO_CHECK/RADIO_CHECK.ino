#include <RH_RF95.h>


// Chip select pin for the radio.
const byte RFM95_CS = 8;
// Intialization pin for radio.
const byte RFM95_INT = 7;
// Reset pin onboard the radio.
const byte RFM95_RST = 4;
// Radio frequency used throught the Eagle Eye Program. CHECK WITH HABET BEFORE EACH FLIGHT!!!!!
#define RF95_FREQ 433.0
// Directs the radio object to focus on two specific ports.
RH_RF95 rf95(Radio.RFM95_CS, Radio.RFM95_INT);
// Used in the computation of the radio system. 
unsigned long broadcast_timer = 0;


void setup()
{
    // Assigns pin 4 to have an output singal connection to the LoRa's radio port.
    pinMode(RFM95_RST, OUTPUT);
    // Sends a high signal to the radio port for intialization.
    digitalWrite(RFM95_RST, HIGH);
    // Adjust the LED to be insync with radio trasmission.
    digitalWrite(RFM95_RST, LOW);
    // 10 millisecond delay to allow for radio setup to complete before next instruction.
    delay(10);
    // Turns the radio output high to compelte setup.
    digitalWrite(RFM95_RST, HIGH);
    // Checks for the creation of the radio object and its physical connection attribute.
    if(!rf95.init())
    {
        // If invalid connection, the program will stall and pulse the onbaord led.
        while (1)
        {
            ;
        }
    }
    // Checks the radio objects tuned frequency. 
    if(!rf95.setFrequency(RF95_FREQ))
    {
        // If invalid connection, the program will stall and pulse the onbaord led.
        while (1)
        {
            ;
        }
    }
    // Sets the max power to be used to in the amplification of the signal being sent out.
    rf95.setTxPower(23, false);
}


void loop()
{
    // Reads in radio transmission if available.
    radio_receive();
    // Each of the crafts have # seconds to broadcast.
    if(millis() - broadcast_timer > 500)
    {
        // Resets the counter. This disables broadcasting again until # seconds have passed.
        broadcast_timer = millis();
        // What will be sent. 
        String packet = "Hello There";
        // Sends the transmission via radio.
        broadcast(packet);
    }
}


/**
 * Responsible for sending out messages via the radio antenna.
 */
void broadcast(String packet)
{
    // Converts from String to char array.
    char transmission[packet.length()+1];
    packet.toCharArray(transmission, packet.length()+1);
    // Sends message passed in as paramter via antenna.
    rf95.send((const uint8_t*)transmission, sizeof(transmission));
    // Pauses all operations until the micro controll has guaranteed the transmission of the
    // signal.
    rf95.waitPacketSent();
}

/**
 * Responsible for reading in signals over the radio antenna.
 */
void radio_receive()
{
    // Checks if radio message has been received.
    if (rf95.available())
    {
      	// Creates a temporary varaible to read in the incoming transmission.
      	uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      	// Gets the length of the above temporary varaible.
      	uint8_t len = sizeof(buf);
        // Reads in the avaiable radio transmission, then checks if it is corrupt or complete.
        if(rf95.recv(buf, &len))
        {
            // Used to display the received data in the GUI.
            radio_input = (char*)buf;
            // Conversion from uint8_t to string. The purpose of this is to be able to convert to an
            // unsigned char array for parsing.
            String str = (char*)buf;
            char to_parse[str.length()];
            str.toCharArray(to_parse,str.length());
            // Debugging to the Serial Monitor.
            Serial.print("Radio In: ");
            Serial.println(radio_input);
        }
	  }
}
