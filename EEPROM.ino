void SetSSID_PASS_NAME(String ssid, String pass, String name) {
    SaveSSID newSSID;
    SavePASS newPASS;
    SaveName newName;
    SaveValid SSIDValid;
    SaveValid PASSValid;
    SaveValid NameValid;
    
    strcpy(PASSValid.valid, saveValid);
    strcpy(newPASS.pass, pass.c_str());                             // -- New PASS
    strcpy(SSIDValid.valid, saveValid);
    strcpy(newSSID.ssid, ssid.c_str());                             // -- New SSID
    strcpy(NameValid.valid, saveValid);
    strcpy(newName.name, name.c_str());                             // -- New SSID
    
    EEPROM.begin(eepromLength);                                     // -- Open EEPROM fs
    
    EEPROM.put(eepromPASSOffset, newPASS);                          // -- Write new pass to fs
    EEPROM.put(eepromPASSValidOffset, PASSValid);                   // -- Write new pass validation to fs
    EEPROM.put(eepromSSIDOffset, newSSID);                          // -- Write new SSID to fs
    EEPROM.put(eepromSSIDValidOffset, SSIDValid);                   // -- Write new SSID validation to fs
    EEPROM.put(eepromNameOffset, newName);                          // -- Write new Name to fs
    EEPROM.put(eepromNameValidOffset, NameValid);                   // -- Write new Name validation to fs
    
    EEPROM.commit();                                                // -- Commit edit
    EEPROM.end();                                                   // -- Close EEPROM fs
    Logln("[NFO] New WIFI SSID, WIFI Password and Brick name saved");
}

#if defined(OPTION_MQTT)
  void SetMQTT(String serv, String port) {
      SaveMQTTserv newServ;
      SaveMQTTport newPort;
      SaveValid ServValid;
      SaveValid PortValid;
      
      strcpy(ServValid.valid, saveValid);
      strcpy(newServ.serv, serv.c_str());                             // -- New Serv
      strcpy(PortValid.valid, saveValid);
      strcpy(newPort.port, port.c_str());                             // -- New Port
      
      EEPROM.begin(eepromLength);                                     // -- Open EEPROM fs
      
      EEPROM.put(eepromMQTTservOffset, newServ);                      // -- Write new Serv to fs
      EEPROM.put(eepromMQTTservValidOffset, ServValid);               // -- Write new Serv validation to fs
      EEPROM.put(eepromMQTTportOffset, newPort);                      // -- Write new Port to fs
      EEPROM.put(eepromMQTTportValidOffset, PortValid);               // -- Write new Port validation to fs
      
      EEPROM.commit();                                                // -- Commit edit
      EEPROM.end();                                                   // -- Close EEPROM fs
      Logln("[NFO] New MQTT settings saved");
  }


  void SetMQTTServer(String MQTTServer) { }
  void SetMQTTPort(String MQTTPort) { }
#endif

void ReadStoredConfig() {
  Logln("[NFO] Reading stored configuration");
  
  SaveValid SSIDValid, PASSValid, NameValid;
  
  EEPROM.begin(eepromLength); // -- Open EEPROM fs
  
  EEPROM.get(eepromSSIDValidOffset, SSIDValid ); // -- Read SSID validation string
  if (strcmp(SSIDValid.valid, "true") == 0) {
      EEPROM.get(eepromSSIDOffset, retreivedSSID ); // -- Read stored SSID
      Logln("[NFO] Retreived SSID : " + String(retreivedSSID.ssid) );
      ssidOK = true;
  } else {
      Logln("[ERR] No SSID defined ! Use \"setSSID newSSIDname\" to set one !");
      ssidOK = false;
  }
  
  EEPROM.get(eepromPASSValidOffset, PASSValid ); // -- Read Pass validation string
  if (strcmp(PASSValid.valid, "true") == 0) {
      EEPROM.get(eepromPASSOffset, retreivedPASS ); // -- Read stored password
      Logln("[NFO] Retreived PASS : " + String(retreivedPASS.pass) );
      passOK = true;
  } else {
      Logln("[ERR] No password defined ! Use \"setPASS newPassword\" to set one !");
      passOK = false;
  }

  EEPROM.get(eepromNameValidOffset, NameValid ); // -- Read name validation string
  if (strcmp(NameValid.valid, "true") == 0) {
      EEPROM.get(eepromNameOffset, retreivedName ); // -- Read stored name
      Logln("[NFO] Retreived Name : " + String(retreivedName.name));
      nameOK = true;
  } else {
      Logln("[ERR] No name defined ! Use brick type instead !");
      strcat(retreivedName.name, BRICK_TYPE);
      nameOK = false;
  }

#if defined(OPTION_MQTT)
  SaveValid MQTTservValid, MQTTportValid;

  EEPROM.get(eepromMQTTservValidOffset, MQTTservValid ); // -- Read MQTT Server validation string
  if (strcmp(MQTTservValid.valid, "true") == 0) {
      EEPROM.get(eepromMQTTservOffset, retreivedMQTTserv ); // -- Read stored server
      Logln("[NFO] Retreived MQTTserv : " + String(retreivedMQTTserv.serv));
      servOK = true;
  } else {
      Logln("[ERR] No MQTTserv defined !");
      servOK = false;
  }

  EEPROM.get(eepromMQTTportValidOffset, MQTTportValid ); // -- Read MQTT Port validation string
  if (strcmp(MQTTportValid.valid, "true") == 0) {
      EEPROM.get(eepromMQTTportOffset, retreivedMQTTport ); // -- Read stored port
      Logln("[NFO] Retreived MQTTport : " + String(retreivedMQTTport.port));
      portOK = true;
  } else {
      Logln("[ERR] No MQTTport defined !");
      portOK = false;
  }
#endif

  EEPROM.end(); // -- Close EEPROM fs

}


