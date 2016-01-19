/***
 * TELEINFO_EDF : Module to receive values from domestic EDF electicity counter (France only)
 */

 // TODO : Full debug, not tested yet 
 
#if defined(MODULE_TELEINFO_EDF)

  #if defined(OPTION_MQTT)
    #define topicEDF "edf/"
  #endif

  #include <SoftwareSerial.h>
  SoftwareSerial cptSerial(14, SW_SERIAL_UNUSED_PIN, false, 128);
  
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
  
  String commands[] = {"ADCO","OPTARIF","ISOUSC","BASE","HCHC","HCHP","EJPHN","EJPHPM","BBRHCJB","BBRHPJB","BBRHCJW","BBRHPJW","BBRHCJR","BBRHPJR","PEJP","PTEC","DEMAIN","IINST","IINST1","IINST2","IINST3","IMAX","IMAX1","IMAX2","IMAX3","PMAX","PAPP","HHPHC","MOTDETAT","PPOT"};
  String units[] = {"","","A","Wh","Wh","Wh","Wh","Wh","Wh","Wh","Wh","Wh","Wh","Wh","min","","","A","A","A","A","A","A","A","A","W","VA","","",""};
  int lengths[] = {12,4,2,9,9,9,9,9,9,9,9,9,9,9,2,4,4,3,3,3,3,3,3,3,3,5,5,1,6,2};
  
  #define NBCOMMANDS 30
  String values[NBCOMMANDS];
  
  
  void EDFsetup() {
      Logln("[NFO] EDF initialization");
      cptSerial.begin(1200);
      for (int i=0; i<NBCOMMANDS; i++) {
        values[i] = "N/A";
      }
      server.on("/edf/status", EDFAPI);
  }

  void EDFloop() {
      DecodeNextTeleInfo();
  }
  
  void EDFAPI() {
      String JSONoutput = "";
      JSONoutput += "{ ";
      for (int i=0; i<NBCOMMANDS; i++) {
        JSONoutput += "\"";
        JSONoutput += commands[i];
        JSONoutput += "\": \"";
        JSONoutput += values[i];
        if (i<(NBCOMMANDS-1)) JSONoutput += "\", ";
        else JSONoutput += "\" ";
      }
      JSONoutput += "}\r\n";
      server.send(200, "application/json", JSONoutput);
  }
   
  String GetTeleInfo() {
      String TeleInfo = "";
      char charIn = 0;
      int maxWait = 100;
      int nbWait = 0;
      while (charIn != startLine) {
          charIn = cptSerial.read() & 0x7F;
          if ((charIn != startLine) && ((nbWait++) > maxWait)) {
            return TeleInfo ;
          }
          else yield();
      }
      while (charIn != endLine) {
          if (cptSerial.available() > 0) {
              charIn = cptSerial.read() & 0x7F;
              TeleInfo += charIn;
              yield();
          }
      }
      return TeleInfo;
  }
  
  void DecodeNextTeleInfo() {
      String data = "";
      String value = "";
  
      int iCommandFound = -1;
      
      data = GetTeleInfo();     // On lit une data

      if (data.length()>2) {
        for (int i=0; i<NBCOMMANDS; i++) { // On la compare Ã  toutes les commandes
          if (data.substring(0,commands[i].length()) == commands[i]) { // Si elle correspond Ã  une commande
            Logln("[EVT] EDF Serial : "+ String(data));
            values[i] = data.substring((commands[iCommandFound].length() + 1),(lengths[iCommandFound] + (commands[iCommandFound].length() + 1)));
            #if defined(OPTION_MQTT)
                MQTTpublish(String(topicEDF)+String(commands[i]), String(values[i]).c_str());
            #endif
            break;
          }
        }
      }
  }
  

#endif

