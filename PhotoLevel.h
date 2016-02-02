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

                int tmp = analogRead(this->pin);
                if (tmp != this->level) {
                    this->level = tmp
                    #if defined(OPTION_MQTT)
                        MQTT.publish(topicLight, String(this->level).c_str());
                    #endif
                }

                tmp = constrain(map(this->level, 0, 1024, 0, 100), 0, 100);
                if (tmp != this->levelPrc) {
                    this->levelPrc = tmp
                    #if defined(OPTION_MQTT)
                        MQTT.publish(topicLightPrc, String(this->levelPrc).c_str());
                    #endif
                }
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
