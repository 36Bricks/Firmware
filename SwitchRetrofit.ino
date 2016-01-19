/**  
 *   SWITCH RETROFIT : Uses a wall switch as input and use it ant convert it in a Wifi switch using a relay as output
 *   The output relay is also controllable on itw own
 *   The input switch can also be used on its own
 */

 // TODO : HTTP API endpoint to set virtual witch state
 
#if defined(MODULE_SWITCH_RETROFIT)
 
  #define SW_RETRO_IN D0            // Real switch input pin
  #define SW_RETRO_OUT D1           // Relay pin
  
  #define SW_RETRO_INTERVAL_MS 50   // Main loop interval time

  long switchRetrofitLastMsg = 0;
  bool realSwitch = false;      // Real switch state
  bool virtualSwitch = false;   // Virtual switch state

  #if defined(OPTION_MQTT)
    #define topicSwRetroVirtualSwitch "swRetro/virtualSwitch"
    #define topicSwRetroRealSwitch "swRetro/realSwitch"
  #endif 
  
  /***
   * Switch retrofit setup : set pins modes, and declare HTTP API endpoints
   */
  void switchRetrofitSetup() {
    Logln("[NFO] Switch Retrofit initialization");
    pinMode(SW_RETRO_IN, INPUT);
    pinMode(SW_RETRO_OUT, OUTPUT);

    realSwitch = ((digitalRead(SW_RETRO_IN) == LOW) ? false : true);  // Get initial real switch state
    
    server.on("/swRetro/status", switchRetrofitStatusAPI);
    server.on("/swRetro/switch", switchRetrofitSwitchVirtualAPI);
  }

  /***
   * Switch retrofit timed main loop : it inverts virtual switch state when real switch state changes 
   */
  void switchRetrofitLoop() {
    long now = millis();
    if (now - switchRetrofitLastMsg > SW_RETRO_INTERVAL_MS) {
      switchRetrofitLastMsg = now; 
      
      if (digitalRead(SW_RETRO_IN) == LOW) {
        if (realSwitch != false) {        // Real switch just switched off
          realSwitch = false;
          switchRetrofitSwitchVirtual();  // Invert virtual switch
          #if defined(OPTION_MQTT)
            MQTTpublish(topicSwRetroRealSwitch, "0");
          #endif
        }
      } else {
        if (realSwitch != true) {         // Real switch just switched on
          realSwitch = true;
          switchRetrofitSwitchVirtual();  // Invert virtual switch
          #if defined(OPTION_MQTT)
            MQTTpublish(topicSwRetroRealSwitch, "1");
          #endif
        }
      }
  
    }
  }
  
  /***
   * Update the output state (controlling relay) according to virtual switch state
   */
  void updateSwitchOutput() {
    digitalWrite(SW_RETRO_OUT, (virtualSwitch ? HIGH : LOW));
  }

  /***
   * Invert virtual switch state and publish to MQTT
   */
  void switchRetrofitSwitchVirtual() {
      virtualSwitch = !virtualSwitch;     // Invert the swicth
      updateSwitchOutput();
      #if defined(OPTION_MQTT)
        MQTTpublish(topicSwRetroVirtualSwitch, (virtualSwitch ? "1" : "0"));
      #endif
  }

  /***
   * HTTP JSON API to invert virtual switch 
   */
  void switchRetrofitSwitchVirtualAPI() {
      switchRetrofitSwitchVirtual();
      server.send(200, "application/json", ReturnOK);
  }

  /***
   * HTTP JSON API to get real and virtual switch states
   */
  void switchRetrofitStatusAPI() {
      String JSONoutput = "{ ";
      JSONoutput += "\"realSwitch\": \"";     JSONoutput+=(realSwitch ? "1" : "0"); JSONoutput+="\", ";
      JSONoutput += "\"virtualSwitch\": \"";  JSONoutput+=(virtualSwitch ? "1" : "0"); JSONoutput+="\" ";
      JSONoutput += "}\r\n";
      server.send(200, "application/json", JSONoutput);
  }

#endif

