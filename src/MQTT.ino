boolean MQTTconnect() {

#ifdef SERIAL_DEBUG
  debugln("Running MQTTconnect()");
  debug("..Attempting MQTT connection to "); debug(mqtt_server); debug("::"); debug((String)mqtt_port); debug("("); debug(mqtt_user); debug("/"); debug(mqtt_password); debugln(")");
#endif
  if (MQTTclient.connect(deviceID, mqtt_user, mqtt_password)) {

/*
#ifdef TEMP_SENSOR
    MQTTclient.publish(mqtt_tempTopic, temp_state);
#endif

#ifdef MOTION_SENSOR
    MQTTclient.publish(mqtt_motionTopic, &motion_state[0]);
#endif

#ifdef REED_SENSOR1
    MQTTclient.publish(mqtt_reed1Topic, &reed_state1[0]);
#endif

#ifdef REED_SENSOR2
    MQTTclient.publish(mqtt_reed2Topic, &reed_state2[0]);
#endif

#ifdef REED_SENSOR3
    MQTTclient.publish(mqtt_reed3Topic, &reed_state3[0]);
#endif

#ifdef REED_SENSOR4
    MQTTclient.publish(mqtt_reed4Topic, &reed_state4[0]);
#endif
*/
		MQTTclient.subscribe(IP_REQUEST);
#ifdef SERIAL_DEBUG
    debugln("..MQTT connected");
#endif

  } else {

#ifdef SERIAL_DEBUG
    debugln("..MQTT connection failed");
#endif

  }

  delay(10);
  return MQTTclient.connected();

} // MQTTconnect()


void callback(char* topic, byte* payload, unsigned int length) {

#ifdef SERIAL_DEBUG
  debugln("In MQTT Callback()");
  debug("..Message arrived ["); debug(topic); debugln("] ");
#endif
	
  if (strcmp(topic, IP_REQUEST)==0) {                                // Check if we want this message if so get the payload

		//																				
		// Build the MQTT messsage name																				
		//																				
		String t_replyMessage = String(t_replyMessage + IP_REPLY + deviceID);
		char replyMessage[40]  = "                              ";
		t_replyMessage.toCharArray(replyMessage, 30);
														
		// Build MQTT message contents																				
		//																				
		String t_Msg = WiFi.localIP().toString();
		char Msg[15] = "              ";																				
		t_Msg.toCharArray(Msg, 15);																				
																					
		MQTTclient.publish(replyMessage, Msg);																				
 
  } else {                                                           // Not interested in this message
  		debugln("..Message ignored");
  	return;
	}

  delay(10);

} // callback()