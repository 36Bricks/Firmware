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
            Logln("[NFO] IP : " + WiFi.localIP().toString());
            Logln("[NFO] HTTP Server started ... begin loop");
        } else {
            Logln("[ERR] WiFi not connected, starting AP !");
            setupWiFiAP(); 
        }
    }
}

/**
 * Setups a Wifi acces point
 */
void setupWiFiAP() {
  Logln("[NFO] Starting WIFI Acces Point !");
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
  
  Logln("[NFO] AP : " + AP_NameString);
  WiFi.softAP(AP_NameChar, WiFiAPPSK);      // Starts the Wifi AP

  delay(2000);                              // Waits AP to fully start
  yield();
  Serial.print("[NFO] Brick IP : ");
  Serial.println(WiFi.softAPIP());
}

