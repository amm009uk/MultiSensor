#include <FS.h>                                                      // SPIFFS support
#include <ESP8266WiFi.h>                                             // ESP8266 Core WiFi Library
#include <ESP8266WebServer.h>                                        // Local WebServer used to serve the configuration portal
#include <ESP8266HTTPUpdateServer.h>                                 // OTA Updater: http://<ip>
#include <PubSubClient.h>                                            // MQTT Client Publish and Subscribe
#include <DNSServer.h>                                               // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266mDNS.h>                                             // Include the mDNS library
#include <WiFiManager.h>                                             // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoJson.h>                                             // Read, write JSON format
#include <DHT.h>                                                     // DHT sensor library by Adafruit
#include <Functions.h>                                               // Our functions
#include <RemoteDebug.h>                                             // Remote debug over telnet   
#include <User.h>                                                    // Custom settings

/*----------------------------------------- Global variables -----------------------------------------*/

const String version = "1.5";                                        // Master version control

WiFiClient WiFiClient;                                               // The WiFi client object
PubSubClient MQTTclient(WiFiClient);                                 // MQTT client object
MDNSResponder mDNS;                                                  // Multi-Cast DNS object
RemoteDebug Debug;                                                   // Debug messages over WiFi using Telnet

char deviceID[30];                                                   // User specified name stored in configuration file
int rebootAt;                                                        // How many mins to wait before auto-reboot

/* MQTT Settings */
char mqtt_server[40];
int  mqtt_port;
char mqtt_user[11];
char mqtt_password[11];
char mqtt_tempTopic[40];
char mqtt_motionTopic[40];
char mqtt_reed1Topic[40];
char mqtt_reed2Topic[40];
char mqtt_reed3Topic[40];
char mqtt_reed4Topic[40];
char mqtt_inTopic[40];
long lastReconnectAttempt = 0;

/* Web Server */
ESP8266WebServer        httpServer(80);                              // WebServer on port 80
ESP8266HTTPUpdateServer httpUpdater;                                 // OTA updates
String                  INDEX_HTML;                                  // To hold web page

/* SIFFS Config file */
bool    spiffsActive = false;
String  getContentType(String filename);                             // convert the file extension to the MIME type
bool    handleFileRead(String path);                                 // send the right file to the client (if it exists)

/* Temperature variables */
int         tempPolling;                                             // Delay in secs for sleep timer (from config file)
#ifdef TEMP_SENSOR
char*       temp_state    = (char*) "UNDEFINED";
long        lastMsg       = 0;
float       temp_c;
float       humidity;
//static char celsiusTemp[7];
//static char fahrenheitTemp[7];
//static char humidityTemp[7];
DHT dht(DHT_PIN, DHTTYPE);                                           // Initialize DHT sensor
#endif

/* Motion sensor variables */
#ifdef MOTION_SENSOR
String motion_state = "UNDEFINED";
String last_motion_state = "";
#endif

/* Reed sensor variables */
#ifdef REED_SENSOR1
String reed_state1 = "UNDEFINED";
String last_reed_state1 = "";
#endif

#ifdef REED_SENSOR2
String reed_state2 = "UNDEFINED";
String last_reed_state2 = "";
#endif

#ifdef REED_SENSOR3
String reed_state3 = "UNDEFINED";
String last_reed_state3 = "";
#endif

#ifdef REED_SENSOR4
String reed_state4 = "UNDEFINED";
String last_reed_state4 = "";
#endif

long now;                                                            // Hold current time

/* ------------------------------------------ Start of code ------------------------------------------*/

void setup() {

#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  delay(100);
#endif

  //
  // RemoteDebug library setup
  //
#ifdef SERIAL_DEBUG
  Debug.setSerialEnabled(true);                                      // Output over serial as well
  Debug.setResetCmdEnabled(true);                                    // Enable the reset command
  rdebugAln("******************** Setup() Begin ********************");
#endif

  //
  // Get chip's Unique ID (used for WiFi Access Point name)
	//
	char chipID[25];                                                   
  uint32_t chipid = ESP.getChipId();                                 // Unique'ish chip ID found thru API
	sprintf(chipID, "ESP-%08X", chipid);
#ifdef SERIAL_DEBUG
  rdebugAln("Chip ID: %s", chipID);
#endif

  if (SPIFFS.begin()) {                                              // Start filesystem
#ifdef SERIAL_DEBUG
    rdebugAln("File system mounted successfully");
#endif
    spiffsActive = true;
  } else {
#ifdef SERIAL_DEBUG
    rdebugAln("File system failed to mount");
#endif
    return;
  }

  char* result;                                                      // Result from read/write of config file

  result = loadConfig();                                             // Load configuration file
  if (strcmp(result, "OK") != 0) {
#ifdef SERIAL_DEBUG
  	rdebugAln("..result of loadConfig() %s", result);
#endif
  	return;
  }

  WiFi.hostname(deviceID);                                           // Set hostname in DNS
	
#ifdef SERIAL_DEBUG
  rdebugAln("WiFiManager starting...");
#endif
  WiFiManager wifiManager;                                           // Initialise WiFiManager and it will do all the work of managing the WiFi
//  wifiManager.resetSettings();                                     // Wipes out previously saved WiFi settings
  wifiManager.setTimeout(300);                                       // Set timeout (secs) for completing WiFi configuration

//  if(!wifiManager.autoConnect(chipID, WWW_PASSWD)) {                 // Fetch SSID and Password from EEPROM and try to connect
  if(!wifiManager.autoConnect(chipID)) {                             // Fetch SSID and Password from EEPROM and try to connect
#ifdef SERIAL_DEBUG
    rdebugAln("Timed out connecting to Access Point");             // If it doesn't connect start an access point and go into
#endif
    delay(5000);                                                     // a blocking loop waiting for configuration or timeout
    ESP.restart();                                                   // Restart and try again
  }

  if (!mDNS.begin(deviceID, WiFi.localIP())) {                       // Start the mDNS responder for <deviceID>.local
#ifdef SERIAL_DEBUG
    rdebugAln("..Error setting up MDNS responder!");
#endif
    return;
  }

  mDNS.addService("ESP8266", "tcp", 80);                             // Advertise HTTP config page
  mDNS.addService("telnet", "tcp", 23);                              // Advertise Telnet
  
  Debug.begin(deviceID);                                             // Start the Remote Debug

  delay(10);

  WiFiStatus();                                                      // Now connected to WiFi so print out info

  // Setup http firmware update page
  httpUpdater.setup(&httpServer, UPDATE_PATH, WWW_USER, WWW_PASSWD);

	// Setup URL handlers
  httpServer.on("/", HTTP_GET, handleRoot);
  httpServer.on("/", HTTP_POST, handleRoot);
  httpServer.on("/saveChanges", HTTP_POST, saveChanges);
  httpServer.onNotFound(handleNotFound);

  httpServer.begin();                                                // Start Web Server

  delay(10);

  MQTTclient.setServer(mqtt_server, mqtt_port);                      // Initialse MQTT client
  MQTTclient.setCallback(callback);                                  // Callback service for receiving MQTT messages
  lastReconnectAttempt = 0;

  delay(10);

  // Initialise PIN's
  pinMode(DHT_PIN, INPUT);
  pinMode(MOTION_PIN, INPUT);
  pinMode(REED_PIN1, INPUT);
  pinMode(REED_PIN2, INPUT);
  pinMode(REED_PIN3, INPUT);
  pinMode(REED_PIN4, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

	digitalWrite(BUILTIN_LED, HIGH);
  
  delay(10);

#ifdef TEMP_SENSOR
  dht.begin();                                                       // Start the DHT sensor
#endif

#ifdef SERIAL_DEBUG
  rdebugAln("******************** Setup() Finish ********************");
#endif

} // setup()

void WiFiStatus() {

#ifdef SERIAL_DEBUG
  rdebugAln("WiFi SSID:    %s", WiFi.SSID().c_str());
  rdebugAln("mDNS started: %s%s", deviceID, ".local");
	rdebugAln("IP address:   %s", WiFi.localIP().toString().c_str());
  rdebugAln("MAC address:  %s", WiFi.macAddress().c_str());
	rdebugAln("Signal (dBm): %d", WiFi.RSSI());
#endif

  delay(10);

} // WiFiStatus()

void reboot() {

  delay(500);
//  ESP.reset();
  ESP.restart();

} //reboot()


void loop() {

  //
  // Non-blocking reconnect to MQTT if WiFi is connected
  // This allows other parts of the loop to run whilst no MQTT connection
  // If theres no WiFi that will be handled by WiFiManager in blocking mode
  //
  if ((WiFi.status() == WL_CONNECTED) && !MQTTclient.connected()) {
    now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect MQTT
      if (MQTTconnect()) {
        // Client connected
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // MQTT Client connected so check for MQTT activity
    MQTTclient.loop();
    delay(100);
  }

	//
	// Give time to Web Server
	//
  httpServer.handleClient();

  if (MQTTclient.connected()) {                                      // Check sensors as long as we have a full connection (WiFi + MQTT)

#ifdef TEMP_SENSOR
    getDHTdata();
#endif

#ifdef MOTION_SENSOR
    checkMotionState(MOTION_PIN, mqtt_motionTopic, last_motion_state, motion_state);
#endif

#ifdef REED_SENSOR1
    checkReedState(REED_PIN1, mqtt_reed1Topic, last_reed_state1, reed_state1);
#endif

#ifdef REED_SENSOR2
    checkReedState(REED_PIN2, mqtt_reed2Topic, last_reed_state2, reed_state2);
#endif

#ifdef REED_SENSOR3
    checkReedState(REED_PIN3, mqtt_reed3Topic, last_reed_state3, reed_state3);
#endif

#ifdef REED_SENSOR4
    checkReedState(REED_PIN4, mqtt_reed4Topic, last_reed_state4, reed_state4);
#endif
	
  }

	//
	// Handle reboot
	//
	if (rebootAt != 0) {
	  long millisecs = millis();

  	String systemUpTimeMn;
  	String systemUpTimeHr;
//  String systemUpTimeDy;

  	systemUpTimeMn = int((millisecs / (1000 * 60)) % 60);
  	systemUpTimeHr = int((millisecs / (1000 * 60 * 60)) % 24 );
//  systemUpTimeDy = int((millisecs / (1000 * 60 * 60 * 24)) % 365);

#ifdef SERIAL_DEBUG
//    rdebugAln("Reboot after %d ... Counter = Min: %s Hr: %s", rebootAt, systemUpTimeMn.c_str(), systemUpTimeHr.c_str());
#endif
  	if (systemUpTimeHr.toInt() == rebootAt) {
    	MQTTclient.publish("Rebooted", deviceID);
    	reboot();
	  }
	}

#ifdef SERIAL_DEBUG
	Debug.handle();                                                    // Telnet debug window
#endif

//	delay(10);
	yield();

} // loop()
