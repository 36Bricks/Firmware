/***
 * SPIFFS - Module to access the embedded (1Mo) file system used to store
 * web files and logs.
 */

/**
 * SPIFFS initialization
 */
void spiffsSetup() {
    if (SPIFFS.begin()) {
        Logln("[NFO] SPIFFS opened");
        FSInfo info;
        if (SPIFFS.info(info)) {
            Serial.printf("[NFO] SPIFFS Total: %u ko, Used: %u ko, Block: %u ko, Page: %u, Max open files: %u, Max path len: %u\n",
                  info.totalBytes/1024,
                  info.usedBytes/1024,
                  info.blockSize/1024,
                  info.pageSize,
                  info.maxOpenFiles,
                  info.maxPathLength
                 );
        }
        Dir root = SPIFFS.openDir("/");
        while (root.next()) {
          String fileName = root.fileName();
          File f = root.openFile("r");
          Serial.printf("[NFO] %s: %d\r\n", fileName.c_str(), f.size());
        }
    } else {
        Logln("[ERR] SPIFFS error : unable to open !");
        ESP.restart();
    }
}

/***
 * Serves a file directly from SPIFFS
 * CAUTION : Some ressources have to be gziped (*.ico, *.js, *.css, *.png, etc...)
 */
bool serveFromSpiffs(String path) {
    Logln("[NFO] Asked to serve '" + path + "' from SPIFFS");
    if(path.endsWith("/")) path += "index.html";

    File in = SPIFFS.open(path.c_str(), "r");
    if (!in) {
        Logln("[ERR] Unable to find '" + path + "' in SPIFFS");
        return false;
    }
    
    in.setTimeout(0);
    long len = in.size();
    Logln("[NFO] " + String(path) + " is " + String(len) + "o long");
   
    String dataType = "text/plain";

    if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
    else if(path.endsWith(".htm")) dataType = "text/html";
    else if(path.endsWith(".css")) dataType = "text/css";
    else if(path.endsWith(".js"))  dataType = "application/javascript";
    else if(path.endsWith(".png")) dataType = "image/png";
    else if(path.endsWith(".gif")) dataType = "image/gif";
    else if(path.endsWith(".jpg")) dataType = "image/jpeg";
    else if(path.endsWith(".ico")) dataType = "image/x-icon";
    else if(path.endsWith(".xml")) dataType = "text/xml";
    else if(path.endsWith(".pdf")) dataType = "application/pdf";
    else if(path.endsWith(".zip")) dataType = "application/zip";

    /*server.sendHeader("Content-Length", String(len));           // Send full content lenght to client
    */server.sendHeader("Cache-Control", " max-age=581097");      // Send cache duration
    server.sendHeader("Content-Encoding", "gzip");              // Send gzip header (every file served directly MUST be gziped)
    /*server.send(200, dataType, "");                             // Send real mime type to client
    */
    long tBefore = millis();
    long sent = server.streamFile(in, dataType);
    if ( sent != len) {
        in.close();
        Logln("[ERR] Sent less data than expected : " + String(sent) + "/" + String(len) );
        return false;
    }
    
    in.close();
    Logln("[NFO] Served " + String(path) + " from SPIFFS in " + String(millis()-tBefore) + "ms");
    return true;
}

/***
 * Reads a file directly from SPIFFS to a String
 */
String readFromSpiffs(String path) {
    if(path.endsWith("/")) path += "index.html";

    File in = SPIFFS.open(path.c_str(), "r");
    if (!in) {
        Logln("[ERR] Unable to find '" + path + "' in SPIFFS");
        return "";
    }
    
    in.setTimeout(0);
    String result = in.readString();
    return result;
}

