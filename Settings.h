/***
 * EEPROM : Functions to read and save configuration to EEPROM
 */

namespace Settings {

    int eepromSSIDOffset = 0;         // Eeprom offset of Wifi ssid setting
    int eepromSSIDValidOffset = 54;   // Eeprom offset of Wifi ssid validation string
    
    int eepromPASSOffset = 64;        // Eeprom offset of Wifi pass setting
    int eepromPASSValidOffset = 118;  // Eeprom offset of Wifi pass validation string
    
    int eepromNameOffset = 128;       // Eeprom offset of brick name setting
    int eepromNameValidOffset = 182;  // Eeprom offset of brick name validation string
    
    struct SaveSSID { char ssid[48]; }; bool ssidOK;
    struct SavePASS { char pass[48]; }; bool passOK;
    struct SaveName { char name[48]; }; bool nameOK;
    
    SaveSSID retreivedSSID;
    SavePASS retreivedPASS;
    SaveName retreivedName;
    
    #if defined(OPTION_MQTT)
      int eepromMQTTservOffset = 192;       // Eeprom offset of MQTT serv setting
      int eepromMQTTservValidOffset = 246;  // Eeprom offset of MQTT serv validation string
      
      int eepromMQTTportOffset = 256;       // Eeprom offset of MQTT port setting
      int eepromMQTTportValidOffset = 310;  // Eeprom offset of MQTT port validation string
      
      int eepromMQTTenabledOffset = 320;       // Eeprom offset of MQTT enabled setting
      int eepromMQTTenabledValidOffset = 352;  // Eeprom offset of MQTT enabled validation string
      
      struct SaveMQTTserv     { char serv[48]; }; bool servOK;
      struct SaveMQTTport     { char port[10]; }; bool portOK;
      struct SaveMQTTenabled  { bool enabled;  }; bool enabledOK;
      
      Settings::SaveMQTTserv    retreivedMQTTserv;
      Settings::SaveMQTTport    retreivedMQTTport;
      Settings::SaveMQTTenabled retreivedMQTTenabled;
    #endif
    
    int eepromLength = 364;                 // Eeprom total used size
    
    struct SaveValid { char valid[5]; };
    const char* saveValid = "true";
    
    
    /***
     * Writes Brick name, Wifi SSID and Wifi password to EEPROM
     */
    void setSSID_PASS_NAME(String ssid, String pass, String name) {
        SaveSSID newSSID;
        SavePASS newPASS;
        SaveName newName;
        SaveValid SSIDValid;
        SaveValid PASSValid;
        SaveValid NameValid;
        
        strcpy(PASSValid.valid, Settings::saveValid);
        strcpy(newPASS.pass, pass.c_str());                             // -- New PASS
        strcpy(SSIDValid.valid, Settings::saveValid);
        strcpy(newSSID.ssid, ssid.c_str());                             // -- New SSID
        strcpy(NameValid.valid, Settings::saveValid);
        strcpy(newName.name, name.c_str());                             // -- New SSID
        
        EEPROM.begin(eepromLength);                                     // -- Open EEPROM fs
        
        EEPROM.put(Settings::eepromPASSOffset, newPASS);                          // -- Write new pass to fs
        EEPROM.put(Settings::eepromPASSValidOffset, PASSValid);                   // -- Write new pass validation to fs
        EEPROM.put(Settings::eepromSSIDOffset, newSSID);                          // -- Write new SSID to fs
        EEPROM.put(Settings::eepromSSIDValidOffset, SSIDValid);                   // -- Write new SSID validation to fs
        EEPROM.put(Settings::eepromNameOffset, newName);                          // -- Write new Name to fs
        EEPROM.put(Settings::eepromNameValidOffset, NameValid);                   // -- Write new Name validation to fs
        
        EEPROM.commit();                                                // -- Commit edit
        EEPROM.end();                                                   // -- Close EEPROM fs
        Log::Logln("[NFO] New WIFI SSID, WIFI Password and Brick name saved");
    }
    
    #if defined(OPTION_MQTT)
    /***
     * Writes MQTT server, port and activation to EEPROM
     */
      void setMQTT(String serv, String port, bool enabled) {
          SaveMQTTserv newServ;
          SaveMQTTport newPort;
          SaveMQTTenabled newEnabled;
          SaveValid ServValid;
          SaveValid PortValid;
          SaveValid EnabledValid;
          
          strcpy(ServValid.valid, saveValid);
          strcpy(newServ.serv, serv.c_str());                             // -- New Serv
          
          strcpy(PortValid.valid, saveValid);
          strcpy(newPort.port, port.c_str());                             // -- New Port
          
          strcpy(EnabledValid.valid, saveValid);
          newEnabled.enabled = enabled;                                   // -- New activation
          
          EEPROM.begin(eepromLength);                                     // -- Open EEPROM fs
          
          EEPROM.put(Settings::eepromMQTTservOffset, newServ);                      // -- Write new Serv to fs
          EEPROM.put(Settings::eepromMQTTservValidOffset, ServValid);               // -- Write new Serv validation to fs
          
          EEPROM.put(Settings::eepromMQTTportOffset, newPort);                      // -- Write new Port to fs
          EEPROM.put(Settings::eepromMQTTportValidOffset, PortValid);               // -- Write new Port validation to fs
          
          EEPROM.put(Settings::eepromMQTTenabledOffset, newEnabled);                // -- Write new activation to fs
          EEPROM.put(Settings::eepromMQTTenabledValidOffset, EnabledValid);         // -- Write new activation validation to fs
          
          EEPROM.commit();                                                // -- Commit edit
          EEPROM.end();                                                   // -- Close EEPROM fs
          Log::Logln("[NFO] New MQTT settings saved");
      }
    
    #endif
    
    /***
     * Read full configuration from EEPROM
     */
    void read() {
      Log::Logln("[NFO] Reading stored configuration");
      
      SaveValid SSIDValid, PASSValid, NameValid;
      EEPROM.begin(Settings::eepromLength);                                 // -- Open EEPROM fs
      
      EEPROM.get(Settings::eepromSSIDValidOffset, SSIDValid );              // -- Read SSID validation string
      if (strcmp(SSIDValid.valid, "true") == 0) {
          EEPROM.get(Settings::eepromSSIDOffset, Settings::retreivedSSID );           // -- Read stored SSID
          Log::Logln("[NFO] Retreived SSID : " + String(Settings::retreivedSSID.ssid) );
          Settings::ssidOK = true;
      } else {
          Log::Logln("[ERR] No SSID defined !");
          Settings::ssidOK = false;
      }
      
      EEPROM.get(Settings::eepromPASSValidOffset, PASSValid );              // -- Read Pass validation string
      if (strcmp(PASSValid.valid, "true") == 0) {
          EEPROM.get(Settings::eepromPASSOffset, Settings::retreivedPASS );           // -- Read stored password
          Log::Logln("[NFO] Retreived PASS : " + String(Settings::retreivedPASS.pass) );
          Settings::passOK = true;
      } else {
          Log::Logln("[ERR] No password defined !");
          Settings::passOK = false;
      }
    
      EEPROM.get(Settings::eepromNameValidOffset, NameValid );              // -- Read name validation string
      if (strcmp(NameValid.valid, "true") == 0) {
          EEPROM.get(Settings::eepromNameOffset, Settings::retreivedName );           // -- Read stored name
          Log::Logln("[NFO] Retreived Name : " + String(Settings::retreivedName.name));
          Settings::nameOK = true;
      } else {
          Log::Logln("[ERR] No name defined ! Use brick type instead !");
          strcat(Settings::retreivedName.name, BRICK_TYPE);
          Settings::nameOK = false;
      }
    
    #if defined(OPTION_MQTT)
      SaveValid MQTTservValid, MQTTportValid, MQTTenabledValid;
    
      EEPROM.get(Settings::eepromMQTTservValidOffset, MQTTservValid );      // -- Read MQTT Server validation string
      if (strcmp(MQTTservValid.valid, "true") == 0) {
          EEPROM.get(Settings::eepromMQTTservOffset, Settings::retreivedMQTTserv );   // -- Read stored MQTT server
          Log::Logln("[NFO] Retreived MQTTserv : " + String(Settings::retreivedMQTTserv.serv));
          Settings::servOK = true;
      } else {
          Log::Logln("[ERR] No MQTTserv defined !");
          Settings::servOK = false;
      }
    
      EEPROM.get(Settings::eepromMQTTportValidOffset, MQTTportValid );      // -- Read MQTT Port validation string
      if (strcmp(MQTTportValid.valid, "true") == 0) {
          EEPROM.get(Settings::eepromMQTTportOffset, Settings::retreivedMQTTport );   // -- Read stored MQTT port
          Log::Logln("[NFO] Retreived MQTTport : " + String(Settings::retreivedMQTTport.port));
          Settings::portOK = true;
      } else {
          Log::Logln("[ERR] No MQTTport defined !");
          Settings::portOK = false;
      }
      
      EEPROM.get(Settings::eepromMQTTenabledValidOffset, MQTTenabledValid );    // -- Read MQTT Port validation string
      if (strcmp(MQTTenabledValid.valid, "true") == 0) {
          EEPROM.get(Settings::eepromMQTTenabledOffset, Settings::retreivedMQTTenabled );    // -- Read stored MQTT port
          Log::Logln("[NFO] Retreived MQTTenabled : " + String(((Settings::retreivedMQTTenabled.enabled==true)?"True":"False")));
          Settings::enabledOK = true;
      } else {
          Log::Logln("[ERR] No MQTTenabled defined !");
          Settings::enabledOK = false;
      }
    #endif
    
      EEPROM.end();                                               // -- Close EEPROM fs
    
    }
}

