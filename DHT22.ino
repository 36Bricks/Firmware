#if defined(MODULE_DHT22)
  #include <DHT.h>

  #define DHT22_PIN D4
  #define DHT22_RESET_PIN D8
  #define DHTTYPE DHT22
  
  DHT dht(DHT22_PIN, DHTTYPE);

  #if defined(OPTION_MQTT)
    #define topicTemperature "sensor/temperature"
    #define topicHumidity "sensor/humidity"
  #endif

  #define DHT22_INTERVAL_MS 5000
  
  void Dht22Setup() {
    Logln("[NFO] DHT22 initialization");
    dht.begin();
    ResetDHT22();
    
    server.on("/dht22/", Dht22API);
  }
  
  void ResetDHT22() {
    pinMode(DHT22_RESET_PIN, OUTPUT);
    delay(20);
    digitalWrite(DHT22_RESET_PIN, LOW);
    delay(20);
    digitalWrite(DHT22_RESET_PIN, HIGH);
  }
  
  float tempLevel;
  float humyLevel;

  long dht22LastMsg = 0;

  void Dht22Loop() {
    long now = millis();
    if (now - dht22LastMsg > DHT22_INTERVAL_MS) {
      dht22LastMsg = now; 
      
      humyLevel = dht.readHumidity();
      tempLevel = dht.readTemperature();

      if (!isnan(humyLevel)) {
        #if defined(OPTION_MQTT)
          MQTTpublish(topicHumidity, String(humyLevel).c_str());
        #endif
      } else {
        ResetDHT22();
        return;
      }
      if (!isnan(tempLevel)) {
        #if defined(OPTION_MQTT)
          MQTTpublish(topicTemperature, String(tempLevel).c_str());
        #endif
      } 
    }
  }
  
  void Dht22API() {
    String JSONoutput = "";
    JSONoutput += "{ \"Temperature\": \"";
    JSONoutput += tempLevel;
    JSONoutput += "\", \"Humidity\": \"";
    JSONoutput += humyLevel;
    JSONoutput += "\" }\r\n";
    server.send(200, "application/json", JSONoutput);
  }
#endif

