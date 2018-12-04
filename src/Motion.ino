#ifdef MOTION_SENSOR
void checkMotionState(unsigned int PIN, char* topic, String& last_motion_state, String& motion_state) {

  last_motion_state = motion_state; //get previous state of motion

  if (digitalRead(PIN) == HIGH) {
    motion_state = (char*) "OPEN";
//		digitalWrite(BUILTIN_LED, LOW);                                  // LED on
  } else {
      motion_state = (char*) "CLOSED";
//			digitalWrite(BUILTIN_LED, HIGH);                               // LED off
  }

  // If the state has changed only then publish the change
  if (last_motion_state != motion_state) {
    MQTTclient.publish(topic, &motion_state[0]);

#ifdef SERIAL_DEBUG
	rdebugAln("Motion state change: %s", (char *) motion_state.c_str());
#endif
  }

  delay(10);

} // checkMotionState()
#endif
