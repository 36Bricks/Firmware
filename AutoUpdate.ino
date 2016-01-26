#if defined(OPTION_AUTO_UPDATE)     
    #include <ESP8266HTTPClient.h>
    #include <ESP8266httpUpdate.h>
    
    #define AUTO_UPDATE_URL  "http://192.168.2.8/bricks/update.php"
    
    void checkForAutoUpdate() {
        if (wifiOK) {
            Logln("[NFO] Strarting auto update for SPIFFS, please wait ...");
            switch(ESPhttpUpdate.updateSpiffs(AUTO_UPDATE_URL, FIRMWARE_VERSION)) {
                case HTTP_UPDATE_FAILED:
                    Logln("[NFO] Auto update failed : " + String(ESPhttpUpdate.getLastErrorString().c_str()));
                    break;
        
                case HTTP_UPDATE_NO_UPDATES:
                    Logln("[NFO] Auto update not needed");
                    break;
        
                case HTTP_UPDATE_OK:
                    Logln("[NFO] Auto update successfull");
                    break;
            }
            
            Logln("[NFO] Strarting auto update for Flash, will reboot if needed ! ");
            switch(ESPhttpUpdate.update(AUTO_UPDATE_URL, FIRMWARE_VERSION)) {
                case HTTP_UPDATE_FAILED:
                    Logln("[NFO] Auto update failed : " + String(ESPhttpUpdate.getLastErrorString().c_str()));
                    break;
        
                case HTTP_UPDATE_NO_UPDATES:
                    Logln("[NFO] Auto update not needed");
                    break;
        
                case HTTP_UPDATE_OK:
                    Logln("[NFO] Auto update successfull");
                    break;
            }
        } else {
            Logln("[NFO] Auto update skipped");
        }
    }
#endif
