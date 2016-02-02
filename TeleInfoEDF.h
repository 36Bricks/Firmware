/***
* TELEINFO_EDF : Module to receive values from domestic EDF electicity counter (France only)
*/

// TODO : Full debug, not tested yet 

#if defined(OPTION_MQTT)
    #define topicEDF "edf/"
#endif

#define startFrame 0x02
#define endFrame 0x03
#define startLine 0x0A
#define endLine 0x0D

#define ADCO 0
#define OPTARIF 1
#define ISOUSC 2
#define BASE 3
#define HCHC 4
#define HCHP 5
#define EJPHN 6
#define EJPHPM 7
#define BBRHCJB 8
#define BBRHPJB 9
#define BBRHCJW 10
#define BBRHPJW 11
#define BBRHCJR 12
#define BBRHPJR 13
#define PEJP 14
#define PTEC 15
#define DEMAIN 16
#define IINST 17
#define IINST1 18
#define IINST2 19
#define IINST3 20
#define IMAX 21
#define IMAX1 22
#define IMAX2 23
#define IMAX3 24
#define PMAX 25
#define PAPP 26
#define HHPHC 27
#define MOTDETAT 28 
#define PPOT 29

#define NBCOMMANDS 30

class teleinfoModule : public Module {
    private:
        int pin;
        SoftwareSerial *cptSerial;
        String commands[NBCOMMANDS] = {"ADCO","OPTARIF","ISOUSC","BASE","HCHC","HCHP","EJPHN","EJPHPM","BBRHCJB","BBRHPJB","BBRHCJW","BBRHPJW","BBRHCJR","BBRHPJR","PEJP","PTEC","DEMAIN","IINST","IINST1","IINST2","IINST3","IMAX","IMAX1","IMAX2","IMAX3","PMAX","PAPP","HHPHC","MOTDETAT","PPOT"};
        String units[NBCOMMANDS] = {"","","A","Wh","Wh","Wh","Wh","Wh","Wh","Wh","Wh","Wh","Wh","Wh","min","","","A","A","A","A","A","A","A","A","W","VA","","",""};
        int lengths[NBCOMMANDS] = {12,4,2,9,9,9,9,9,9,9,9,9,9,9,2,4,4,3,3,3,3,3,3,3,3,5,5,1,6,2};
        String values[NBCOMMANDS];
         
    public:
        teleinfoModule(int pin) {
            this->pin = pin;
            this->cptSerial = new SoftwareSerial(this->pin, SW_SERIAL_UNUSED_PIN, 128);
        }

        void setup() {
            Log::Logln("[NFO] EDF initialization");
            this->cptSerial->begin(1200);
            for (int i=0; i<NBCOMMANDS; i++) {
                this->values[i] = "N/A";
            }
            MainServer::server.on("/edf/status", (std::bind(&teleinfoModule::statusAPI, this)));
        }
        
        void loop() {
            this->decodeNextTeleInfo();
        }
        
        void statusAPI() {
            String JSONoutput = "";
            JSONoutput += "{ ";
            for (int i=0; i<NBCOMMANDS; i++) {
                JSONoutput += "\"";
                JSONoutput += this->commands[i];
                JSONoutput += "\": \"";
                JSONoutput += this->values[i];
                if (i<(NBCOMMANDS-1)) JSONoutput += "\", ";
                else JSONoutput += "\" ";
            }
            JSONoutput += "}\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }
        
        String getTeleInfo() {
            String TeleInfo = "";
            char charIn = 0;
            int maxWait = 100;
            int nbWait = 0;
            while (charIn != startLine) {
                charIn = this->cptSerial->read() & 0x7F;
                if ((charIn != startLine) && ((nbWait++) > maxWait)) {
                    return TeleInfo ;
                }
                else yield();
            }
            while (charIn != endLine) {
                if (this->cptSerial->available() > 0) {
                    charIn = this->cptSerial->read() & 0x7F;
                    TeleInfo += charIn;
                    yield();
                }
            }
            return TeleInfo;
        }
        
        void decodeNextTeleInfo() {
            String data = "";
            String value = "";
            
            int iCommandFound = -1;
            
            data = this->getTeleInfo();     // On lit une data
            
            if (data.length()>2) {
                for (int i=0; i<NBCOMMANDS; i++) { // On la compare à toutes les commandes
                    if (data.substring(0, this->commands[i].length()) == this->commands[i]) { // Si elle correspond à une commande
                        Log::Logln("[EVT] EDF Serial : "+ String(data));
                        int tmp = data.substring((this->commands[iCommandFound].length() + 1),(this->lengths[iCommandFound] + (this->commands[iCommandFound].length() + 1)));
                        if (tmp != this->values[i]) {
                            this->values[i] = tmp;
                            #if defined(OPTION_MQTT)
                                MQTT.publish(String(topicEDF) + String(this->commands[i]), String(this->values[i]).c_str());
                            #endif
                        }
                        break;
                    }
                }
            }
        }
};


