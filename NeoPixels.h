/***
 * NEOPIXELS : RGB Pixels module, drives up to 1024 adressable rgb leds from one single output pin
 */

 // TODO : HTTP API endpoint to set individual pixels
 
class neoPixelsModule : public Module {
    private:
        int inputPin;
        Adafruit_NeoPixel *NeoPixels;
        bool rainbowLoopEnabled = true;
        long rainbowLastUpdt = 0;
        uint16_t rainbowColorIndex = 0;

    public:
        neoPixelsModule(int inputPin) {
            this->inputPin = inputPin;
            NeoPixels = new Adafruit_NeoPixel(NEOPIXELS_NUM, this->inputPin, NEO_GRB + NEO_KHZ800);
        }
        
        /***
        * NEOPIXELS module setup : initialize NeoPixels lib, and declare HTTP API endpoints
        */
        void setup() {
            Log::Logln("[NFO] NeoPixels initialization");
            NeoPixels->begin();
            MainServer::server.on("/pixels/rainbow/on", (std::bind(&neoPixelsModule::enableRainbowAPI, this)));
            MainServer::server.on("/pixels/rainbow/off", (std::bind(&neoPixelsModule::disableRainbowAPI, this)));
        }
        
        /***
        * NEOPIXELS main loop
        */
        void loop() {
            rainbowLoop();
        }
        
        /***
        * NEOPIXELS rainbow loop : loops through colors 
        */
        void rainbowLoop() {
            if (this->rainbowLoopEnabled) {
                long now = millis();
                if (now - this->rainbowLastUpdt > NEOPIXELS_RAINBOW_INTERVAL_MS) {
                    this->rainbowLastUpdt = now; 
                    for(uint16_t i = 0; i < NeoPixels->numPixels(); i++) {
                        NeoPixels->setPixelColor(i, this->Wheel(((i * 256 / NeoPixels->numPixels()) + this->rainbowColorIndex) & 255));
                    }
                    NeoPixels->show();
                    this->rainbowColorIndex++;
                    if (this->rainbowColorIndex >= (256*5)) this->rainbowColorIndex=0;
                }
            }
        }
        
        /***
        * HTTP JSON API to enable rainbow mode
        */
        void enableRainbowAPI() {
            this->rainbowLoopEnabled = true;
            MainServer::ReturnOK();
        }
        
        /***
        * HTTP JSON API to disable rainbow mode
        */
        void disableRainbowAPI() {
            this->rainbowLoopEnabled = false;
            MainServer::ReturnOK();
        }
        
        /***
        * Converts a single position (0-255) to NeoPixels RGB color 
        */
        uint32_t Wheel(byte WheelPos) {
            WheelPos = 255 - WheelPos;
            if(WheelPos < 85)  
                return NeoPixels->Color(255 - WheelPos * 3, 0, WheelPos * 3);
            else if(WheelPos < 170) {
                WheelPos -= 85;
                return NeoPixels->Color(0, WheelPos * 3, 255 - WheelPos * 3);
            } else {
                WheelPos -= 170;
                return NeoPixels->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
            }
        }
};

