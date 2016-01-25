/***
 * Brick main web page, builds a page with a section for each enabled module
 */
void httpMainWebPage() {
  String HTMLoutput = readFromSpiffs("/header.html") + readFromSpiffs("/app.html");      
  HTMLoutput.replace("%%TYPE%%",BRICK_TYPE);        // Replace brick type in template
  
#if defined(MODULE_DHT22)       
  HTMLoutput = dht22MainWebPage(HTMLoutput);        // DHT22 module main app section
#endif   
#if defined(MODULE_STRIP)       
  HTMLoutput = stripMainWebPage(HTMLoutput);        // Strip module main app section
#endif   
#if defined(MODULE_OUTPUTS)       
  HTMLoutput = outputMainWebPage(HTMLoutput);        // Strip module main app section
#endif   

  server.send(200, "text/html", HTMLoutput);    
}


/***
 * Handle all undefined endpoints, even resource files directly stored on flash
 * 404 page filled with system informations
 */
void handleNotFound() {
  
  // try to find the file in the flash
  if(serveFromSpiffs(server.uri())) return;
  
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

