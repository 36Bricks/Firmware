/***
 * DHT22 : Temperature and humidity module using DHT22 sensor
 */

 // TODO : Find why we have to reset the sensor... we could save one GPIO... perhaps because powered on 3.3v instead of 5v
 // TODO : Add min and max graph values for temperature
     
#if defined(OPTION_MQTT)
    #define topicTemperature  "sensor/temperature"
    #define topicHumidity     "sensor/humidity"
#endif

class dht22Module : public Module {
    private:
        int inputPin, resetPin;
        DHT *dht;
        float tempLevel;
        float humyLevel;
        long lastUpdt = 0;

    public:
        dht22Module(int inputPin, int resetPin) {
            this->inputPin = inputPin;
            this->resetPin = resetPin;
            dht = new DHT(this->inputPin, DHTTYPE);
        }
        
        /***
        * DHT22 module setup : initialize DHT lib, resets the DHT22 and declare HTTP API endpoints
        */
        void setup() {
            Log::Logln("[NFO] DHT22 initialization");
            dht->begin();
            this->ResetDHT22();
            MainServer::server.on("/dht22/",(std::bind(&dht22Module::statusAPI, this)));
        }

        /***
        * DHT22 module main loop : reads the actual temperature and humidity levels
        */
        // TODO : Only publish to MQTT if value changed
        void loop() {
            long now = millis();
            if (now - this->lastUpdt > DHT22_INTERVAL_MS) {
                this->lastUpdt = now; 

                float tmp = dht->readHumidity();       // Reads humidity
                if (!isnan(tmp)) {
                    if (tmp != this->humyLevel) {
                        this->humyLevel = tmp;
                        #if defined(OPTION_MQTT)
                            MQTT.publish(topicHumidity, String(this->humyLevel).c_str());
                        #endif
                    }
                } else {
                    this->ResetDHT22();            // Resets if there is en error, hope next try will be OK
                    return;
                }
                
                tmp = dht->readTemperature();      // Reads temperature
                if ((!isnan(tmp)) && (tmp != this->tempLevel)) {
                     this->tempLevel = tmp;
                     #if defined(OPTION_MQTT)
                        MQTT.publish(topicTemperature, String(this->tempLevel).c_str());
                     #endif
                }
            }
        }
        
        /***
        * Resets the DHT22 : simply cuts its power pin and reenables it 
        */
        void ResetDHT22() {
            pinMode(this->resetPin, OUTPUT);
            digitalWrite(this->resetPin, LOW);
            delay(20);
            digitalWrite(this->resetPin, HIGH);
        }

        /***
        * HTTP JSON API to get temperature and humidity
        */
        void statusAPI() {
            String JSONoutput = "";
            JSONoutput += "{ \"Temperature\": \"";
            JSONoutput += this->tempLevel;
            JSONoutput += "\", \"Humidity\": \"";
            JSONoutput += this->humyLevel;
            JSONoutput += "\" }\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }

        /***
        * App section for brick main web page
        */
        String mainWebPage(String actualPage) {
            actualPage.replace("<!-- %%APP_ZONE%% -->", SpiFfs::readFile("/app_dht22.html"));    
            actualPage.replace("%%TEMP%%", String(this->tempLevel));    // Replace actual temperature
            actualPage.replace("%%HUMY%%", String(this->humyLevel));    // Replace actual humidity
            return actualPage;
        }
};
