# Purpose
	Holds all dependencies to run LORA's software.

	Upload LORA.ino file to LORA to run.

### Dependencies
	- Arduino.h
	- stdlib.h
	- SoftwareSerial.h
	- math.h
	- TinyGPSPlus.h
	- string.h
	- Wire.h
	- RH_RF95.h

## LORA.ino
	Arduino file uploaded to micro controller.

## Data.cpp
	Implements data handling. Parses packet received from ground station.

## GPS.cpp
	Updates GPS data onboard LoRa.

## I2C.cpp
	Sends and receives data between microcontroller and LoRa.

## Radio.cpp
	Sends and receives data between LoRa and ground station.