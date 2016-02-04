/**
 * Main HTTP server
 */
 
// TODO : Add NTP and timezone configuration to brick config

#define HTTP_API_PORT 80

namespace MainServer {

    ESP8266WebServer server(HTTP_API_PORT);

    /***
    * Brick main web page, builds a page with a section for each enabled module
    */
    void httpMainWebPage() {
        String HTMLoutput = SpiFfs::readFile("/header.html") + SpiFfs::readFile("/app.html");      
        HTMLoutput.replace("%%TYPE%%",BRICK_TYPE);              // Replace brick type in template
        
        for (Module *r = listModules; r; r = r->nextModule)     // main web page section of all instantiated modules
            HTMLoutput = r->mainWebPage(HTMLoutput);

        long tBefore = millis();
        MainServer::server.sendContent(HTMLoutput.c_str());
        Log::Logln("[NFO] Served Main Web Page " + String(HTMLoutput.length()) + "b in " + String(millis()-tBefore) + "ms");
    }
    
    /***
     * JSON OK answer
     */
    void ReturnOK() {
        MainServer::server.send(200, "application/json", "{ \"ret\": \"OK\" }\r\n");
    }
    
    /**
    * Serves the setup page
    */
    // TODO : Hide MQTT config block when OPTION_MQTT not defined 
    void WifiSetupPage() {
        Log::Logln("[NFO] Serving setup page ...");
        String SetupPage = SpiFfs::readFile("/header.html") + SpiFfs::readFile("/setup.html");      // Setup page template
    
        // Fill the select box with available networks
        int n = WiFi.scanNetworks();        // Scans networks
        String opt = "";
        for (int i = 0; i < n; ++i) {
            opt += "<option value='";
            opt += WiFi.SSID(i);
            opt += "'";
            opt += strcmp(WiFi.SSID(i).c_str(), Settings::retreivedSSID.ssid)?"":" selected='selected' ";
            opt += ">";
            opt += WiFi.SSID(i);
            opt += " (";
            opt += WiFi.RSSI(i);
            opt += "db";
            opt += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?"":" / PSK";
            opt += ")</option>";
            yield();
        }
        SetupPage.replace("%%TYPE%%",BRICK_TYPE);                       // Replace brick type in template
        SetupPage.replace("%%SSID%%",opt);                              // Replace network list in template
        SetupPage.replace("%%PASS%%",Settings::retreivedPASS.pass);     // Replace Wifi password in template
        SetupPage.replace("%%NAME%%",Settings::retreivedName.name);     // Replace brick name in template
        #if defined(OPTION_MQTT)
            SetupPage.replace("%%MQTT_SERV%%",Settings::retreivedMQTTserv.serv);                    // Replace MQTT server adress in template
            SetupPage.replace("%%MQTT_PORT%%",String(Settings::retreivedMQTTport.port));            // Replace MQTT server port in template
            SetupPage.replace("%%MQTT_ENABLED%%", ((Settings::retreivedMQTTenabled.enabled)?" selected='selected'":""));  // Select enabled ...
            SetupPage.replace("%%MQTT_DISABLED%%",((Settings::retreivedMQTTenabled.enabled)?"":" selected='selected'"));  // ... or disabled in MQTT select
        #endif

        long tBefore = millis();
        MainServer::server.sendContent(SetupPage.c_str());
        Log::Logln("[NFO] Served Setup page " + String(SetupPage.length()) + "b in " + String(millis()-tBefore) + "ms");
    }
    
    /**
     * Setup page Form destination : saves the config
     */
    void SaveWifiSetup() {
        Log::Logln("[NFO] Setup form submitted");
        
        String HTMLoutput = SpiFfs::readFile("/header.html") + SpiFfs::readFile("/setup-thx.html");      // Thank you page template
        HTMLoutput.replace("%%TYPE%%",BRICK_TYPE);    // Replace brick type in template
        
        String newName = server.arg("name");          // Parse brick name
        String newSSID = server.arg("ssid");          // Parse wifi ssid
        String newPASS = server.arg("pass");          // Parse wifi password
        String newMQTTServ = server.arg("mqttserv");  // Parse MQTT server adress
        String newMQTTPort = server.arg("mqttport");  // Parse MQTT server port
        String newMQTTEnab = server.arg("mqttenab");  // Parse MQTT activation
        yield();  
        
        Log::Logln("[EVT] New Name defined : "+newName);
        Log::Logln("[EVT] New SSID defined : "+newSSID);
        Log::Logln("[EVT] New Password defined : "+newPASS);
        Settings::setSSID_PASS_NAME(newSSID, newPASS, newName); yield();    // Saves name, wifi ssid and wifi pass to eeprom
      
        #if defined(OPTION_MQTT)
            Log::Logln("[EVT] New MQTT Server defined : "+newMQTTServ);
            Log::Logln("[EVT] New MQTT Port defined : "+newMQTTPort);
            Log::Logln("[EVT] New MQTT Activation defined : "+String((newMQTTEnab=="1")?"True":"False"));
            Settings::setMQTT(newMQTTServ, newMQTTPort, ((newMQTTEnab=="1")? true : false));yield();                // Saves MQTT settings to eeprom
        #endif  
      
        long tBefore = millis();
        MainServer::server.sendContent(HTMLoutput.c_str());
        Log::Logln("[NFO] Served Settings saved page " + String(HTMLoutput.length()) + "b in " + String(millis()-tBefore) + "ms");
    }
     
    /***
     * Serves a file directly from SPIFFS
     * CAUTION : ressources served this way have to be gziped
     */
    bool serveFromSpiffs(String path) {
        if(path.endsWith("/")) path += "index.html";
    
        File in = SPIFFS.open(path.c_str(), "r");
        if (!in) {
            Log::Logln("[ERR] Unable to find '" + path + "' in SPIFFS");
            return false;
        }
        
        in.setTimeout(0);
        long len = in.size();
        String dataType = "text/plain";
    
        if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
        else if(path.endsWith(".htm")) dataType = "text/html";
        else if(path.endsWith(".css")) dataType = "text/css";
        else if(path.endsWith(".js"))  dataType = "application/javascript";
        else if(path.endsWith(".png")) dataType = "image/png";
        else if(path.endsWith(".gif")) dataType = "image/gif";
        else if(path.endsWith(".jpg")) dataType = "image/jpeg";
        else if(path.endsWith(".ico")) dataType = "image/x-icon";
        else if(path.endsWith(".xml")) dataType = "text/xml";
        else if(path.endsWith(".pdf")) dataType = "application/pdf";
        else if(path.endsWith(".zip")) dataType = "application/zip";
     
        MainServer::server.sendHeader("Cache-Control", " max-age=581097");      // Send cache duration
        MainServer::server.sendHeader("Content-Encoding", "gzip");              // Send gzip header (every file served directly MUST be gziped)
        
        long tBefore = millis();
        long sent = server.streamFile(in, dataType);
        if ( sent != len) {
            in.close();
            Log::Logln("[ERR] Sent less data than expected : " + String(sent) + "/" + String(len) );
            return false;
        }
        
        in.close();
        Log::Logln("[NFO] Served " + String(path) + " from SPIFFS " + String(len) + "b in " + String(millis()-tBefore) + "ms");
        return true;
    }

    /***
     * Handle all undefined endpoints, even resource files directly stored on flash
     * 404 page filled with system informations
     */
    void handleNotFound() {
        // try to find the file in the flash
        if(MainServer::serveFromSpiffs(server.uri())) return;
        
        String message = "File Not Found\n\n";
        message += "URI..........: ";
        message += server.uri();
        message += "\nMethod.....: ";
        message += (server.method() == HTTP_GET)?"GET":"POST";
        message += "\nArguments..: ";
        message += server.args();
        message += "\n";
        for (uint8_t i=0; i<server.args(); i++){
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
        }
        message += "\n";
        message += "FreeHeap.....: " + String(ESP.getFreeHeap()) + "\n";
        message += "ChipID.......: " + String(ESP.getChipId()) + "\n";
        message += "FlashChipId..: " + String(ESP.getFlashChipId()) + "\n";
        message += "FlashChipSize: " + String(ESP.getFlashChipSize()) + " bytes\n";
        message += "getCycleCount: " + String(ESP.getCycleCount()) + " Cycles\n";
        message += "UpTime.......: " + String(millis()/1000) + " Seconds\n";
        MainServer::server.send(404, "text/plain", message);
    }

     /**
     * main web server setup : declare HTTP API endpoints
     */
    void setup() {
        server.on("/", MainServer::httpMainWebPage);    // Brick main app page, built from each module app section
        server.on("/setup", MainServer::WifiSetupPage);         // Setup web page
        server.on("/saveCFG", MainServer::SaveWifiSetup);                   // Setup form destination
        server.on("/reset", []() {                              // Resets the brick
            MainServer::server.send( 200, "text/plain", "Resetting..." );
            ESP.restart();
        });
        
        server.on("/isBrick", []() {                            // To know if this is a brick, used by Android app brick discovery
            String JSONoutput = "";
            JSONoutput += "{ \"brickType\": \"";
            JSONoutput += BRICK_TYPE;
            JSONoutput += "\", \"brickName\": \"";
            JSONoutput += String(Settings::retreivedName.name);
            JSONoutput += "\", \"brickVersion\": \"";
            JSONoutput += FIRMWARE_VERSION;
            JSONoutput += "\" }\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        });
      
        server.on("/firmware", HTTP_GET, [](){                  // Firmware update web page
            server.sendHeader("Connection", "close");
            server.sendHeader("Access-Control-Allow-Origin", "*");
            String firmPage = SpiFfs::readFile("/header.html") + SpiFfs::readFile("/firmware-update.html");  // Firmware update page
            String firmVersion = String(FIRMWARE_VERSION) + " for " + String(BRICK_TYPE);
            firmPage.replace("%%VERSION%%",firmVersion); // Replace firmware version in template

            long tBefore = millis();
            MainServer::server.sendContent(firmPage.c_str());
            Log::Logln("[NFO] Served Firmware update page " + String(firmPage.length()) + "b in " + String(millis()-tBefore) + "ms");
       });
        
        // Firmware update file upload (form destination)
        server.onFileUpload([](){
            if(server.uri() != "/update") return;
            HTTPUpload& upload = server.upload();
            if(upload.status == UPLOAD_FILE_START){
                Serial.setDebugOutput(true);
                WiFiUDP::stopAll();
                uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                Log::Logln("[NFO] Update : " + String(upload.filename) + " - Max = " + String(maxSketchSpace));
                if(!Update.begin(maxSketchSpace)){//start with max available size
                    Update.printError(Serial);
                }
            } else if(upload.status == UPLOAD_FILE_WRITE){
                if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
                    Update.printError(Serial);
                }
            } else if(upload.status == UPLOAD_FILE_END){
                if(Update.end(true)){ //true to set the size to the current progress
                    Log::Logln("[NFO] Update Success : " + String(upload.totalSize));
                    Log::Logln("[NFO] Rebooting ...");
                } else {
                    Update.printError(Serial);
                }
                Serial.setDebugOutput(false);
            }
            yield();
        });
        
        // Firmware update form destination (processed after file upload)
        server.on("/update", HTTP_POST, [](){
            MainServer::server.sendHeader("Connection", "close");
            MainServer::server.sendHeader("Access-Control-Allow-Origin", "*");
            String firmPage = SpiFfs::readFile("/header.html") + SpiFfs::readFile("/firmware-update-complete.html");  // Firmware update complete page
            MainServer::server.send(200, "text/html", firmPage); 
            ESP.restart();
        });

        server.onNotFound(handleNotFound);  // Handle all other files (from flash) and 404
        server.begin();                     // Starts the web server to handle all HTTP requests
        MDNS.addService("http", "tcp", HTTP_API_PORT);

    }

}

