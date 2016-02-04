/**
* SSDP : announce the brick presence on network
*/

class ssdpOption : public Option {
    private:
                
    public:
        void loop() {
        }

        void setup() {
            Log::Logln("[NFO] SSDP initialization");

            MainServer::server.on("/description.xml", HTTP_GET, [](){
                Log::Logln("[EVT] SSDP request");
                SSDP.schema(MainServer::server.client());
            });
    
            SSDP.setSchemaURL("description.xml");
            SSDP.setHTTPPort(80);
            SSDP.setDeviceType("urn:schemas-upnp-org:device:DimmableLight:1");
            SSDP.setName(String(Settings::retreivedName.name) + " (" + WiFi.localIP().toString() + ")");
            SSDP.setURL("/");
            SSDP.setSerialNumber(ESP.getChipId());
            SSDP.setModelName("36Brick " + String(BRICK_TYPE));
            SSDP.setModelNumber(String(FIRMWARE_VERSION));
            SSDP.setModelURL("http://bricks.trois-six.com");
            SSDP.setManufacturer("Trois-Six");
            SSDP.setManufacturerURL("http://www.trois-six.com");
            SSDP.setTTL(255);
            SSDP.begin();
            
        }
};

