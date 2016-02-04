/**
* discovery : Auto discover other bricks on network 
* to enable cross-bricks options.
*/
#define DICOVERY_SUBNET_START 1
#define DICOVERY_SUBNET_END 64
#define DICOVERY_INTERVAL_MS 1000

class discoveryOption : public Option {
    private:
        bool readyToScan = false;
        int nextTestIp = DICOVERY_SUBNET_START;
        int otherBricks = 0;
        String subNet;
        long lastUpdt = 0;
        String foundBricks[16];

    public:
        void loop() {
            if (!this->readyToScan) return;
            
            long now = millis();
            if (now - this->lastUpdt > DICOVERY_INTERVAL_MS) {
                this->lastUpdt = now; 

                // Test next IP in subnet
                String ipToTest = this->subNet + "." + String(nextTestIp);
                if(Ping.ping(ipToTest.c_str(),1)) {
                    Log::Logln("[NFO] IP " + ipToTest + " respond to Ping ...");
    
                    // Try the isBrick API endpoint
                    HTTPClient cli;
                    cli.begin("http://"+ipToTest+"/isBrick"); 
                    cli.setTimeout(250);
                    int httpReturnCode = cli.GET();
                    // httpCode will be negative on error
                    if(httpReturnCode > 0) {
                        if(httpReturnCode == HTTP_CODE_OK) {
                            String payload = cli.getString();
                            Log::Logln("[NFO] It's a brick ! ");
                            this->foundBricks[this->otherBricks] = ipToTest;
                            this->otherBricks++;
                        } else {
                            Log::Logln("[NFO] Return code : " + String(httpReturnCode));
                        }
                    }
                }
                this->nextTestIp++;
                
                if (nextTestIp > DICOVERY_SUBNET_END) {
                    this->readyToScan = false;
                    Log::Logln("[NFO] Discovery ended, found " + String(this->otherBricks) + " other bricks : ");
                    for (int i = 0; i < this->otherBricks; i++) {
                       Log::Logln("[NFO] " + String(i) + " : " + String(this->foundBricks[i]));
                    }
                }
            }
        }

        void setup() {
            if (MainWifi::ok) {
                String ourIP = WiFi.localIP().toString();
                this->subNet = ourIP.substring(0, ourIP.lastIndexOf('.'));
                Log::Logln("[NFO] Discovery initialization on subnet " + this->subNet + "."+String(DICOVERY_SUBNET_START)+"->"+String(DICOVERY_SUBNET_END));
                this->readyToScan = true;
            }
        }
        
};
