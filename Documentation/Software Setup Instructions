# Software Setup Instructions

Author: Jared Danner / (319-231-2412) / jareddanner3@gmail.com

Purpose : Directions for installing the software and dependencies needed to run the Eagle Eye software, 

## Prerequisites

1) Create a GitHub account: [https://github.com/](https://github.com/) 
2) Download Git: [https://git-scm.com/downloads](https://git-scm.com/downloads)
		a) Use default installation settings. 
3) Repository Owner (Jared Witt) has to grant you access to commit to the repository.
4) Download Arduino: [https://www.arduino.cc/en/Main/Software](https://www.arduino.cc/en/Main/Software)
			(Arduino will be used to interact with the craft's software and flash it to the micro controllers.)

## GitHub
1) Go to Eagle Eye's Repo: [https://github.com/EagleEyeisu/FlightSoftware](https://github.com/EagleEyeisu/FlightSoftware)
2) Clone down the repository by clicking the green "Clone or download" button.
	a) Copy this URL.
	b) Open "Windows Powershell" (windows) or "Terminal" (Linux, but if you are using Linux you won't need this tutorial which is ironic because you won't read this either)
	c) Navigate to a place where you want this repo to be saved. Direct questions here: [https://www.google.com/](https://www.google.com/)
	c) Type "git clone" into your terminal window. 
	d) Paste the url after "git clone". Hit Enter. The repo will begin to clone down.

## Opening the software
The beginner Integrated Development Environment (IDE) for Arduino micro controllers is well Arduino. Think of this as an advanced notepad++ with the ability to load code directly to the micro controller and talk back and forth via a UART(serial (RX/TX)) line. 
1) Open the Arduino Editor.
2) File -> Open -> Navigate to where you saved the repo, enter the "Craft" folder. Click on the Craft.ino file (The one with the blue circle).

 Arduino should now show all of the associated files along the top of the Editor. the "Main" file is the "Craft.ino". Like any good software project, we try to isolate and organize our code by using classes (different files for different parts of the software). The program starts execution at the beginning of void setup() and cycles through void loop() until either shutdown or it hits an error. This is not like your previous experience is python or matlab where it runs once and finishes at the bottom. In embedded software, the goal of the program is for it to never "end" and to always be performing a function (taking gps samples, using the radio, or sleeping until its next assignment, but never finishing).

3) Near the top left of the editor will be 2 buttons "verify" and "upload". "Verify" will simply run through your code and check for syntax and compile errors. It will report errors found in the terminal below. "Upload" does what "verify" does and assuming it compiles, it will flash it over to the micro controller. All of the software that goes on micro controllers is written in a very nice variant of C++ called .....Arduino, see a theme yet? Just refer to it as C++. 

## Using the C
When you are ready to test something out on hardware, (you probably aren't as you're about to find out) you'll need to use POWER AND DATA micro-usb cable (blue tape coords) to connect to what ever micro controller you want. Arduino MEGA, Uno, Feather M0, etc.. Eagle Eye uses the Adafruit Feather M0 (think of this as a very powerful Arduino) to power the craft and the ground station micro controller which interacts with the python GUI. 
1) To upload to the Feather M0, you'll need to download a package into Arduino that will tell the IDE how to talk to the Adafruit product. Follow the instructions at: [https://learn.adafruit.com/adafruit-feather-m0-basic-proto/setup](https://learn.adafruit.com/adafruit-feather-m0-basic-proto/setup) (Most of this is extra info, just follow the pictures along with the red url).
2) Again follow the picture instructions at: [https://learn.adafruit.com/adafruit-feather-m0-basic-proto/using-with-arduino-ide](https://learn.adafruit.com/adafruit-feather-m0-basic-proto/using-with-arduino-ide) to install the package you just downloaded. (Ignore everything after the "Install Adafruit SAMD" section)
3. You should now be ready to upload your sketches to the Feather M0. Go ahead and upload.
4. Congrats it failed. Learning moment! At the top of Craft.ino, you probably noticed there are a few libraries surrounded by <>. These are software libraries that we used in our project to not have to reinvent the wheel. (The ones surrounded by "" are Eagle Eye's libraries that we specifically made). You'll have to download these libraries individually to be able to correctly upload the sketch to the Feather. In the future, if want to do something, look up a library for it on Github before you do it yourself. There is no work like teamwork in software. Reuse as much as possible. 
5. For the following links, click the green button but download the files via the "Download Zip" option. Unzip the folder. The inner folder is in the same folder as unzipped so pull out of the actual folder and place it in your libraries folder of your Arduino installation. Google if unsure.
	a) RH_RF95 (Radio Head) @ [https://github.com/PaulStoffregen/RadioHead](https://github.com/PaulStoffregen/RadioHead)
	b) TinyGPSPlus @ [https://github.com/mikalhart/TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus) This one is a bit odd. Once placed in the libraries folder, open the /src/ folder. Change the names from TinyGPS++ to TinyGPSPlus for both the header and cpp files. Arduino complains about having the + at the end of the name. (Also change line 24 (the #include) of the .cpp file)
	c) Adafruit_MAX31855 @ [https://learn.adafruit.com/thermocouple/arduino-code](https://learn.adafruit.com/thermocouple/arduino-code) Follow the section related to MAX31855.
	d) Adafruit_LSM9DS0 @ [https://github.com/adafruit/Adafruit_LSM9DS0_Library](https://github.com/adafruit/Adafruit_LSM9DS0_Library)
	e) Adafruit_Sensor.h @ [https://github.com/adafruit/Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor)
Arduino.h is built in and allows access to the various Arduino functions you'll come to know and hate. 

6. You will now be able to upload the software to the Feather M0 correctly. It won't appear to run unless you've hooked up every component but it will upload.
	
