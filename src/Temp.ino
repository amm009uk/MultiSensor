
#ifdef TEMP_SENSOR
void getDHTdata() {

#ifdef SERIAL_DEBUG
//  rdebugAln("Running getDHTdata()");
#endif

  now = millis();
  if ( now - lastMsg <= (tempPolling * 1000) ) {                         // Only get DHT data according to polling interval
    return;
  }
  lastMsg = now;

  temp_c  = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temp_c) || isnan(humidity)) {                            // Check if any DHT read failed and exit early to try again
#ifdef SERIAL_DEBUG
    rdebugAln("Failed to read data from DHT sensor!");
#endif
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temp_c, humidity, false);
  temp_state = (char*)String(hic).c_str();                           // Float --> String

//  if (!isnan(hic)) {
    MQTTclient.publish(mqtt_tempTopic, String(hic).c_str());
#ifdef SERIAL_DEBUG
//    rdebugAln("..Humidity: "); debug(String(humidity).c_str());
//    rdebugAln(" %\tTemperature: "); debug(String(temp_c).c_str()); debug(" *C ");
//    rdebugAln(" \tHeat index: "); debug(String(hic).c_str()); debugln(" *C ");

    rdebugAln("Humidity: %f \tTemperature: %f \tHeat index: %f", humidity, temp_c, hic);
#endif
//  }
  
  delay(10);

} // getDHTdata()
#endif