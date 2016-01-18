#if defined(OPTION_NTP) 

  #include <TimeLib.h> 
  #include <WiFiUdp.h>

  #define NTP_SERVER_NAME "fr.pool.ntp.org" 
  #define NTP_PACKET_SIZE 48  // NTP time stamp is in the first 48 bytes of the message
  #define NTP_INTERVAL_MS 500 // 0.5 seconds
  #define timeZone 1          // Central European Time
  #define UDT_LOCAL_PORT 2390     

  IPAddress timeServerIP;
  byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
  elapsedMillis ntpUpdateTimeElapsed;
  WiFiUDP udp;
  
  void ntpSetup() {
    Logln("[NFO] NTP initialization");
    if (wifiOK) { 
      udp.begin(UDT_LOCAL_PORT);
      setSyncProvider(getNtpTime);
    }
  }

  void ntpLoop() {
    if (wifiOK) { 
      if (ntpUpdateTimeElapsed > NTP_INTERVAL_MS)  {       
        TheTime = DateTime();
        ntpUpdateTimeElapsed = 0;       // reset the counter to 0 so the counting starts over...
      }
    }
  }

  String DateTime() {
    String tmp = String(hour()) + printDigits(minute());
    tmp += printDigits(second()) + " ";
    tmp += String(day())+"/"+String(month())+"/"+String(year()); 
    return tmp;
  }
  String GetDate() {
    String tmp = printDigits(day())+"/"+printDigits(month())+"/"+String(year()); 
    return tmp;
  }
  String GetTime() {
    String tmp = String(hour()) + printDigits(minute());
    tmp += printDigits(second()) ; 
    return tmp;
  }

  String printDigits(int digits){
    // utility for digital clock display: prints preceding colon and leading 0
    return String(":") + String(((digits < 10)?"0":"")) + String(digits);
  }



  time_t getNtpTime() {
    Logln("[NFO] NTP sync time ...");
    while (udp.parsePacket() > 0) ; // discard any previously received packets
    WiFi.hostByName(NTP_SERVER_NAME, timeServerIP);  //get a random server from the pool
    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
    uint32_t beginWait = millis();
    while (millis() - beginWait < 15000) {
      int size = udp.parsePacket();
      if (size >= NTP_PACKET_SIZE) {
        udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
        unsigned long secsSince1900;
        // convert four bytes starting at location 40 to a long integer
        secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long)packetBuffer[43];
        return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
      }
    }
    Logln("[NFO] NTP sync time ERROR :'(");
    return 0;
  }

  // send an NTP request to the time server at the given address
  unsigned long sendNTPpacket(IPAddress& address) {
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;            // Stratum, or type of clock
    packetBuffer[2] = 6;            // Polling Interval
    packetBuffer[3] = 0xEC;         // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
  
    udp.beginPacket(address, 123);  //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
  }
#endif
