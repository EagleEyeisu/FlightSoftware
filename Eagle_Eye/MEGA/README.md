# Purpose
	Holds all dependencies to run MEGA's software.

	Upload the MEGA.ino file to the microcontroller and run

#### Note on IO
     LoRa, IMU, and BMP communication occur over I2C. Exact pins connected do not matter, but must match up with clock/data port ordering according to MEGA pinouts.

     Temperature sensor pinouts are:
     		 CLK = D5
		 CS = D4
		 D0 = D3

### Dependencies:
	- Ardunio.h
	- Adafruit_BMP085_U.h
	- Adafruit_MAX31855.h
	- Adafruit_LSM9DS0.h
	- Adafruit_Simple_AHRS.h
	- Adafruit_Sensor.h
	- Wire.h
	- Servo.h


## MEGA.ino
	Arduino file uploaded to micro controller.

## DATA.cpp
	Implements data handling. Updates data when associated function called. Parses network packets sent between LoRa and the microcontroller. Calculates altitude from barometric pressure data.

## I2C.cpp
	Handles communication between LoRa and the microcontroller. Builds packet from data on board microcontroller and sends to LoRa using i2c protocol.

## IMU.cpp
	Handles roll, pitch, and yaw data values. Uses values to calculate heading and bearing, distance from target, and altitude difference between craft and target.

## MOTOR.cpp
	Implements handling of motor and servo. Craft works based on simple state machine: 

	[No Movement] -> [Yaw Correction] -> [Forward/Backward] -> [Altitude Correction] 

## THERMO.cpp
	Handles thermometer data