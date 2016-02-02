/**
 * STRIP : Drives a non-adressable RGB Led Strip through an RGB driver module
 */
 // TODO : HTTP API endpoint to fade ON/OFF
 // TODO : HTTP API endpoint to flash a color (alarm)
      
class stripModule : public Module {
    private:
        int clkPin, dioPin;
        RGBdriver *driver;
        long rainbowLastUpdt = 0;
        
        int rainbowLoopIndex=0;
        bool rainbowLoopEnabled = true;
        
        long red, green, blue;
        
    public:
        stripModule(int clkPin, int dioPin) {
            this->clkPin = clkPin;
            this->dioPin = dioPin;
            this->driver = new RGBdriver(this->clkPin, this->dioPin);
        }
        
        /***
        * Strip setup : declare HTTP API endpoints
        */
        void setup() {
            Log::Logln("[NFO] RGB Strip initialization");
            MainServer::server.on("/strip/hue", (std::bind(&stripModule::stripSetColorAPI, this)));
            MainServer::server.on("/strip/rainbow/on", (std::bind(&stripModule::stripEnableRainbowAPI, this)));
            MainServer::server.on("/strip/rainbow/off", (std::bind(&stripModule::stripDisableRainbowAPI, this)));
            MainServer::server.on("/strip/getcolor", (std::bind(&stripModule::stripGetColorAPI, this)));
        }
        
        /***
        * Strip main loop
        */
        void loop() {
            stripRainbowLoop();
        }
        
        /***
        * Strip rainbow loop : loops through colors 
        */
        void stripRainbowLoop() {
            if (this->rainbowLoopEnabled) {
                long now = millis();
                if (now - this->rainbowLastUpdt > STRIP_INTERVAL_MS) {
                    this->rainbowLastUpdt = now; 
                    int WheelPos = 255 - this->rainbowLoopIndex;
                    if(WheelPos < 85) {
                        this->red = 255 - WheelPos * 3;
                        this->green = 0;
                        this->blue = WheelPos * 3; 
                    } else if(WheelPos < 170) {
                        WheelPos -= 85;
                        this->red = 0;
                        this->green = WheelPos * 3;
                        this->blue = 255 - WheelPos * 3; 
                    } else {
                        WheelPos -= 170;
                        this->red = WheelPos * 3;
                        this->green = 255 - WheelPos * 3;
                        this->blue = 0; 
                    }
                    this->driver->begin();
                    this->driver->SetColor(this->red, this->green, this->blue);
                    this->driver->end();
                    this->rainbowLoopIndex++;
                    if (this->rainbowLoopIndex>=256) this->rainbowLoopIndex=0;
                }
            }
        }
        
        /***
        * HTTP JSON API to enable rainbow loop 
        */
        void stripEnableRainbowAPI() {
            this->rainbowLoopEnabled = true;
            MainServer::ReturnOK();
        }
        
        /***
        * HTTP JSON API to disable rainbow loop 
        */
        void stripDisableRainbowAPI() {
            this->rainbowLoopEnabled = false;
            MainServer::ReturnOK();
        }
        
        /***
        * HTTP JSON API to set a color
        */
        void stripSetColorAPI() {
            String newColor = MainServer::server.arg("color");
            newColor.replace("#","");
            
            long number = strtol( newColor.c_str(), NULL, 16);
            this->red = number >> 16;
            this->green = number >> 8 & 0xFF;
            this->blue = number & 0xFF; 
            
            this->rainbowLoopEnabled = false;
            
            this->driver->begin();
            this->driver->SetColor(this->red, this->green, this->blue);
            this->driver->end();
            
            MainServer::ReturnOK();
        }
        
        /***
        * HTTP JSON API to get current color (HTML format without #)
        */
        void stripGetColorAPI() {
            String JSONoutput = "";
            JSONoutput += "{ \"color\": \"";
            JSONoutput += ((this->red<16)?"0":"")   + String(this->red, HEX) + 
                ((this->green<16)?"0":"") + String(this->green, HEX) + 
                ((this->blue<16)?"0":"")  + String(this->blue, HEX);
            JSONoutput += "\" }\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }
        
        /***
        * App section for brick main web page
        */
        String mainWebPage(String actualPage) {
            actualPage.replace("<!-- %%APP_ZONE%% -->", SpiFfs::readFile("/app_strip.html"));   
            actualPage.replace("%%COLOR%%", ((this->red<16)?"0":"")   + String(this->red, HEX) + 
                ((this->green<16)?"0":"") + String(this->green, HEX) + 
                ((this->blue<16)?"0":"")  + String(this->blue, HEX));    // Replace with actual color (HTML format without #)
            return actualPage;
        }
};
