/***
 * CURRENT SENSOR : Light sensor module, using a photoresistor
 */
 
#if defined(MODULE_CURRENT)

  #define CURRENT_PIN A0                  // Current sensor input pin
  #define CURRENT_INTERVAL_MS 1000        // Photosensor update interval

  #define CURRENT_VOLTAGE_SCALE 3300      // 3.3v max scale
  #define CURRENT_MV_PER_AMP    66        // 66mV per amp scale (-30A to 30A range)
  #define CURRENT_MIDDLE_POINT  2500      // analog mV (sensor VCC/2) offset to zero from sensor (tested on proto sensor)
  
  #if defined(OPTION_MQTT)
    #define topicCurrentRaw    "sensor/currentRaw"
    #define topicCurrentAmps    "sensor/currentAmps"
  #endif

  int currentRaw, currentMAmps;
  long currentLastMsg = 0;
  
  /***
   * current sensor setup : declare HTTP API endpoints
   */
  void currentSetup() {
    Logln("[NFO] Current sensor initialization");
    server.on("/current", currentSensorAPI);
  }
  
  /***
   * current sensor main loop : sample current level and publish on MQTT topic
   */
  void currentLoop() {
    long now = millis();
    if (now - currentLastMsg > CURRENT_INTERVAL_MS) {
      currentLastMsg = now; 
      
      long currentSum = 0;
      for(int i = 0; i < 100; i++) {
        currentSum += analogRead(CURRENT_PIN);
      }

      currentRaw = currentSum / 100.0;
      currentMAmps = (((long)currentRaw  * CURRENT_VOLTAGE_SCALE / 1024) - CURRENT_MIDDLE_POINT) * 1000 / CURRENT_MV_PER_AMP;

      #if defined(OPTION_MQTT)
        MQTTpublish(topicCurrentRaw, String(currentRaw).c_str());
        MQTTpublish(topicCurrentAmps, String(currentMAmps).c_str());
      #endif

      //Logln("[DBG] Current : " + String(currentMAmps) + "mA / " + String(currentRaw));
    }
  }
  
  /***
   * HTTP JSON API : answer with current level
   */
  void currentSensorAPI() {
    String JSONoutput = "";
    JSONoutput += "{ \"currentRaw\": \"";
    JSONoutput += currentRaw;
    JSONoutput += "\", \"currentMAmps\": \"";
    JSONoutput += currentMAmps;
    JSONoutput += "\" }\r\n";
    server.send(200, "application/json", JSONoutput);
  }

  
#endif
