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
    public:
        void loop() {}
        
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
            MainServer::server.on("/out/1/on", (std::bind(&outputModule::activateOutput2, this)));
            MainServer::server.on("/out/1/off", (std::bind(&outputModule::desactivateOutput2, this)));
            MainServer::server.on("/out/status", (std::bind(&outputModule::statusAPI, this)));
        }

        /***
        * HTTP JSON API to enable output 1 
        */
        void activateOutput1() {
            digitalWrite(this->pin1, HIGH); 
            this->output1 = true;
            #if defined(OPTION_MQTT)
                MQTT.publish(topicOutput1, "1");
            #endif
            MainServer::ReturnOK();
        }

        /***
        * HTTP JSON API to disable output 1 
        */
        void desactivateOutput1() {
            digitalWrite(this->pin1, LOW); 
            this->output1 = false;
            #if defined(OPTION_MQTT)
                MQTT.publish(topicOutput1, "0");
            #endif
            MainServer::ReturnOK();
        }
        
        /***
        * HTTP JSON API to enable output 2 
        */
        void activateOutput2() {
            digitalWrite(this->pin2, HIGH); 
            this->output2 = true;
            #if defined(OPTION_MQTT)
                MQTT.publish(topicOutput2, "1");
            #endif
            MainServer::ReturnOK();
        }
        
       /***
        * HTTP JSON API to disable output 2 
        */
        void desactivateOutput2() {
            digitalWrite(this->pin2, LOW); 
            this->output2 = false;
            #if defined(OPTION_MQTT)
                MQTT.publish(topicOutput2, "0");
            #endif
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

};

