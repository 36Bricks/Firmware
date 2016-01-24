#define WIFI_RETRY_MAX 120      // 120 tries with 250ms interval = 30 seconds
#define WIFI_RETRY_DELAY 250

/**
 * Setups the ESP to connect to user wifi
 */

void setupWifi() {
    MDNS.begin(host);
    if (ssidOK && passOK) {
        wifiOK = true;
        Logln("[NFO] Connecting WIFI to " + String(retreivedSSID.ssid) + " / " + String(retreivedPASS.pass));
    
        WiFi.begin(retreivedSSID.ssid, retreivedPASS.pass);
        int nbRetries = 0;
        while (WiFi.status() != WL_CONNECTED) {
            nbRetries++;
            delay(WIFI_RETRY_DELAY);
            if (nbRetries > WIFI_RETRY_MAX) {
                wifiOK = false;
                break;
            }
        }
    
        if (wifiOK) {
            Logln("[NFO] WiFi connected ! ");
            Logln("[NFO] IP : " + String(WiFi.localIP()));
            Logln("[NFO] HTTP Server started ... begin loop");
        } else {
            Logln("[ERR] WiFi not connected, starting AP !");
            setupWiFiAP(); 
        }
    }
}


