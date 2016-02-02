/***
 * SPIFFS - Module to access the embedded (1Mo) file system used to store
 * web files and logs.
 */
 
namespace SpiFfs {
    
    /**
     * SPIFFS initialization
     */
    void setup() {
        if (SPIFFS.begin()) {
            FSInfo info;
            if (SPIFFS.info(info)) {
                Log::Logln("[NFO] SPIFFS opened : " + String(info.usedBytes/1024) + " ko used / " + String(info.totalBytes/1024) + " ko total");
            }
        } else {
            Log::Logln("[ERR] SPIFFS error : unable to open !");
            ESP.restart();
        }
    }
    
    /***
     * Reads a file directly from SPIFFS to a String
     */
    String readFile(String path) {
        if(path.endsWith("/")) path += "index.html";
    
        File in = SPIFFS.open(path.c_str(), "r");
        if (!in) {
            Log::Logln("[ERR] Unable to find '" + path + "' in SPIFFS");
            return "";
        }
        
        in.setTimeout(0);
        String result = in.readString();
        return result;
    }
}
        
