/***
 * OUTPUTS : Digital outputs modules. Used to control relays.
 */
 
// TODO : HTTP API endpoints for timed and sequenced outputs modes
// TODO : Add initial state to app section web page
    
#if defined(OPTION_MQTT)
    #define topicOutput1 "outputs/1"
    #define topicOutput2 "outputs/2"
#endif 

class outputModule : public Module {
    private:
        int pin1, pin2;
        bool output1 = false;
        bool output2 = false;

        bool blink1Enabled = false, blink2Enabled = false;
        long blink1OnDuration  = 0, blink2OnDuration  = 0;
        long blink1OffDuration = 0, blink2OffDuration = 0;
        long lastBlink1Switch  = 0, lastBlink2Switch  = 0;
        
    public:
        void loop() {
            blink1Loop();
            blink2Loop();
        }
        
        outputModule(int pin1, int pin2) {
            this->pin1 = pin1;
            this->pin2 = pin2;
        }
        
        /***
        * Outputs setup : set pins modes, and declare HTTP API endpoints
        */
        void setup() {
            Log::Logln("[NFO] Outputs initialization");
            pinMode(this->pin1, OUTPUT); this->desactivateOutput1();
            pinMode(this->pin2, OUTPUT); this->desactivateOutput2();
            
            MainServer::server.on("/out/0/on", (std::bind(&outputModule::activateOutput1, this)));
            MainServer::server.on("/out/0/off", (std::bind(&outputModule::desactivateOutput1, this)));
            MainServer::server.on("/out/0/blink", (std::bind(&outputModule::blink1API, this)));
            MainServer::server.on("/out/1/on", (std::bind(&outputModule::activateOutput2, this)));
            MainServer::server.on("/out/1/off", (std::bind(&outputModule::desactivateOutput2, this)));
            MainServer::server.on("/out/1/blink", (std::bind(&outputModule::blink2API, this)));
            MainServer::server.on("/out/status", (std::bind(&outputModule::statusAPI, this)));
        }

        /***
        * HTTP JSON API to enable output 1 
        */
        void activateOutput1() {
            digitalWrite(this->pin1, HIGH); 
            blink1Enabled = false;
            if (!this->output1) {
                this->output1 = true;
                #if defined(OPTION_MQTT)
                    MQTT.publish(topicOutput1, "1");
                #endif
            }
            MainServer::ReturnOK();
        }

        /***
        * HTTP JSON API to disable output 1 
        */
        void desactivateOutput1() {
            digitalWrite(this->pin1, LOW); 
            blink1Enabled = false;
            if (this->output1) {
                this->output1 = false;
                #if defined(OPTION_MQTT)
                    MQTT.publish(topicOutput1, "0");
                #endif
            }
            MainServer::ReturnOK();
        }
        
        /***
        * HTTP JSON API to enable output 2 
        */
        void activateOutput2() {
            digitalWrite(this->pin2, HIGH); 
            blink2Enabled = false;
            if (!this->output2) {
                this->output2 = true;
                #if defined(OPTION_MQTT)
                    MQTT.publish(topicOutput2, "1");
                #endif
            }
            MainServer::ReturnOK();
        }
        
       /***
        * HTTP JSON API to disable output 2 
        */
        void desactivateOutput2() {
            digitalWrite(this->pin2, LOW); 
            blink2Enabled = false;
            if (this->output2) {
                this->output2 = false;
                #if defined(OPTION_MQTT)
                    MQTT.publish(topicOutput2, "0");
                #endif
            }
            MainServer::ReturnOK();
        }
        
        /***
        * HTTP JSON API to send outputs statuses
        */
        void statusAPI() {
            String JSONoutput = "{ ";
            JSONoutput += "\"out0\": \""; JSONoutput+=(this->output1 ? "1" : "0"); JSONoutput+="\", ";
            JSONoutput += "\"out1\": \""; JSONoutput+=(this->output2 ? "1" : "0"); JSONoutput+="\" ";
            JSONoutput += "}\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }

        /***
        * App section for brick main web page
        */
        String mainWebPage(String actualPage) {
            actualPage.replace("<!-- %%APP_ZONE%% -->", SpiFfs::readFile("/app_outputs.html"));    
            return actualPage;
        }

        /***
        * Blink output 1 loop
        */
        void blink1Loop() {
            if (!this->blink1Enabled) return;
            
            long testDuration = 0;
            if (this->output1) testDuration = this->blink1OnDuration;
            else testDuration = this->blink1OffDuration;

            if (millis() - this->lastBlink1Switch > testDuration) {
                this->lastBlink1Switch = millis();
                if (this->output1) {
                    Log::Logln("[NFO] Output 1 blink OFF");
                    digitalWrite(this->pin1, LOW); 
                    this->output1 = false;
                } else {
                    Log::Logln("[NFO] Output 1 blink ON");
                    digitalWrite(this->pin1, HIGH); 
                    this->output1 = true;
                }
            }
        }

        /***
        * Blink output 2 loop
        */
        void blink2Loop() {
            if (!this->blink2Enabled) return;
            
            long testDuration = 0;
            if (this->output2) testDuration = this->blink2OnDuration;
            else testDuration = this->blink2OffDuration;

            if (millis() - this->lastBlink2Switch > testDuration) {
                this->lastBlink2Switch = millis();
                if (this->output2) {
                    Log::Logln("[NFO] Output 2 blink OFF");
                    digitalWrite(this->pin2, LOW); 
                    this->output2 = false;
                } else {
                    Log::Logln("[NFO] Output 2 blink ON");
                    digitalWrite(this->pin2, HIGH); 
                    this->output2 = true;
                }
            }
        }

        /***
        * HTTP JSON API to blink output 1
        */
        void blink1API() {
            this->blink1OnDuration  = MainServer::server.arg("on").toInt();  // Parse on duration
            this->blink1OffDuration = MainServer::server.arg("off").toInt(); // Parse off duration
            if ((this->blink1OnDuration==0) || (this->blink1OffDuration==0)) {
                Log::Logln("[NFO] Output 1 blink mode stopped");
                this->blink1Enabled = false;
            } else {
                Log::Logln("[NFO] Output 1 blink mode started : " + String(this->blink1OnDuration) + " / " + String(this->blink1OffDuration));
               this->blink1Enabled = true;
               this->lastBlink1Switch = millis();
            }
            MainServer::ReturnOK();
        }

        /***
        * HTTP JSON API to blink output 2
        */
        void blink2API() {
            this->blink2OnDuration  = MainServer::server.arg("on").toInt();  // Parse on duration
            this->blink2OffDuration = MainServer::server.arg("off").toInt(); // Parse off duration
            if ((this->blink2OnDuration==0) || (this->blink2OffDuration==0)) {
                Log::Logln("[NFO] Output 2 blink mode stopped");
                this->blink2Enabled = false;
            } else {
                Log::Logln("[NFO] Output 2 blink mode started : " + String(this->blink2OnDuration) + " / " + String(this->blink2OffDuration));
               this->blink2Enabled = true;
               this->lastBlink2Switch = millis();
            }
            MainServer::ReturnOK();
        }

};

