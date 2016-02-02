/**
* AutoUpdate : Auto updates SPIFFS and FLASH over internet.
* The script on update server compares firmware versions to
* only update if needed.
*/
// TODO : Add an auto-update on an every-day basis

class autoUpdateOption : public Option {
    private:
                
    public:
        void loop() {}

        void setup() {
            if (MainWifi::ok) {
                Log::Logln("[NFO] Strarting auto update for SPIFFS, please wait ...");
                switch(ESPhttpUpdate.updateSpiffs(AUTO_UPDATE_URL, FIRMWARE_VERSION)) {
                    case HTTP_UPDATE_FAILED:
                        Log::Logln("[NFO] Auto update failed : " + String(ESPhttpUpdate.getLastErrorString().c_str()));
                        break;
            
                    case HTTP_UPDATE_NO_UPDATES:
                        Log::Logln("[NFO] Auto update not needed");
                        break;
            
                    case HTTP_UPDATE_OK:
                        Log::Logln("[NFO] Auto update successfull");
                        break;
                }
                
                Log::Logln("[NFO] Strarting auto update for Flash, will reboot if needed ! ");
                switch(ESPhttpUpdate.update(AUTO_UPDATE_URL, FIRMWARE_VERSION)) {
                    case HTTP_UPDATE_FAILED:
                        Log::Logln("[NFO] Auto update failed : " + String(ESPhttpUpdate.getLastErrorString().c_str()));
                        break;
            
                    case HTTP_UPDATE_NO_UPDATES:
                        Log::Logln("[NFO] Auto update not needed");
                        break;
            
                    case HTTP_UPDATE_OK:
                        Log::Logln("[NFO] Auto update successfull");
                        break;
                }
            } else {
                Log::Logln("[NFO] Auto update skipped");
            }
        }
 };
