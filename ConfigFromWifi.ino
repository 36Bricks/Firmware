//-----------------------------------------------------------
//--------------- CONFIGURATION PAR WIFI --------------------
//-----------------------------------------------------------
//-- Gestion de la configuration du module par le wifi.    --
//-----------------------------------------------------------
//-- setupWiFiAP() - Démarre un AP Wifi si pas connecté à  --
//-- un Wifi (Premier démarrage ou WIFI injoignable.       --
//-----------------------------------------------------------
//-- WifiSetupPage() - Sert la page de config Wifi et MQTT --
//-----------------------------------------------------------
//-- SaveWifiSetup() - Retour du form de config, enregistr --
//-- ement de la config en EEPROM                          --
//-----------------------------------------------------------
// TODO : Enlever le mot de passe au AP                    --
//-----------------------------------------------------------

const char WiFiAPPSK[] = "sparkfun";

void setupConfigFromWifi() {
  server.on("/setup", WifiSetupPage);
  server.on("/saveCFG", SaveWifiSetup);
  server.on("/reset", []() {
    server.send ( 200, "text/plain", "Resetting..." );
    ESP.restart();
  });
  
}

void setupWiFiAP() {
  Logln("[NFO] Starting WIFI Acces Point !");
  WiFi.mode(WIFI_AP);

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
  WiFi.softAP(AP_NameChar, WiFiAPPSK);

  server.begin();
  yield();
  delay(2000);
  yield();
  Serial.print("[NFO] IP : ");
  Serial.println(WiFi.softAPIP());
}

void WifiSetupPage() {
    Logln("[NFO] Served setup page");
    int n = WiFi.scanNetworks();
      
    String SetupPage = ConfigPage;

    // Fill select with networks
    String opt = "";
    for (int i = 0; i < n; ++i) {
        opt += "<option value='";
        opt += WiFi.SSID(i);
        opt += "'>";
        opt += WiFi.SSID(i);
        opt += " (";
        opt += WiFi.RSSI(i);
        opt += "db)";
        opt += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
        opt += "</option>";
        yield();
    }
    SetupPage.replace("%%TYPE%%",BRICK_TYPE);
    SetupPage.replace("%%SSID%%",opt);
    SetupPage.replace("%%PASS%%",retreivedPASS.pass);
    SetupPage.replace("%%NAME%%",retreivedName.name);
  #if defined(OPTION_MQTT)
    SetupPage.replace("%%MQTT_SERV%%",retreivedMQTTserv.serv);
    SetupPage.replace("%%MQTT_PORT%%",String(retreivedMQTTport.port));
  #endif
    server.send(200, "text/html", SetupPage);
}


void SaveWifiSetup() {
  Logln("[NFO] Setup form submitted");
  
  String HTMLoutput = ThxPage;
  HTMLoutput.replace("%%TYPE%%",BRICK_TYPE);

  String newName = server.arg("name");
  String newSSID = server.arg("ssid");
  String newPASS = server.arg("pass");
  String newMQTTServ = server.arg("mqttserv");
  String newMQTTPort = server.arg("mqttport");
  yield();  

  Logln("[EVT] New Name defined : "+newName);
  Logln("[EVT] New SSID defined : "+newSSID);
  Logln("[EVT] New Password defined : "+newPASS);
  SetSSID_PASS_NAME(newSSID, newPASS, newName); yield();
  
#if defined(OPTION_MQTT)
  Logln("[EVT] New MQTT Server defined : "+newMQTTServ);
  Logln("[EVT] New MQTT Port defined : "+newMQTTPort);
  SetMQTT(newMQTTServ, newMQTTPort);yield();
#endif  
  
  server.send(200, "text/html", HTMLoutput);
}


