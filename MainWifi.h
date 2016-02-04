#define WIFI_RETRY_MAX 120      // 120 tries with 250ms interval = 30 seconds
#define WIFI_RETRY_DELAY 250
#define HOST "36brick"

namespace MainWifi {
    
    bool ok = false;
    const char APPSK[] = "troissix";            // TODO : remove AP password

    /**
    * Setups a Wifi acces point
    */
    void setupAP() {
        Log::Logln("[NFO] Starting WIFI Acces Point !");
        WiFi.mode(WIFI_AP);
        
        // Creates AP name using brick type and mac address. ie: "36Brick MultiSensor 36A36A"
        uint8_t mac[WL_MAC_ADDR_LENGTH];
        WiFi.softAPmacAddress(mac);
        String macID = String(mac[WL_MAC_ADDR_LENGTH - 3], HEX) +
                        String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                        String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
        macID.toUpperCase();
        
        String AP_NameString = "36Brick ";
        AP_NameString += BRICK_TYPE;
        AP_NameString += " " + macID;
        
        char AP_NameChar[AP_NameString.length() + 1];
        memset(AP_NameChar, 0, AP_NameString.length() + 1);
        for (int i=0; i<AP_NameString.length(); i++)
            AP_NameChar[i] = AP_NameString.charAt(i);
        
        Log::Logln("[NFO] AP : " + AP_NameString);
        WiFi.softAP(AP_NameChar, APPSK);      // Starts the Wifi AP
        
        delay(2000);                              // Waits AP to fully start
        yield();
        Log::Logln("[NFO] Brick IP : " + WiFi.softAPIP().toString());
    }
    
    /**
    * Setups the ESP to connect to user wifi
    */
    void setup() {
        WiFi.mode(WIFI_STA);
        if (Settings::ssidOK && Settings::passOK) {
            MainWifi::ok = true;
            Log::Logln("[NFO] Connecting WIFI to " + String(Settings::retreivedSSID.ssid) + " / " + String(Settings::retreivedPASS.pass));
        
            WiFi.begin(Settings::retreivedSSID.ssid, Settings::retreivedPASS.pass);
            int nbRetries = 0;
            while (WiFi.status() != WL_CONNECTED) {
                nbRetries++;
                delay(WIFI_RETRY_DELAY);
                if (nbRetries > WIFI_RETRY_MAX) {
                    MainWifi::ok = false;
                    break;
                }
            }
        
            if (MainWifi::ok) {
                MDNS.begin(HOST);
                Log::Logln("[NFO] WiFi connected ! ");
                Log::Logln("[NFO] IP : " + WiFi.localIP().toString());
                Log::Logln("[NFO] HOSTNAME : " + String(HOST));
                Log::Logln("[NFO] HTTP Server started");
            } else {
                Log::Logln("[ERR] WiFi not connected, starting AP !");
                MainWifi::setupAP(); 
            }
        } else {
            Log::Logln("[ERR] WiFi settings not set, starting standalone AP !");
            MainWifi::setupAP(); 
        }
    }
    

}

