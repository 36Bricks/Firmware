/***
 * OUTPUTS : Digital outputs modules. Used to control relays.
 */
 
// TODO : HTTP API endpoints for timed and sequenced outputs modes
// TODO : Add initial state to app section web page

#define OUTPUT_NB_OUT 2
#define OUTPUT_MAX_SYNC 8

class outputModule : public Module {
    private:
        int pin[OUTPUT_NB_OUT];
        bool output[OUTPUT_NB_OUT] = {false, false};

        bool blinkEnabled[OUTPUT_NB_OUT] = {false, false};
        long blinkOnDuration[OUTPUT_NB_OUT]  = {0, 0};
        long blinkOffDuration[OUTPUT_NB_OUT] = {0, 0};
        long lastBlinkSwitch[OUTPUT_NB_OUT]  = {0, 0};
        String topicOutput[OUTPUT_NB_OUT] = {"outputs/1", "outputs/2"};

        brickCommand *syncCommands[OUTPUT_NB_OUT][OUTPUT_MAX_SYNC];
        int enabledSyncCommands[OUTPUT_NB_OUT];
        
    public:
        outputModule(int pin1, int pin2) {
            this->pin[0] = pin1;
            this->pin[1] = pin2;
        }

        /***
        * Outputs setup : set pins modes, and declare HTTP API endpoints
        */
        void setup() {
            Log::Logln("[NFO] Outputs initialization");
            for(int out=0; out<OUTPUT_NB_OUT; out++) {
                for (int i=0; i<OUTPUT_MAX_SYNC; i++) {
                    this->syncCommands[out][i] = NULL;
                }
                this->enabledSyncCommands[out] = 0;
                pinMode(this->pin[out], OUTPUT); setOutput(out, false);
            }
            
            MainServer::server.on("/out/0/on", (std::bind(&outputModule::activateOutput1, this)));
            MainServer::server.on("/out/0/off", (std::bind(&outputModule::desactivateOutput1, this)));
            MainServer::server.on("/out/0/blink", (std::bind(&outputModule::blink1API, this)));
            MainServer::server.on("/out/0/addsync", (std::bind(&outputModule::addSync1, this)));
            MainServer::server.on("/out/0/remsync", (std::bind(&outputModule::remSync1, this)));
            
            MainServer::server.on("/out/1/on", (std::bind(&outputModule::activateOutput2, this)));
            MainServer::server.on("/out/1/off", (std::bind(&outputModule::desactivateOutput2, this)));
            MainServer::server.on("/out/1/blink", (std::bind(&outputModule::blink2API, this)));
            MainServer::server.on("/out/1/addsync", (std::bind(&outputModule::addSync2, this)));
            MainServer::server.on("/out/1/remsync", (std::bind(&outputModule::remSync2, this)));
            
            MainServer::server.on("/out/status", (std::bind(&outputModule::statusAPI, this)));
        }

        /***
        * Outputs loop
        */
        void loop() {
            for(int out=0; out<OUTPUT_NB_OUT; out++) blinkLoop(out);
        }
        
        /***
        * HTTP JSON API to enable outputs 
        */
        void setOutput(unsigned short numOutput, bool activate) {
            digitalWrite(this->pin[numOutput], (activate)?HIGH:LOW); 
            blinkEnabled[numOutput] = false;
            if (this->output[numOutput] != activate) {
                this->output[numOutput] = activate;
                #if defined(OPTION_MQTT)
                    MQTT.publish(this->topicOutput[numOutput], (activate)?"1":"0");
                #endif
                syncCommand(numOutput, (activate)?"on":"off");
            }
            MainServer::ReturnOK();
        }
        void activateOutput1() { setOutput(0, true); }
        void activateOutput2() { setOutput(1, true); }
        void desactivateOutput1() { setOutput(0, false); }
        void desactivateOutput2() { setOutput(1, false); }

        /**
         * HTTP API to add a sync'ed command to an output
         */
        void addSync(unsigned char out) {
            bool toAdd = true;
            for (int i=0; i<this->enabledSyncCommands[out]; i++) {
                if (this->syncCommands[out][i]->url == MainServer::server.arg("url")) {
                    toAdd = false;
                }
            }
            if (toAdd) {
                this->syncCommands[out][this->enabledSyncCommands[out]] = new brickCommand(MainServer::server.arg("name"), MainServer::server.arg("url"));
                this->enabledSyncCommands[out]++;
                MainServer::ReturnOK();
            } else {
                Log::Logln("[ERR] Sync already present");
                MainServer::ReturnKO("Sync already present");
            }
        }
        void addSync1() { addSync(0); }
        void addSync2() { addSync(1); }
        
        /**
         * HTTP API to remove a sync'ed command of an output
         */
        void remSync(unsigned char out) {
            bool isRemoved = false;
            for (int i=0; i<this->enabledSyncCommands[out]; i++) {
                if (isRemoved == true) {    // If removed, we move others one place up
                    Log::Logln("[NFO] Move " + this->syncCommands[out][i]->url + " from " + String(i) + " to " + String(i-1));
                    this->syncCommands[out][i-1] = new brickCommand(this->syncCommands[out][i]->name, this->syncCommands[out][i]->url);
                } else if(this->syncCommands[out][i]->url == MainServer::server.arg("url")) {
                    Log::Logln("[NFO] " + this->syncCommands[out][i]->url + " removed");
                    isRemoved = true;
                }
            }
            this->enabledSyncCommands[out]--;
        }
        void remSync1() { remSync(0); }
        void remSync2() { remSync(1); }
        
        void syncCommand(unsigned short numOut, String command) {
            for (int i=0; i<this->enabledSyncCommands[numOut]; i++) {
                if (this->syncCommands[numOut][i]!=NULL) {
                    Log::Logln("[EVT] Trigger synced command : " + syncCommands[numOut][i]->url );
                    HTTPClient cli;
                    cli.begin(this->syncCommands[numOut][i]->url + command); 
                    cli.setTimeout(250);
                    cli.GET();
                    yield();
                }
            }
        }

       /***
        * HTTP JSON API to send outputs statuses
        */
        void statusAPI() {
            String JSONoutput = "{ ";
            for(int out=0; out<OUTPUT_NB_OUT; out++) {
                JSONoutput += "\"out"+String(out)+"\": \""; 
                JSONoutput +=(this->output[out] ? "1" : "0"); 
                JSONoutput +="\"";
                if (out<(OUTPUT_NB_OUT-1)) JSONoutput +=", ";
            }
            JSONoutput += "}\r\n";
            MainServer::server.send(200, "application/json", JSONoutput);
        }

        /***
        * App section for brick main web page
        */
        // TODO : add buttons to control blink modes
        String mainWebPage(String actualPage) {
            actualPage.replace("<!-- %%APP_ZONE%% -->", SpiFfs::readFile("/app_outputs.html"));    
            
            // Hide link buttons if no other commands
            if (discovery.commandsCount()<1) {
                Log::Logln("[NFO] Did not found any linkable command");
                actualPage.replace("%%DISP_OUT_LINKS%%", "style=\"display:none\"");
                actualPage.replace("%%SYNC_COMMANDS_0%%", "");
                actualPage.replace("%%SYNC_COMMANDS_1%%", "");
            } else {
                actualPage.replace("%%DISP_OUT_LINKS%%", "");
                actualPage.replace("%%SYNC_COMMANDS_0%%", this->commandsList(0));
                actualPage.replace("%%SYNC_COMMANDS_1%%", this->commandsList(1));
            }
            return actualPage;
        }

        /***
        * Blink outputs loop
        */
        void blinkLoop(unsigned short numOutput) {
            if (!this->blinkEnabled[numOutput]) return;
            
            long testDuration = 0;
            if (this->output[numOutput]) testDuration = this->blinkOnDuration[numOutput];
            else testDuration = this->blinkOffDuration[numOutput];

            if (millis() - this->lastBlinkSwitch[numOutput] > testDuration) {
                this->lastBlinkSwitch[numOutput] = millis();
                if (this->output[numOutput]) {
                    Log::Logln("[NFO] Output "+String(numOutput+1)+" blink OFF");
                    digitalWrite(this->pin[numOutput], LOW); 
                    this->output[numOutput] = false;
                } else {
                    Log::Logln("[NFO] Output "+String(numOutput+1)+" blink ON");
                    digitalWrite(this->pin[numOutput], HIGH); 
                    this->output[numOutput] = true;
                }
            }
        }

        /**
        * Retruns a list of commands of all discovered bricks to replace "%%SYNC_COMMANDS%%" on app html files
        */
        String commandsList(unsigned char numOut) {
            String tmp = "";
            for (DiscoveredBrick *d = discoveredBricks; d; d = d->nextBrick) {
                for (int i=0; i<8; i++) {
                    if (d->commands[i] != NULL) {
                        tmp += "<li><label><input type=\"checkbox\" data-name=\"";
                        tmp += String(d->commands[i]->name);
                        tmp += "\" data-url=\"";
                        tmp += String(d->commands[i]->url);
                        tmp += "\" ";
                        for (int c=0; c<this->enabledSyncCommands[numOut]; c++) {
                            if (this->syncCommands[numOut][c]!=NULL) {
                                if (syncCommands[numOut][c]->url == d->commands[i]->url) {
                                    tmp += "checked=\"checked\" ";
                                    break;
                                }
                            }
                        }
                        tmp += "/> ";
                        tmp += String(d->commands[i]->name);
                        tmp += " on ";
                        tmp += String(d->name);
                        tmp += "</label></li>";
                    }
                } 
            }
            return tmp;
        }

        /***
        * HTTP JSON API to blink outputs
        */
        void blinkAPI(unsigned short numOutput) {
            this->blinkOnDuration[numOutput]  = MainServer::server.arg("on").toInt();  // Parse on duration
            this->blinkOffDuration[numOutput] = MainServer::server.arg("off").toInt(); // Parse off duration
            if ((this->blinkOnDuration[numOutput]==0) || (this->blinkOffDuration[numOutput]==0)) {
                Log::Logln("[NFO] Output "+String(numOutput+1)+" blink mode stopped");
                this->blinkEnabled[numOutput] = false;
            } else {
                Log::Logln("[NFO] Output "+String(numOutput+1)+" blink mode started : " + String(this->blinkOnDuration[numOutput]) + " / " + String(this->blinkOffDuration[numOutput]));
               this->blinkEnabled[numOutput] = true;
               this->lastBlinkSwitch[numOutput] = millis();
            }
            MainServer::ReturnOK();
        }
        void blink1API() { blinkAPI(0); }
        void blink2API() { blinkAPI(1); }
        
};

