#if defined(MODULE_PHOTO)

  #define PHOTOSENSOR_PIN A0
  #define PHOTOSENSOR_INTERVAL_MS 1000
  
  #if defined(OPTION_MQTT)
    #define topicLight "sensor/light"
    #define topicLightPrc "sensor/light_prc"
  #endif

  int lightLevel;
  int lightLevelPrc;
  long PhotoLastMsg = 0;
  
  void PhotoSetup() {
    Logln("[NFO] PhotoSensor initialization");
    server.on("/light/level", PhotoAPI);
  }
  
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
  
  void PhotoAPI() {
    String JSONoutput = "";
    JSONoutput += "{ \"LightLevel\": \"";
    JSONoutput += lightLevel;
    JSONoutput += "\", \"LightLevelPrc\": \"";
    JSONoutput += lightLevelPrc;
    JSONoutput += "\" }\r\n";
    server.send(200, "application/json", JSONoutput);
  }

  
#endif
