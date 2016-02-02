/***
* CURRENT SENSOR : Current sensor module, using an ACS712 module
*/

// TODO : 30A ACS712 is way too big, need to try a 3A MAX471
// TODO : code mainWebPage for embedded web app

#if defined(OPTION_MQTT)
    #define topicCurrentRaw    "sensor/currentRaw"
    #define topicCurrentAmps   "sensor/currentAmps"
#endif

class currentModule : public Module {
    private:
        int pin;
        int raw, mAmps;
        long lastUpdt = 0;

    public:
        currentModule(int pin) {
            this->pin = pin;
        }
        
        /***
        * current sensor setup : declare HTTP API endpoint
        */
        void setup() {
            Log::Logln("[NFO] Current sensor initialization");
            MainServer::server.on("/current", (std::bind(&currentModule::statusAPI, this)));
        }

        /***
        * current sensor main loop : sample current level and publish on MQTT topic
        */
        void loop() {
            long now = millis();
            if (now - this->lastUpdt > CURRENT_INTERVAL_MS) {
                this->lastUpdt = now; 
                
                long currentSum = 0;
                for(int i = 0; i < 100; i++) {              // Reads a hundred times ...
                    currentSum += analogRead(this->pin);    // ... sums all the values ...
                }                                           // ...
                int tmp = currentSum / 100.0;             // ... and makes average of value
                if (tmp != this->raw) {
                    this->raw = tmp;
                    this->mAmps = (((long)this->raw  * CURRENT_VOLTAGE_SCALE / 1024) - CURRENT_MIDDLE_POINT) * 1000 / CURRENT_MV_PER_AMP;
                    
                    #if defined(OPTION_MQTT)
                        MQTT.publish(topicCurrentRaw,  String(this->raw).c_str());
                        MQTT.publish(topicCurrentAmps, String(this->mAmps).c_str());
                    #endif
                }
            }
        }

        /***
        * HTTP JSON API : answer with current level
        */
        void statusAPI() {
            String JSONoutput = "";
            JSONoutput += "{ \"currentRaw\": \"";
            JSONoutput += this->raw;
            JSONoutput += "\", \"currentMAmps\": \"";
            JSONoutput += this->mAmps;
            JSONoutput += "\" }\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }

        /***
        * App section for brick main web page
        */
        String mainWebPage(String actualPage) {
            return actualPage;
        }

};
