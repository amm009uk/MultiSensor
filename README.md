ESP8266 All-in-One for controlling several sensors :: Motion (1), Reed (4) and DHT (1)

# MultiSensor

### This project assumes you know what an esp8266 device is and how to upload code to it.

-------------------------------------------------------------------------------------------------------------
## Version
1.0 Initial testing completed

-------------------------------------------------------------------------------------------------------------
## Features
This firmware is designed to control up to 6 sensors:
- One motion
- One temperature
- Up to four reed switches (door/window type)

You don't need all the sensors. Each sensor may be disabled/enabled through MultiSensor\src\User.h

Each sensor sends its own MQTT message.

-------------------------------------------------------------------------------------------------------------
## Pin Connections 
Project was developed on a Wemos D1 mini pro. Sensors were hooked up as follows:
- D4 Pin for Temperature sensor
- D2 Pin for motion sensor
- D1 Pin for door/window sensor 1
- D5 Pin for door/window sensor 2
- D6 Pin for door/window sensor 3
- D7 Pin for door/window sensor 4

-------------------------------------------------------------------------------------------------------------
## Setup device
1. Flash SPIFFs to upload MultiSensor/data/*.json. You may modify contents but not necessary.

2. To enable/disable serial debug output, uncomment/comment first line in sonoff/src/User.h.

3. Flash firmware.

All future updates can now be performed over the air no need for USB assuming above was successful.

-------------------------------------------------------------------------------------------------------------
## Usage
1. Device will initially come up with its own *Access Point* called ITEAD-xxxxxxx. Connect to this and configure WiFi parameters. Once saved, device will reboot.

2. On bootup, device will connect to your WiFi. Find its IP address through your router and connect to it. Configure all parameters and once saved, device will reboot.

3. All sensors will send data as appropriate.

- An alternative method for finding your device is to scan your mDNS network

-------------------------------------------------------------------------------------------------------------
## OTA Updates
Once device is connected to your WiFi, find its IP and connect to it. User/Password are stored in sonoff/src/User.h so you can always modify and flash new firmware to change it.

-------------------------------------------------------------------------------------------------------------
- I am simply reusing other peoples amazing work for instance the following libraries PubSubClient and WifiManager.

- My development environment is Atom with its builtin PlatformIO toolset. Its a fantastic build and debug environment.