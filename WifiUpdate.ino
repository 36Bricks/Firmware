/**
 * Manages the firmware update through wifi
 */
void wifiUpdateSetup() {

    // Firmware update web page
    server.on("/firmware", HTTP_GET, [](){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      String firmPage = readFromSpiffs("/header.html") + readFromSpiffs("/firmware-update.html");  // Firmware update page
      String firmVersion = String(FIRMWARE_VERSION) + " for " + String(BRICK_TYPE);
      firmPage.replace("%%VERSION%%",firmVersion); // Replace firmware version in template
      server.send(200, "text/html", firmPage); 
    });
    
    // Firmware update file upload (form destination)
    server.onFileUpload([](){
      if(server.uri() != "/update") return;
      HTTPUpload& upload = server.upload();
      if(upload.status == UPLOAD_FILE_START){
          Serial.setDebugOutput(true);
          WiFiUDP::stopAll();
          uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
          Logln("[NFO] Update : " + String(upload.filename) + " - Max = " + String(maxSketchSpace));
          if(!Update.begin(maxSketchSpace)){//start with max available size
              Update.printError(Serial);
          }
      } else if(upload.status == UPLOAD_FILE_WRITE){
          if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
              Update.printError(Serial);
          }
      } else if(upload.status == UPLOAD_FILE_END){
          if(Update.end(true)){ //true to set the size to the current progress
              Logln("[NFO] Update Success : " + String(upload.totalSize));
              Logln("[NFO] Rebooting ...");
          } else {
              Update.printError(Serial);
          }
          Serial.setDebugOutput(false);
      }
      yield();
    });
    
    // Firmware update form destination (processed after file upload)
    server.on("/update", HTTP_POST, [](){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      String firmPage = readFromSpiffs("/header.html") + readFromSpiffs("/firmware-update-complete.html");  // Firmware update complete page
      server.send(200, "text/html", firmPage); 
      ESP.restart();
    });
}

