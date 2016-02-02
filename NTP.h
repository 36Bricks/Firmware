/***
 * NTP : Option to sync ESP time online
 * /!\ TimeLib automatically manages NTP sync each time it's necessary (tested @ 5 minutes)
 */

// TODO : HTTP API Endpoint to force sync with remote NTP server
time_t getNtpTime();
#define NTP_PACKET_SIZE 48                  // NTP time stamp is in the first 48 bytes of the message
    
class ntpOption : public Option {
    private:
         long ntpUpdateTimeElapsed;
         long lastUpdt = 0;
                
    public:
       byte packetBuffer[NTP_PACKET_SIZE];        //buffer to hold incoming and outgoing packets
        IPAddress timeServerIP;
        WiFiUDP udp;
        /***
        * NTP option setup : starts UDP communication and define Sync provider
        */
        void setup() {
            Log::Logln("[NFO] NTP initialization");
            if (MainWifi::ok) { 
                udp.begin(UDP_LOCAL_PORT);
                setSyncProvider(getNtpTime);
            }
        }

        /***
        * NTP main loop : updates local time on defined interval
        */
        void loop() {
            if (MainWifi::ok) { 
                long now = millis();
                if (now - this->lastUpdt > NTP_INTERVAL_MS) {
                    lastUpdt = now;      
                    TheTime = dateTime();
                    ntpUpdateTimeElapsed = 0;       // reset the counter to 0 so the counting starts over...
                }
            }
        }

        /***
        * Returns date and time as a formated string
        */
        String dateTime() {
            String tmp = String(hour()) + ":" + printDigits(minute()) + ":" + printDigits(second()) + " " + printDigits(day()) + "/" + printDigits(month()) + "/" + String(year()); 
            return tmp;
        }

        /***
        * Returns date as a formated string
        */
        String getDate() {
            String tmp = printDigits(day())+"/"+printDigits(month())+"/"+String(year()); 
            return tmp;
        }

        /***
        * Returns time as a formated string
        */
        String getTime() {
            String tmp = String(hour()) + printDigits(minute());
            tmp += printDigits(second()) ; 
            return tmp;
        }

        /***
        * Returns a number with leading zero
        */
        String printDigits(int digits){
            return String(((digits < 10)?"0":"")) + String(digits);
        }

};

