/**  
 *   SWITCH RETROFIT : Uses a wall switch as input and use it ant convert it in a Wifi switch using a relay as output
 *   The output relay is also controllable on itw own
 *   The input switch can also be used on its own
 */

 // TODO : HTTP API endpoint to set virtual witch state

#if defined(OPTION_MQTT)
    #define topicSwRetroVirtualSwitch "swRetro/virtualSwitch"
    #define topicSwRetroRealSwitch "swRetro/realSwitch"
#endif 

class switchRetrofitModule : public Module {
    private:
        int inPin, outPin;
        long lastMsg = 0;
        bool realSwitch = false;      // Real switch state
        bool virtualSwitch = false;   // Virtual switch state
        
    public:
        switchRetrofitModule(int inPin, int outPin) {
            this->inPin = inPin;
            this->outPin = outPin;
        }
        
        /***
        * Switch retrofit setup : set pins modes, and declare HTTP API endpoints
        */
        void setup() {
            Log::Logln("[NFO] Switch Retrofit initialization");
            pinMode(this->inPin, INPUT);
            pinMode(this->outPin, OUTPUT);
            
            this->realSwitch = ((digitalRead(this->inPin) == LOW) ? false : true);  // Get initial real switch state
            
            MainServer::server.on("/swRetro/status", (std::bind(&switchRetrofitModule::statusAPI, this)));
            MainServer::server.on("/swRetro/switch", (std::bind(&switchRetrofitModule::switchVirtualAPI, this)));
        }
        
        /***
        * Switch retrofit timed main loop : it inverts virtual switch state when real switch state changes 
        */
        void loop() {
            long now = millis();
            if (now - this->lastMsg > SW_RETRO_INTERVAL_MS) {
                this->lastMsg = now; 
                
                if (digitalRead(this->inPin) == LOW) {
                    if (this->realSwitch != false) {        // Real switch just switched off
                        this->realSwitch = false;
                        this->switchVirtual();              // Invert virtual switch
                        #if defined(OPTION_MQTT)
                            MQTT.publish(topicSwRetroRealSwitch, "0");
                        #endif
                    }
                } else {
                    if (this->realSwitch != true) {         // Real switch just switched on
                        this->realSwitch = true;
                        this->switchVirtual();              // Invert virtual switch
                        #if defined(OPTION_MQTT)
                            MQTT.publish(topicSwRetroRealSwitch, "1");
                        #endif
                    }
                }
            }
        }
        
        /***
        * Update the output state (controlling relay) according to virtual switch state
        */
        void updateSwitchOutput() {
            digitalWrite(this->outPin, (this->virtualSwitch ? HIGH : LOW));
        }
        
        /***
        * Invert virtual switch state and publish to MQTT
        */
        void switchVirtual() {
            this->virtualSwitch = !this->virtualSwitch;     // Invert the swicth
            this->updateSwitchOutput();
            #if defined(OPTION_MQTT)
                MQTT.publish(topicSwRetroVirtualSwitch, (this->virtualSwitch ? "1" : "0"));
            #endif
        }
        
        /***
        * HTTP JSON API to invert virtual switch 
        */
        void switchVirtualAPI() {
            this->switchVirtual();
            MainServer::ReturnOK();
        }
        
        /***
        * HTTP JSON API to get real and virtual switch states
        */
        void statusAPI() {
            String JSONoutput = "{ ";
            JSONoutput += "\"realSwitch\": \"";     JSONoutput+=(this->realSwitch ? "1" : "0"); JSONoutput+="\", ";
            JSONoutput += "\"virtualSwitch\": \"";  JSONoutput+=(this->virtualSwitch ? "1" : "0"); JSONoutput+="\" ";
            JSONoutput += "}\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }
  };

