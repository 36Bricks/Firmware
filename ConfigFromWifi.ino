/***
 *  ConfigFromWifi : Manages configuration page through wifi.
 *  
 *  Like Chromecast, if there is no Wifi configuration defined
 *  (or if it can't connect to the defined Wifi SSID), it creates
 *  its own Wifi access point to allow easy configuration.
 *  
 *  Config web page is always available, even when not on AP mode. 
 */

// TODO : Add a switch to enable/disable MQTT option
// TODO : Add NTP and timezone configuration

const char WiFiAPPSK[] = "troissix";            // TODO : remove AP password

/**
 * ConfigFromWifi setup : declare HTTP API endpoints
 */
void setupConfigFromWifi() {
  server.on("/setup", WifiSetupPage);                     // Setup web page
  server.on("/saveCFG", SaveWifiSetup);                   // Setup form destination
  server.on("/reset", []() {                              // Resets the brick
    server.send( 200, "text/plain", "Resetting..." );
    ESP.restart();
  });
  server.on("/isBrick", []() {                            // To know if this is a brick, used by Android app brick discovery
      String JSONoutput = "";
      JSONoutput += "{ \"brickType\": \"";
      JSONoutput += BRICK_TYPE;
      JSONoutput += "\", \"brickName\": \"";
      JSONoutput += String(retreivedName.name);
      JSONoutput += "\", \"brickVersion\": \"";
      JSONoutput += FIRMWARE_VERSION;
      JSONoutput += "\" }\r\n";
      server.send(200, "application/json", JSONoutput);
  });
}

/**
 * Serves the setup page
 */
void WifiSetupPage() {
    Logln("[NFO] Served setup page");
    String SetupPage = readFromSpiffs("/header.html") + readFromSpiffs("/setup.html");      // Setup page template

    // Fill the select box with available networks
    int n = WiFi.scanNetworks();        // Scans networks
    String opt = "";
    for (int i = 0; i < n; ++i) {
        opt += "<option value='";
        opt += WiFi.SSID(i);
        opt += "'";
        opt += strcmp(WiFi.SSID(i).c_str(),retreivedSSID.ssid)?"":" selected='selected' ";
        opt += ">";
        opt += WiFi.SSID(i);
        opt += " (";
        opt += WiFi.RSSI(i);
        opt += "db";
        opt += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?"":" / PSK";
        opt += ")</option>";
        yield();
    }
    SetupPage.replace("%%TYPE%%",BRICK_TYPE);             // Replace brick type in template
    SetupPage.replace("%%SSID%%",opt);                    // Replace network list in template
    SetupPage.replace("%%PASS%%",retreivedPASS.pass);     // Replace Wifi password in template
    SetupPage.replace("%%NAME%%",retreivedName.name);     // Replace brick name in template
  #if defined(OPTION_MQTT)
    SetupPage.replace("%%MQTT_SERV%%",retreivedMQTTserv.serv);          // Replace MQTT server adress in template
    SetupPage.replace("%%MQTT_PORT%%",String(retreivedMQTTport.port));  // Replace MQTT server port in template
    SetupPage.replace("%%MQTT_ENABLED%%", ((retreivedMQTTenabled.enabled)?" selected='selected'":""));  // Select enabled ...
    SetupPage.replace("%%MQTT_DISABLED%%",((retreivedMQTTenabled.enabled)?"":" selected='selected'"));  // ... or disabled in MQTT select
  #endif
    server.send(200, "text/html", SetupPage);             // Serv the page as html
}

/**
 * Setup page Form destination : saves the config
 */
void SaveWifiSetup() {
  Logln("[NFO] Setup form submitted");
  
  String HTMLoutput = readFromSpiffs("/header.html") + readFromSpiffs("/setup-thx.html");      // Thank you page template
  HTMLoutput.replace("%%TYPE%%",BRICK_TYPE);    // Replace brick type in template

  String newName = server.arg("name");          // Parse brick name
  String newSSID = server.arg("ssid");          // Parse wifi ssid
  String newPASS = server.arg("pass");          // Parse wifi password
  String newMQTTServ = server.arg("mqttserv");  // Parse MQTT server adress
  String newMQTTPort = server.arg("mqttport");  // Parse MQTT server port
  String newMQTTEnab = server.arg("mqttenab");  // Parse MQTT activation
  yield();  

  Logln("[EVT] New Name defined : "+newName);
  Logln("[EVT] New SSID defined : "+newSSID);
  Logln("[EVT] New Password defined : "+newPASS);
  SetSSID_PASS_NAME(newSSID, newPASS, newName); yield();    // Saves name, wifi ssid and wifi pass to eeprom
  
#if defined(OPTION_MQTT)
  Logln("[EVT] New MQTT Server defined : "+newMQTTServ);
  Logln("[EVT] New MQTT Port defined : "+newMQTTPort);
  Logln("[EVT] New MQTT Activation defined : "+String((newMQTTEnab=="1")?"True":"False"));
  SetMQTT(newMQTTServ, newMQTTPort, ((newMQTTEnab=="1")? true : false));yield();                // Saves MQTT settings to eeprom
#endif  
  
  server.send(200, "text/html", HTMLoutput);    // Serves the thank you page as html
}


