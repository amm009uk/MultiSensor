ESP8266 All-in-One for controlling several sensors :: (1) DHT, (1) Motion and (4) Reeds

# MultiSensor

### This project assumes you know what an esp8266 device is and how to upload code to it

-------------------------------------------------------------------------------------------------------------
## Features
Firmware is designed to control up to 6 sensors:  
	- One motion  
	- One temperature  
	- Up to four reed switches (door/window type)  

Each sensor may be enabled/disabled through MultiSensor\src\User.h

Each sensor sends its own MQTT status message - so you must have a Mosquitto Broker running

Temperature is actually "heat index" and is calculated by taking temperature and humidity into account

-------------------------------------------------------------------------------------------------------------
## Version
1.0 Initial version  
1.1 Testing completed  
1.2 Updated README  
1.3 Added IP lookup  
1.4 Fixed PIN mappings  (motion was hooked up to D4/builtin LED)  
1.5 Added over the air serial output with Telnet (RemoteDebug library)  

-------------------------------------------------------------------------------------------------------------
## Device Setup
1. Flash SPIFFs to upload the configuration files - MultiSensor/data/config.json. You may modify the contents prior to upload but not necessary

2. Flash firmware

3. Device will initially come up with its own *Access Point* called esp82XX-xxxxxxx. Connect to this and configure WiFi parameters. Once saved, device will reboot and connect to your WiFi  
   See section **Finding device IP Address**

4. Once device is connected to WiFi, connect to it using a Browser. User/Password are stored in MultiSensor/src/User.h

5. Configure device parameters on web page and save settings. Once saved, device will reboot and reconnect to your WiFi and MQTT Broker

6. All sensors will now send their own MQTT message as appropriate

- Above steps should be done over USB-->Serial interface until device is fully functioning  
- Future firmware updates can be performed over the air with no need for USB-->Serial interface

7. Test all sensors and once ok, turn off debugging and upload new compiled firmware  
   See section **Debug - Serial/Telnet output**

-------------------------------------------------------------------------------------------------------------
## Finding device IP Address
	To get the device IP address you have the following options:
	1. Look at the Serial output where it will show on startup (assuming you have debug output turned on)
	2. Look in your router to see WiFi clients
	3. Try an mDNS browser app but this often takes time to get the device showing up

	4. If already connected to WiFi and MQTT Broker, you can send a blank MQTT message as defined in user.h at "IP_REQUEST".  
     Each device will respond with a MQTT message such as defined with "IP/REPLY/<deviceID>" with the IP address in the payload.

-------------------------------------------------------------------------------------------------------------
## Debug - Serial/Telnet output
	You have two options after turning on SERIAL_DEBUG within MultiSensor\src\User.h:
		- Serial output over USB if connected
		- Telnet if connected

**Do not leave SERIAL_DEBUG enabled for normal use**

-------------------------------------------------------------------------------------------------------------
## OTA Firmware Updates
Once device is connected to your WiFi, find its IP and connect to it through using a Browser  
User/Password are stored in MultiSensor/src/User.h and you can always modify and flash new firmware to change it  
Follow on screen firmware update instructions to flash new firmware

-------------------------------------------------------------------------------------------------------------
## Pin Connections 
Project was developed on a **Wemos D1 mini pro** board. I like this boards form-factor and its ease of use. 

Sensors can be hooked up as follows:
- D1 Pin for Temperature sensor (DHT22)
- D2 Pin for Motion sensor (HC-SR501)
- D5 Pin for door/window sensor 1 (Reed)
- D6 Pin for door/window sensor 2
- D7 Pin for door/window sensor 3
- D8 Pin for door/window sensor 4

These can be reconfigured within MultiSensor\src\User.h.

-------------------------------------------------------------------------------------------------------------
## Credits
I am simply reusing other peoples amazing work for instance the following libraries:  
	- [PubSubClient](https://github.com/knolleary/pubsubclient)  
	- [WifiManager](https://github.com/tzapu/WiFiManager)  
	- [RemoteDebug](https://github.com/JoaoLopesF/RemoteDebug)  
	- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)  
	- [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)  
	- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)  

AND OF COURSE the many examples on github

My development environment is Atom with its builtin PlatformIO toolset. Its a fantastic build and debug environment.