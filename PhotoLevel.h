/***
 * PHOTO LEVEL : Light sensor module, using a photoresistor
 */
    
#if defined(OPTION_MQTT)
    #define topicLight    "sensor/light"
    #define topicLightPrc "sensor/light_prc"
#endif

class photoModule : public Module {
    private:
        int pin;
        int level, levelPrc;
        long lastUpdt = 0;

    public:
        photoModule(int pin) {
            this->pin = pin;
        }
        
        /***
        * Photosensor setup : declare HTTP API endpoints
        */
        void setup() {
            Log::Logln("[NFO] PhotoSensor initialization");
            MainServer::server.on("/light/level", (std::bind(&photoModule::PhotoLevelAPI, this)));
        }

        /***
        * Photosensor main loop : sample light level and publish on MQTT topics
        */
        void loop() {
            long now = millis();
            if (now - this->lastUpdt > PHOTOSENSOR_INTERVAL_MS) {
                this->lastUpdt = now; 
                
                this->level = analogRead(this->pin);
                this->levelPrc = map(this->level, 0, 1024, 0, 100);
                this->levelPrc = constrain(this->levelPrc, 0, 100);
                
                #if defined(OPTION_MQTT)
                    MQTT.publish(topicLight, String(this->level).c_str());
                    MQTT.publish(topicLightPrc, String(this->levelPrc).c_str());
                #endif
            }
        }

        /***
        * HTTP JSON API : answer with light level (raw and percent)
        */
        void PhotoLevelAPI() {
        String JSONoutput = "";
            JSONoutput += "{ \"LightLevel\": \"";
            JSONoutput += this->level;
            JSONoutput += "\", \"LightLevelPrc\": \"";
            JSONoutput += this->levelPrc;
            JSONoutput += "\" }\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }
};
