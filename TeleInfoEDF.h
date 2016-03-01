/***
* TELEINFO_EDF : Module to receive values from domestic EDF electicity counter (France only)
*/

// TODO : Full debug, not tested yet 
#include <SoftwareSerial.h>             // Software Serial library
#include <LibTeleinfo.h>

#if defined(OPTION_MQTT)
    #define topicEDF "edf/"
#endif

void ADPSCallback(uint8_t phase) {
  if (phase == 0 ) {
    Log::Logln("ADPS");
  }
  else {
    Log::Logln("ADPS PHASE #" + String('0' + phase));
  }
}

void DataCallback(ValueList * me, uint8_t  flags) {
    if (flags & TINFO_FLAGS_ADDED) 
        Log::Logln("NEW -> ");
    
    if (flags & TINFO_FLAGS_UPDATED)
        Log::Logln("MAJ -> ");
    
    Log::Logln(String(me->name)+"="+String(me->value));
    #if defined(OPTION_MQTT)
        MQTT.publish(String(topicEDF) + String(me->name), String(me->value));
    #endif
}

void NewFrame(ValueList * me) {
  Log::Logln("FRAME -> SAME AS PREVIOUS");
}

void UpdatedFrame(ValueList * me) {
  Log::Logln("FRAME -> UPDATED");
}

class teleinfoModule : public Module {
    private:
        int pin;
        SoftwareSerial *cptSerial;
        TInfo          tinfo; // Teleinfo object
         
    public:
        teleinfoModule(int pin) {
            this->pin = pin;
            this->cptSerial = new SoftwareSerial(this->pin, SW_SERIAL_UNUSED_PIN, 128);
        }

        void setup() {
            Log::Logln("[NFO] EDF initialization");
            this->cptSerial->begin(1200);

            tinfo.init();
            tinfo.attachADPS(ADPSCallback);
            tinfo.attachData(DataCallback);
            tinfo.attachNewFrame(NewFrame);
            tinfo.attachUpdatedFrame(UpdatedFrame);
            
            MainServer::server.on("/edf/status", (std::bind(&teleinfoModule::statusAPI, this)));
        }
        
        void loop() {
            static char c;
            if (cptSerial->available()) {
                c = cptSerial->read() ;
                tinfo.process(c);
            }
        }
        
        void statusAPI() {
            String JSONoutput = "";
            JSONoutput += "{ ";
            for (int i=0; i<NBCOMMANDS; i++) {
                JSONoutput += "\"";
                //JSONoutput += this->commands[i];
                JSONoutput += "\": \"";
                //JSONoutput += this->values[i];
                if (i<(NBCOMMANDS-1)) JSONoutput += "\", ";
                else JSONoutput += "\" ";
            }
            JSONoutput += "}\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }
        

        /***
        * App section for brick main web page
        */
        // TODO 
        String mainWebPage(String actualPage) {
            return actualPage;
        }

};


