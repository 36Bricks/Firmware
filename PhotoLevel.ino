/***
 * PHOTO LEVEL : Light sensor module, using a photoresistor
 */
 
#if defined(MODULE_PHOTO)

  #define PHOTOSENSOR_PIN A0              // Photosensor input pin
  #define PHOTOSENSOR_INTERVAL_MS 1000    // Photosensor update interval
  
  #if defined(OPTION_MQTT)
    #define topicLight    "sensor/light"
    #define topicLightPrc "sensor/light_prc"
  #endif

  int lightLevel;
  int lightLevelPrc;
  long PhotoLastMsg = 0;
  
  /***
   * Photosensor setup : declare HTTP API endpoints
   */
  void PhotoSetup() {
    Logln("[NFO] PhotoSensor initialization");
    server.on("/light/level", PhotoLevelAPI);
  }
  
  /***
   * Photosensor main loop : sample light level and publish on MQTT topics
   */
  void PhotoLoop() {
    long now = millis();
    if (now - PhotoLastMsg > PHOTOSENSOR_INTERVAL_MS) {
      PhotoLastMsg = now; 
      
      lightLevel = analogRead(PHOTOSENSOR_PIN);
      lightLevelPrc = map(lightLevel, 0, 1024, 0, 100);
      lightLevelPrc = constrain(lightLevelPrc, 0, 100);
      
      #if defined(OPTION_MQTT)
        MQTTpublish(topicLight, String(lightLevel).c_str());
        MQTTpublish(topicLightPrc, String(lightLevelPrc).c_str());
      #endif
    }
  }
  
  /***
   * HTTP JSON API : answer with light level (raw and percent)
   */
  void PhotoLevelAPI() {
    String JSONoutput = "";
    JSONoutput += "{ \"LightLevel\": \"";
    JSONoutput += lightLevel;
    JSONoutput += "\", \"LightLevelPrc\": \"";
    JSONoutput += lightLevelPrc;
    JSONoutput += "\" }\r\n";
    server.send(200, "application/json", JSONoutput);
  }

  
#endif
