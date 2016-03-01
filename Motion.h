/***
 * MOTION : HC-SR501 Infrared PIR Motion Sensor module
 */
  
#if defined(OPTION_MQTT)
    #define motionTopic    "sensor/motion"
#endif


class motionModule : public Module {
    private:
        int inputPin;
        bool motionSensed = false;
        long motionLastMsg = 0;

    public:
        motionModule(int inputPin) {
            this->inputPin = inputPin;
        }
        /***
        * Motion sensor setup : set input pin mode and declare HTTP API endpoints
        */
        void setup() {
            Log::Logln("[NFO] Motion sensor initialization");
            pinMode(this->inputPin, INPUT);
            MainServer::server.on("/motion", (std::bind(&motionModule::statusAPI, this)));
        }
        
        /***
        * Motion sensor main loop : sample motion level and publish on MQTT topic
        */
        void loop() {
            long now = millis();
            if (now - this->motionLastMsg > MOTIONSENSOR_INTERVAL_MS) {
                this->motionLastMsg = now; 
                
                switch (digitalRead(this->inputPin)) {
                    case HIGH :
                        if (this->motionSensed == false) { // Motion just sensed
                            Log::Logln("[NFO] Motion sensed");
                            this->motionSensed = true;
                            #if defined(OPTION_MQTT)
                                MQTT.publish(motionTopic, "1");
                            #endif
                        }
                        break;
                    case LOW:
                        if (this->motionSensed == true) { // Motion just stoped
                            Log::Logln("[NFO] Motion disapeared");
                            this->motionSensed = false;
                            #if defined(OPTION_MQTT)
                                MQTT.publish(motionTopic, "0");
                            #endif
                        }
                        break;
                }
            }
        }
        
        /***
        * HTTP JSON API
        */
        void statusAPI() {
            String JSONoutput = "";
            JSONoutput += "{ \"motion\": \"";
            JSONoutput += ((this->motionSensed==true)?"1":"0");
            JSONoutput += "\" }\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }

        /***
        * App section for brick main web page
        */
        // TODO
        String mainWebPage(String actualPage) {
            return actualPage;
        }
};
