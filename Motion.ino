/***
 * MOTION : HC-SR501 Infrared PIR Motion Sensor module
 */

#if defined(MODULE_MOTION)

  #define MOTIONSENSOR_PIN D2              // Motion sensor input pin
  #define MOTIONSENSOR_INTERVAL_MS 100     // Motion sensor update interval
  
  #if defined(OPTION_MQTT)
    #define motionTopic    "sensor/motion"
  #endif

  bool motionSensed = false;
  long motionLastMsg = 0;
  
  /***
   * Motion sensor setup : set input pin mode and declare HTTP API endpoints
   */
  void MotionSetup() {
    Logln("[NFO] Motion sensor initialization");
    pinMode(MOTIONSENSOR_PIN, INPUT);
    server.on("/motion", MotionAPI);
  }
  
  /***
   * Motion sensor main loop : sample motion level and publish on MQTT topic
   */
  void MotionLoop() {
    long now = millis();
    if (now - motionLastMsg > MOTIONSENSOR_INTERVAL_MS) {
      motionLastMsg = now; 
      
      switch (digitalRead(MOTIONSENSOR_PIN)) {
          case HIGH :
              if (motionSensed == false) { // Motion just sensed
                  Logln("[NFO] Motion sensed");
                  motionSensed = true;
                  #if defined(OPTION_MQTT)
                    MQTTpublish(motionTopic, "1");
                  #endif
              }
              break;
          case LOW:
              if (motionSensed == true) { // Motion just stoped
                  Logln("[NFO] Motion disapeared");
                  motionSensed = false;
                  #if defined(OPTION_MQTT)
                    MQTTpublish(motionTopic, "0");
                  #endif
              }
              break;
      }
    }
  }
  
  /***
   * HTTP JSON API
   */
  void MotionAPI() {
    String JSONoutput = "";
    JSONoutput += "{ \"motion\": \"";
    JSONoutput += ((motionSensed==true)?"1":"0");
    JSONoutput += "\" }\r\n";
    server.send(200, "application/json", JSONoutput);
  }

  
#endif
