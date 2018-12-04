boolean MQTTconnect() {

#ifdef SERIAL_DEBUG
  rdebugAln("Attempting MQTT connection to %s::%d with %s/%s", mqtt_server, mqtt_port, mqtt_user, mqtt_password);
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
    rdebugAln("MQTT connected");
#endif

  } else {

#ifdef SERIAL_DEBUG
    rdebugAln("MQTT connection failed");
#endif
  }

  delay(10);
  return MQTTclient.connected();

} // MQTTconnect()


void callback(char* topic, byte* payload, unsigned int length) {

//#ifdef SERIAL_DEBUG
//  debugln("In MQTT Callback()");
//  debug("..Message arrived ["); debug(topic); debugln("] ");
//#endif

#ifdef SERIAL_DEBUG
	rdebugAln("MQTT Callback() initiated");
  rdebugAln("Message arrived: %s", topic);
#endif
	
  if (strcmp(topic, IP_REQUEST)==0) {                                // Check if we want this message

  	String replyMessage = IP_REPLY;                                  // Build the MQTT reply messsage name
  	replyMessage.concat(deviceID);                                   // ...

  	String Msg = WiFi.localIP().toString();                          // Build MQTT message payload contents

#ifdef SERIAL_DEBUG
	rdebugAln("MQTT Publish %s with payload %s", replyMessage.c_str(), Msg.c_str());
#endif

		MQTTclient.publish(replyMessage.c_str(), Msg.c_str());		       // Publish message to Broker

  } else {                                                           // Not interested in this message
  		rdebugAln("Message ignored");
  	return;
	}

  delay(10);

} // callback()