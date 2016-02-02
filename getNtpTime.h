/***
* send an NTP request to the time server at the given address
*/
unsigned long sendNTPpacket(IPAddress& address) {
    memset(NTP.packetBuffer, 0, NTP_PACKET_SIZE);
    
    NTP.packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    NTP.packetBuffer[1] = 0;            // Stratum, or type of clock
    NTP.packetBuffer[2] = 6;            // Polling Interval
    NTP.packetBuffer[3] = 0xEC;         // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    NTP.packetBuffer[12]  = 49;
    NTP.packetBuffer[13]  = 0x4E;
    NTP.packetBuffer[14]  = 49;
    NTP.packetBuffer[15]  = 52;
    
    NTP.udp.beginPacket(address, 123);  //NTP requests are to port 123
    NTP.udp.write(NTP.packetBuffer, NTP_PACKET_SIZE);
    NTP.udp.endPacket();
}

/***
* NTP Sync provider
*/
time_t getNtpTime() {
    Log::Logln("[NFO] NTP sync time ...");
    while (NTP.udp.parsePacket() > 0) ;                   // discard any previously received packets
    WiFi.hostByName(NTP_SERVER_NAME, NTP.timeServerIP);       // get a server from the pool
    sendNTPpacket(NTP.timeServerIP);                          // send an NTP packet to the time server
    uint32_t beginWait = millis();
    while (millis() - beginWait < 15000) {                // waits for the answer
        int size = NTP.udp.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
            NTP.udp.read(NTP.packetBuffer, NTP_PACKET_SIZE);          // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 =  (unsigned long)NTP.packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)NTP.packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)NTP.packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)NTP.packetBuffer[43];
            return secsSince1900 - 2208988800UL + NTP_TIME_ZONE * SECS_PER_HOUR;
        }
        yield();
    }
    Log::Logln("[NFO] NTP sync time ERROR :'(");
    return 0;
}

