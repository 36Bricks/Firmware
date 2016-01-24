/***
 * DHT22 : Temperature and humidity module using DHT22 sensor
 */

 // TODO : Find why we have to reset the sensor... we could save one GPIO... perhaps because powered on 3.3v instead of 5v
 // TODO : Add min and max graph values for temperature
 
#if defined(MODULE_DHT22)
  #include <DHT.h>

  #define DHT22_PIN D4            // DHT22 input pin
  #define DHT22_RESET_PIN D8      // DHT22 reset (and power) pin
  #define DHTTYPE DHT22           // Sensor type : DHT22
  #define DHT22_INTERVAL_MS 5000  // DHT22 read interval
  
  DHT dht(DHT22_PIN, DHTTYPE);
  float dht22TempLevel;
  float dht22HumyLevel;
  long dht22LastMsg = 0;

  #if defined(OPTION_MQTT)
    #define topicTemperature  "sensor/temperature"
    #define topicHumidity     "sensor/humidity"
  #endif
  
  /***
   * DHT22 module setup : initialize DHT lib, resets the DHT22 and declare HTTP API endpoints
   */
  void Dht22Setup() {
    Logln("[NFO] DHT22 initialization");
    dht.begin();
    ResetDHT22();
    server.on("/dht22/", Dht22API);
  }
  
  /***
   * Resets the DHT22 : simply cuts its power pin and reenables it 
   */
  void ResetDHT22() {
    pinMode(DHT22_RESET_PIN, OUTPUT);
    digitalWrite(DHT22_RESET_PIN, LOW);
    delay(20);
    digitalWrite(DHT22_RESET_PIN, HIGH);
  }
  
  /***
   * DHT22 module main loop : reads the actual temperature and humidity levels
   */
  void Dht22Loop() {
    long now = millis();
    if (now - dht22LastMsg > DHT22_INTERVAL_MS) {
      dht22LastMsg = now; 
      
      dht22HumyLevel = dht.readHumidity();         // Reads humidity
      dht22TempLevel = dht.readTemperature();      // Reads temperature

      if (!isnan(dht22HumyLevel)) {
        #if defined(OPTION_MQTT)
          MQTTpublish(topicHumidity, String(dht22HumyLevel).c_str());
        #endif
      } else {
        ResetDHT22();                         // Resets if there is en error, hope next try will be OK
        return;
      }
      if (!isnan(dht22TempLevel)) {
        #if defined(OPTION_MQTT)
          MQTTpublish(topicTemperature, String(dht22TempLevel).c_str());
        #endif
      } 
    }
  }
  
  /***
   * HTTP JSON API to get temperature and humidity
   */
  void Dht22API() {
    String JSONoutput = "";
    JSONoutput += "{ \"Temperature\": \"";
    JSONoutput += dht22TempLevel;
    JSONoutput += "\", \"Humidity\": \"";
    JSONoutput += dht22HumyLevel;
    JSONoutput += "\" }\r\n";
    server.send(200, "application/json", JSONoutput);
  }

  /***
   * App section for brick main web page
   */
  String dht22MainWebPage(String actualPage) {
      actualPage.replace("<!-- %%APP_ZONE%% -->", readFromFlash("app_dht22.html"));    
      actualPage.replace("%%TEMP%%", String(dht22TempLevel));    // Replace actual temperature
      actualPage.replace("%%HUMY%%", String(dht22HumyLevel));    // Replace actual humidity
      return actualPage;
  }

#endif

