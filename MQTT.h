/***
 * MQTT : Option to publish to MQTT broker
 */
 
class mqttOption : public Option {
    
    private:
        WiFiClient wifiClient;
        PubSubClient *MQTTclient;
        bool ok = false;                  // MQTT Connection state
                
    public:
        mqttOption() {
            MQTTclient = new PubSubClient(this->wifiClient);
        }
        
        /***
        * MQTT main loop : keeps MQTT connected and processes MQTT loop
        */
        void loop() {
           if (MainWifi::ok && this->ok && Settings::retreivedMQTTenabled.enabled) {
                if (!MQTTclient->connected()) {
                    this->reconnect();
                }
                MQTTclient->loop();
            }
        }

        /***
        * MQTT option setup : sets the MQTT server communication
        */
        void setup() {
            Log::Logln("[NFO] MQTT initialization");
            this->ok = Settings::servOK && Settings::portOK && Settings::enabledOK;        // MQTT options (server and port) are defined and loaded
            if (this->ok && Settings::retreivedMQTTenabled.enabled) {
                MQTTclient->setServer(Settings::retreivedMQTTserv.serv, atoi(Settings::retreivedMQTTport.port));
                this->reconnect();
            } else 
                Log::Logln("[NFO] MQTT disabled");
        }

        /***
        * Connects to the MQTT server
        */
        void reconnect() {
            int nbEssaisMQTT = 0;
            while (!MQTTclient->connected()) {
                Log::Logln(String("[NFO] Attempting MQTT connection : " + String(Settings::retreivedMQTTserv.serv)));
                if (MQTTclient->connect("36BrickClient")) {
                    Log::Logln("[NFO] MQTT connected");
                } else {
                    nbEssaisMQTT++;
                    if (nbEssaisMQTT >= MQTT_ESSAIS_MAX) {
                        Log::Logln("[NFO] MQTT connection failed ! MQTT disabled");
                        this->ok = false;
                        return;
                    } else {
                        Log::Logln("[NFO] MQTT connection failed, try again in 5 seconds");
                        delay(5000);
                    }
                    yield();
                }
            }
        }
        
        /***
        * Publishes an information to an MQTT topic
        */
        void publish(String topic, String info ) {
            if (MainWifi::ok && this->ok) MQTTclient->publish(String(MQTTprefix + topic).c_str(), info.c_str(), true);
        }

};

