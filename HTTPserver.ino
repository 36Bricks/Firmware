#include "flashfiles.h"

/***
 * Brick main web page, builds a page with a section for each enabled module
 */
void httpMainWebPage() {
  String HTMLoutput = readFromFlash("header.html") + readFromFlash("app.html");      
  HTMLoutput.replace("%%TYPE%%",BRICK_TYPE);        // Replace brick type in template
  
#if defined(MODULE_DHT22)       
  HTMLoutput = dht22MainWebPage(HTMLoutput);        // DHT22 module main app section
#endif   
#if defined(MODULE_STRIP)       
  HTMLoutput = stripMainWebPage(HTMLoutput);        // Strip module main app section
#endif   

  server.send(200, "text/html", HTMLoutput);    
}

/***
 * Serves a file directly from Flash memory 
 * Files are defined in websites.h, generated using convertWebFiles.sh
 * 
 * CAUTION : Some ressources have to be gziped (*.ico, *.js, *.css, *.png, etc...)
 */
bool serveFromFlash(String path) {
  if(path.endsWith("/")) path += "index.html";

  int NumFiles = sizeof(files)/sizeof(struct t_websitefiles);
  for(int i=0; i<NumFiles; i++) {
    if(path.endsWith(String(files[i].filename))) {      
      _FLASH_ARRAY<uint8_t>* filecontent;
      unsigned int len = 0;
      
      String dataType = files[i].mime;
      len = files[i].len;
      
      server.sendHeader("Content-Length", String(len));         // Send full content lenght to client
      server.sendHeader("Cache-Control", " max-age=581097");    // Send cache duration
      server.sendHeader("Content-Encoding", "gzip");            // Send gzip header
      server.send(200, files[i].mime, "");                      // Send real mime type to client
      
      filecontent = (_FLASH_ARRAY<uint8_t>*)files[i].content;
      filecontent->open();
      
      WiFiClient client = server.client();
      long tBefore = millis();
      client.write(*filecontent, SERVE_FROM_FLASH_PACKET_SIZE);
      
      Logln("[NFO] Served " + String(files[i].filename) + " from Flash in " + String(millis()-tBefore) + "ms");
      return true;
    }
  }
  return false;
}

/***
 * Reads a file directly from Flash memory to a String
 */
String readFromFlash(String path) {
  if(path.endsWith("/")) path += "index.html";

  int NumFiles = sizeof(files)/sizeof(struct t_websitefiles);
  for(int i=0; i<NumFiles; i++) {
    if(path.endsWith(String(files[i].filename))) {      
      _FLASH_ARRAY<uint8_t>* filecontent;
      unsigned int len = files[i].len;
      char buffer[len];

      filecontent = (_FLASH_ARRAY<uint8_t>*)files[i].content;
      filecontent->open();
      int c = 0;
      for (c=0; c<filecontent->count(); ++c) {
        buffer[c] = (*filecontent)[c];
      }
      buffer[c] = 0;
      return String(buffer);
    }
  }
  return String("ERROR");
}

/***
 * Handle all undefined endpoints, even resource files directly stored on flash
 * 404 page filled with system informations
 */
void handleNotFound() {
  
  // try to find the file in the flash
  if(serveFromFlash(server.uri())) return;
  
  String message = "File Not Found\n\n";
  message += "URI..........: ";
  message += server.uri();
  message += "\nMethod.....: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments..: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  message += "\n";
  message += "FreeHeap.....: " + String(ESP.getFreeHeap()) + "\n";
  message += "ChipID.......: " + String(ESP.getChipId()) + "\n";
  message += "FlashChipId..: " + String(ESP.getFlashChipId()) + "\n";
  message += "FlashChipSize: " + String(ESP.getFlashChipSize()) + " bytes\n";
  message += "getCycleCount: " + String(ESP.getCycleCount()) + " Cycles\n";
  message += "UpTime.......: " + String(millis()/1000) + " Seconds\n";
  server.send(404, "text/plain", message);
}

