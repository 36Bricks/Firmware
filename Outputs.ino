/***
 * OUTPUTS : Digital outputs modules. Used to control relays.
 */
 
// TODO : HTTP API endpoints for timed and sequenced outputs modes
// TODO : Add initial state to app section web page

#if defined(MODULE_OUTPUTS)

  #define OUTPUT_1 D0     // Output 1 pin
  #define OUTPUT_2 D1     // Output 2 pin
  
  #if defined(OPTION_MQTT)
    #define topicOutput1 "outputs/1"
    #define topicOutput2 "outputs/2"
  #endif 
  
  bool output1 = false;
  bool output2 = false;

  /***
   * Outputs setup : set pins modes, and declare HTTP API endpoints
   */
  void outputSetup() {
    Logln("[NFO] Outputs initialization");
    pinMode(OUTPUT_1, OUTPUT); desactivateOutput1(); // LED 0
    pinMode(OUTPUT_2, OUTPUT); desactivateOutput2(); // LED 1

    server.on("/out/0/on", activateOutput1);
    server.on("/out/0/off", desactivateOutput1);
    server.on("/out/1/on", activateOutput2);
    server.on("/out/1/off", desactivateOutput2);
    server.on("/out/status", outputStatus);
  }

  /***
   * HTTP JSON API to enable output 1 
   */
  void activateOutput1() {
      digitalWrite(OUTPUT_1, HIGH); 
      output1 = true;
      #if defined(OPTION_MQTT)
        MQTTpublish(topicOutput1, "1");
      #endif
      server.send(200, "application/json", ReturnOK);
  }

  /***
   * HTTP JSON API to disable output 1 
   */
  void desactivateOutput1() {
      digitalWrite(OUTPUT_1, LOW); 
      output1 = false;
      #if defined(OPTION_MQTT)
        MQTTpublish(topicOutput1, "0");
      #endif
      server.send(200, "application/json", ReturnOK);
  }

  /***
   * HTTP JSON API to enable output 2 
   */
  void activateOutput2() {
      digitalWrite(OUTPUT_2, HIGH); 
      output2 = true;
      #if defined(OPTION_MQTT)
        MQTTpublish(topicOutput2, "1");
      #endif
      server.send(200, "application/json", ReturnOK);
  }

  /***
   * HTTP JSON API to disable output 2 
   */
  void desactivateOutput2() {
      digitalWrite(OUTPUT_2, LOW); 
      output2 = false;
      #if defined(OPTION_MQTT)
        MQTTpublish(topicOutput2, "0");
      #endif
      server.send(200, "application/json", ReturnOK);
  }

  /***
   * HTTP JSON API to send outputs statuses
   */
  void outputStatus() {
      String JSONoutput = "{ ";
      JSONoutput += "\"out0\": \""; JSONoutput+=(output1 ? "1" : "0"); JSONoutput+="\", ";
      JSONoutput += "\"out1\": \""; JSONoutput+=(output2 ? "1" : "0"); JSONoutput+="\" ";
      JSONoutput += "}\r\n";
      server.send(200, "application/json", JSONoutput);
  }

  /***
   * App section for brick main web page
   */
  String outputMainWebPage(String actualPage) {
      actualPage.replace("<!-- %%APP_ZONE%% -->", readFromFlash("app_outputs.html"));    
      return actualPage;
  }


#endif

