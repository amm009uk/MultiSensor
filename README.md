ESP8266 All-in-One for controlling several sensors :: (1) Motion, (4) Reeds and (1) DHT

# MultiSensor

### This project assumes you know what an esp8266 device is and how to upload code to it

-------------------------------------------------------------------------------------------------------------
## Version
1.0 Initial testing completed  
1.2 Updated README  
1.4 Fixed PIN mappings  
1.5 Added over the air debug (RemoteDebug library)  

-------------------------------------------------------------------------------------------------------------
## Features
Firmware is designed to control up to 6 sensors:  
	- One motion  
	- One temperature  
	- Up to four reed switches (door/window type)  

You don't need all the sensors. Each sensor may be enabled/disabled through MultiSensor\src\User.h

Each sensor sends its own MQTT message status - so you must have a Mosquitto Broker running

Temperature is actually "heat index" and is calculated by taking temperature and humidity in account

-------------------------------------------------------------------------------------------------------------
## Setup device
1. Flash SPIFFs to upload the configuration file - MultiSensor/data/config.json. You may modify the contents prior to upload but not necessary.

2. Flash firmware.

3. Device will initially come up with its own *Access Point* called esp8266-xxxxxxx. Connect to this and configure WiFi parameters. Once saved, device will reboot and connect to your WiFi.  
   See section **Finding device IP Addres**

4. Once device is connected to WiFi, connect to it using browser. 

5. Configure device parameters on web page and save.  
   Once saved, device will reboot and reconnect to your WiFi.

6. All sensors will send their own MQTT message as appropriate.

7. Test all sensors and once ok, turn off debugging and upload new compiled firmware - see section **Debug - Serial/Telnet output**.

- Above steps above should be done over USB-->Serial interface until device is fully functioning.

- Future firmware updates can be performed over the air no need for USB-->Serial interface.

-------------------------------------------------------------------------------------------------------------
## Finding device IP Address
	To get the device IP address you have the following options:
	1. Look at the Serial output where it will show on startup (assuming you have debug output turned on)
	2. Look in your router
	3. Try an mDNS browser app but this often takes time to get the ESP showing up

	4. If already connected to WiFi and MQTT Broker, you can send a blank MQTT message as defined in user.h at "IP_REQUEST".  
     The device will respond with another MQTT message such as IP/Reply/<unique ID> with the IP address in the payload.

-------------------------------------------------------------------------------------------------------------
## Debug - Serial/Telnet output
	You have two options after turning on SERIAL_DEBUG within MultiSensor\src\User.h:
		- Serial USB if connected
		- Telnet if connected
	
	***** Do not leave SERIAL_DEBUG enabled for normal use *****

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

These can be reconfiured within MultiSensor\src\User.h.

-------------------------------------------------------------------------------------------------------------
## OTA Firmware Updates
Once device is connected to your WiFi, find its IP and connect to it. User/Password are stored in Multiensor/src/User.h so you can always modify and flash new firmware to change it.

-------------------------------------------------------------------------------------------------------------
- I am simply reusing other peoples amazing work for instance the following libraries PubSubClient, WifiManager, RemoteDebug etc.

- My development environment is Atom with its builtin PlatformIO toolset. Its a fantastic build and debug environment.