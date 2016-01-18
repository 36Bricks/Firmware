
#if defined(MODULE_OUTPUTS)
  #define OUTPUT_1 D0
  #define OUTPUT_2 D1

  #if defined(OPTION_MQTT)
    #define topicOutput1 "outputs/1"
    #define topicOutput2 "outputs/2"
  #endif 
  
  int led0=0, led1=0;

  void OutputSetup() {
    Logln("[NFO] Outputs initialization");
    pinMode(OUTPUT_1, OUTPUT); DesactivateOutput1(); // LED 0
    pinMode(OUTPUT_2, OUTPUT); DesactivateOutput2(); // LED 1

    server.on("/out/0/on", ActivateOutput1);
    server.on("/out/0/off", DesactivateOutput1);
    server.on("/out/1/on", ActivateOutput2);
    server.on("/out/1/off", DesactivateOutput2);
    server.on("/out/status", OutputStatus);
  }

  void ActivateOutput1() {
      digitalWrite(OUTPUT_1, HIGH); 
      led0 = 1;
      #if defined(OPTION_MQTT)
        MQTTpublish(topicOutput1, "1");
      #endif
      server.send(200, "application/json", ReturnOK);
  }

  void DesactivateOutput1() {
      digitalWrite(OUTPUT_1, LOW); 
      led0 = 0;
      #if defined(OPTION_MQTT)
        MQTTpublish(topicOutput1, "0");
      #endif
      server.send(200, "application/json", ReturnOK);
  }

  void ActivateOutput2() {
      digitalWrite(OUTPUT_2, HIGH); 
      led1 = 1;
      #if defined(OPTION_MQTT)
        MQTTpublish(topicOutput2, "1");
      #endif
      server.send(200, "application/json", ReturnOK);
  }

  void DesactivateOutput2() {
      digitalWrite(OUTPUT_2, LOW); 
      led1 = 0;
      #if defined(OPTION_MQTT)
        MQTTpublish(topicOutput2, "0");
      #endif
      server.send(200, "application/json", ReturnOK);
  }

  void OutputStatus() {
      String JSONoutput = "{ ";
      JSONoutput += "\"out0\": \""; JSONoutput+=(led0 ? "1" : "0"); JSONoutput+="\", ";
      JSONoutput += "\"out1\": \""; JSONoutput+=(led1 ? "1" : "0"); JSONoutput+="\" ";
      JSONoutput += "}\r\n";
      server.send(200, "application/json", JSONoutput);
  }

#endif

