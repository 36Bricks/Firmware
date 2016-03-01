class brickValue {
    public:
        String name = "";
        String url = "";

        brickValue(String name, String url) {
            this->name = name;
            this->url = url;
        }
};

class brickCommand {
    public:
        String name = "";
        String url = "";

        brickCommand(String name, String url) {
            this->name = name;
            this->url = url;
        }
};

/**
 * DiscoveredBrick : used to store found bricks
 */
class DiscoveredBrick {
    public:
        DiscoveredBrick *nextBrick;
        DiscoveredBrick(String);
        String name;
        String IP;
        brickValue *values[8];
        brickCommand *commands[8];
};
DiscoveredBrick *discoveredBricks = NULL;
DiscoveredBrick::DiscoveredBrick(String IP) {
    nextBrick = discoveredBricks;
    discoveredBricks = this;
    this->IP = IP;
    for (int i=0; i<8; i++) {
        this->values[i] = NULL;
        this->commands[i] = NULL;
    }
}

/**
* discovery : Auto discover other bricks on network 
* to enable cross-bricks options.
*/
#define DICOVERY_SUBNET_START 45
#define DICOVERY_SUBNET_END 64
#define DICOVERY_INTERVAL_MS 1000
class discoveryOption : public Option {
    private:
        bool readyToScan = false;
        int nextTestIp = DICOVERY_SUBNET_START;
        int otherBricks = 0;
        String subNet;
        long lastUpdt = 0;

    public:
        void setup() {
            if (MainWifi::ok) {
                String ourIP = WiFi.localIP().toString();
                this->subNet = ourIP.substring(0, ourIP.lastIndexOf('.'));
                Log::Logln("[NFO] Discovery on subnet " + this->subNet + "."+String(DICOVERY_SUBNET_START)+"->"+String(DICOVERY_SUBNET_END));
                this->readyToScan = true;
                MainServer::server.on("/isBrick", (std::bind(&discoveryOption::isBrickAPI, this)));    // To know if this is a brick, used by Android app and brick discovery
            }
        }
        
        void loop() {
            if (!this->readyToScan) return;
            
            long now = millis();
            if (now - this->lastUpdt > DICOVERY_INTERVAL_MS) {
                this->lastUpdt = now; 

                // Test next IP in subnet
                String ipToTest = this->subNet + "." + String(nextTestIp);
                if(Ping.ping(ipToTest.c_str(),1)) {
                    Log::Logln("[NFO] IP " + ipToTest + " responds to Ping ...");
    
                    // Try the isBrick API endpoint
                    HTTPClient cli;
                    cli.begin("http://"+ipToTest+"/isBrick"); 
                    cli.setTimeout(250);
                    int httpReturnCode = cli.GET();
                    // httpCode will be negative on error
                    if(httpReturnCode > 0) {
                        if(httpReturnCode == HTTP_CODE_OK) {
                            String payload = cli.getString();
                            StaticJsonBuffer<1024> jsonBuffer;
                            JsonObject& json = jsonBuffer.parseObject(payload.c_str());
                            if (!json.success()) {
                                Serial.println("[ERR] Failed to parse config file : " + String(payload));
                            } else {
                                DiscoveredBrick *tmp = new DiscoveredBrick(ipToTest);
                                tmp->name = String(json["brickName"].asString());
                                tmp->values[0] = new brickValue("Temperature", "http://192.168.2.63/dht22/"); // Todo
                                for(int i=0; i<(sizeof(json["commands"])/4); i++) {
                                    tmp->commands[i] = new brickCommand(
                                        json["commands"][i]["name"], 
                                        json["commands"][i]["url"]
                                    );
                                    Log::Logln("[NFO] added command " + String(tmp->commands[i]->name) + String(" / ") + String(tmp->commands[i]->url));
                                }
                                Log::Logln("[NFO] It's a " + String(json["brickType"].asString()) + " brick named " + tmp->name);
                                this->otherBricks++;
                            }
                        } else {
                            Log::Logln("[NFO] Return code : " + String(httpReturnCode));
                        }
                    }
                }
                this->nextTestIp++;
                
                if (nextTestIp > DICOVERY_SUBNET_END) {
                    this->readyToScan = false;
                    Log::Logln("[NFO] Discovery ended, found " + String(this->otherBricks) + " other bricks : ");
                    for (DiscoveredBrick *d = discoveredBricks; d; d = d->nextBrick)
                        Log::Logln("[NFO] " + d->IP);
                }
            }
        }
        
        
        /**
        * Retruns the count of available commands
        */
        unsigned int commandsCount() {
            unsigned int nbCommands = 0;
            for (DiscoveredBrick *d = discoveredBricks; d; d = d->nextBrick) {
                for (int i=0; i<8; i++) {
                    if (d->commands[i] != NULL) {
                        nbCommands++;
                    }
                } 
            }
            return nbCommands;
        }

        /**
        * Discovery HTTP JSON API answer to other bricks and app
        */
        void isBrickAPI () {
            StaticJsonBuffer<1024> jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();
            root["brickType"] = String(BRICK_TYPE);
            root["brickName"] = String(Settings::retreivedName.name);
            root["brickVersion"] = String(FIRMWARE_VERSION);

            JsonArray& nestedCommands = root.createNestedArray("commands");
            unsigned char iCommands=0;
            #if defined(MODULE_OUTPUTS)                                                     // Outputs module
                JsonObject& nestedCommand1 = nestedCommands.createNestedObject();
                nestedCommand1["name"] = String("Output1");
                nestedCommand1["url"]  = String("http://" + String(WiFi.localIP().toString()) + "/out/0/");
                iCommands++;
                JsonObject& nestedCommand2 = nestedCommands.createNestedObject();
                nestedCommand2["name"] = String("Output2");
                nestedCommand2["url"]  = String("http://" + String(WiFi.localIP().toString()) + "/out/1/");
                iCommands++;
            #endif

            char buffer[512];
            root.printTo(buffer, sizeof(buffer));
            MainServer::server.send(200, "application/json", buffer);
        }
        
};


