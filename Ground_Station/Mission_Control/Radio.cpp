/**
   Radio.ccp holds all functions related the radio port/module infused inside the LoRa FeatherWing
   development micro controller.
*/

#include <Arduino.h>
#include "Radio.h"
#include "Data.h"
#include <RH_RF95.h>
#include "Globals.h"


/**
   Constructor used to reference all other variables & functions.
*/
RADIO::RADIO()
{

}


/**
   Parses and returns the radio transmission's Time Stamp (ms).
      craft         -> 1
      mission_control -> 6
*/
float RADIO::get_radio_timestamp(String selector)
{
  if (selector == "craft")
  {
    return (Data.Parse(1));
  }
  else if (selector == "mc")
  {
    return (Data.Parse(6));
  }

}


/**
 * Returns the packet's roll data.
 */
float RADIO::get_radio_roll()
{
    return (Data.Parse(2));
}


/**
 * Returns the packet's pitch data.
 */
float RADIO::get_radio_pitch()
{
    return (Data.Parse(3));
}


/**
 * Returns the packet's yaw data.
 */
float RADIO::get_radio_yaw()
{
    return (Data.Parse(4));
}


/**
 * Returns the packet's craft_state data.
 */
float RADIO::get_radio_craft_state()
{
    return (Data.Parse(5));
}


/**
 * Parses and returns the radio transmission's manual direction command.
 */
float RADIO::get_radio_manual_direction()
{
    return (Data.Parse(7));
}


/**
 * Parses and returns the radio transmission's anchor variable.
 * 0.0 -> pause
 * 1.0 -> running
 */
float RADIO::get_radio_craft_anchor()
{
    return (Data.Parse(8));
}


/**
 * Parses and returns the radio transmission's target throttle variable.
 */
float RADIO::get_radio_target_throttle()
{
    return (Data.Parse(9));
}


/**
   Assigns correct pins to the radio output port. Tests connections and variables.
*/
void RADIO::initialize()
{
  // Assigns pin to have an output singal connection to the LoRa's radio port.
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
  if (!rf95.init())
  {
    // If invalid connection, the program will stall and pulse the onbaord led.
    while (1)
    {
       Serial.println("Radio: Error Init");
    }
  }
  // Checks the radio objects tuned frequency.
  if (!rf95.setFrequency(RF95_FREQ))
  {
    // If invalid connection, the program will stall and pulse the onbaord led.
    while (1)
    {
      Serial.println("Radio: Error Frequency");
    }
  }
  // Sets the max power to be used to in the amplification of the signal being sent out.
  rf95.setTxPower(23, false);
}


/**
   Manages all radio comms either incoming or outgoing.
*/
void RADIO::manager()
{
  // Reads in radio transmission if available.
  radio_receive();
  // Each of the crafts have # seconds to broadcast. That means each craft will broadcast every # seconds.
  if (millis() - broadcast_timer >= network_node_delay)
  {
    // Resets the counter. This disables broadcasting again until 10 seconds has passed.
    broadcast_timer = millis();
    String packet = "";
    // Normal network operation.
    packet = construct_network_packet();
    broadcast(packet);
  }
}


/**
   Constructs the normal network packet.
*/
String RADIO::construct_network_packet()
{
  // Updates the time object to hold the most current operation time.
  mission_control_ts = millis() / 1000.0;
  // Casting all float values to a character array with commas saved in between values
  // so the character array can be parsed when received by another craft.
  String temp = "";
  temp += "$";
  temp += ",";
  temp += craft_ts;
  temp += ",";
  temp += roll;
  temp += ",";
  temp += pitch;
  temp += ",";
  temp += yaw;
  temp += ",";
  temp += craft_state;
  temp += ",";
  temp += mission_control_ts;
  temp += ",";
  temp += manual_direction;
  temp += ",";
  temp += anchor_status;
  temp += ",";
  temp += target_throttle;
  temp += ",";
  temp += "$";
  radio_output = "";
  // Copy contents.
  radio_output = temp;
  return temp;
}


/**
   Creates an array to be sent out via Radio. Fills that array with correct values and returns it.
*/
void RADIO::broadcast(String packet)
{
  Serial.print("Radio O: ");
  Serial.println(radio_output);
  // Converts from String to char array.
  char transmission[packet.length() + 1];
  packet.toCharArray(transmission, packet.length() + 1);
  // Sends message passed in as paramter via antenna.
  rf95.send((const uint8_t*)transmission, sizeof(transmission));
  // Pauses all operations until the micro controll has guaranteed the transmission of the
  // signal.
  rf95.waitPacketSent();
}


/**
   Responsible for reading in signals over the radio antenna.
*/
void RADIO::radio_receive()
{
  // Checks if radio message has been received.
  if (rf95.available())
  {
    // Creates a temporary varaible to read in the incoming transmission.
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    // Gets the length of the above temporary varaible.
    uint8_t len = sizeof(buf);
    // Reads in the avaiable radio transmission.
    if (rf95.recv(buf, &len))
    {
      // Used to display the received data in the GUI.
      radio_input = (char*)buf;
      Serial.print("Radio I: "); Serial.println(radio_input);
      // Conversion from uint8_t to string. The purpose of this is to be able to convert to an
      // unsigned char array for parsing.
      String str = (char*)buf;
      char to_parse[str.length()];
      str.toCharArray(to_parse, str.length());
      // Checks for a valid packet. Only parses contents if the packet valid to prevent
      // data corruption.
      if (validate_checksum())
      {
        // Reads in the time stamp for craft's last broadcast.
        float temp_ts = get_radio_timestamp("craft");
        // Compares the currently brought in time stamp to the one stored onboad.
        if (temp_ts > craft_ts)
        {
          // If the incoming signal has more up-to-date versions, we overwrite our saved version with
          // the new ones.
          craft_ts = temp_ts;
          roll = get_radio_roll();
          pitch = get_radio_pitch();
          yaw = get_radio_yaw();
          craft_state = get_radio_craft_state();
        }
        // Pulls the RSSI from the signal. (Received Signal Strength Indicator)
        received_rssi = rf95.lastRssi();
      }
    }
  }
}


/**
   Responsible for ensuring that a full packet has been received
   by validating that the packet begins and ends with the correct
   symbol '$'.
*/
bool RADIO::validate_checksum()
{
  // Gets the length of the packet. Non-zero indexed.
  int str_length = radio_input.length();
  // Checks for the correct starting and ending symbols.
  if ((radio_input.charAt(0) == '$') && (radio_input.charAt(str_length - 1) == '$'))
  {
    // If both are detected, valid packet.
    return true;
  }
  else
  {
    // Otherwise, invalid packet. Prevents the system from
    // attempting to parse its contents.
    return false;
  }
}
