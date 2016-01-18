#if defined(MODULE_STRIP)
  #include "RGBdriver.h"
  
  #define STRIP_CLK D7     
  #define STRIP_DIO D6
  #define STRIP_INTERVAL_MS 12

  RGBdriver stripDriver(STRIP_CLK,STRIP_DIO);
  long stripLastUpdt = 0;
  
  void stripSetup() {
     Logln("[NFO] RGB Strip initialization");
     server.on("/strip/hue", stripSetColor);
     server.on("/strip/rainbow/on", stripEnableRainbow);
     server.on("/strip/rainbow/off", stripDisableRainbow);
    
  }

  int stripLoopIndex=0;
  bool stripLoopEnabled = true;
  
  void stripLoop() {
    if (stripLoopEnabled) {
        long now = millis();
        if (now - stripLastUpdt > STRIP_INTERVAL_MS) {
            stripDriver.begin();
            stripLastUpdt = now; 
            int WheelPos = 255 - stripLoopIndex;
            if(WheelPos < 85) {
              stripDriver.SetColor(255 - WheelPos * 3, 0, WheelPos * 3);
            } else if(WheelPos < 170) {
              WheelPos -= 85;
              stripDriver.SetColor(0, WheelPos * 3, 255 - WheelPos * 3);
            } else {
              WheelPos -= 170;
              stripDriver.SetColor(WheelPos * 3, 255 - WheelPos * 3, 0);
            }
            stripLoopIndex++;
            if (stripLoopIndex>=256) stripLoopIndex=0;
            stripDriver.end();
    
        }
    }
  }
  
  void stripEnableRainbow() {
    stripLoopEnabled = true;
    server.send(200, "application/json", ReturnOK);
  }
  
  void stripDisableRainbow() {
    stripLoopEnabled = false;
    server.send(200, "application/json", ReturnOK);
  }
  
  void stripSetColor() {
    String newColor = server.arg("color");
    newColor.replace("#","");
    
    long number = strtol( newColor.c_str(), NULL, 16);
    long red = number >> 16;
    long green = number >> 8 & 0xFF;
    long blue = number & 0xFF; 
    
    stripLoopEnabled = false;
    
    stripDriver.begin();
    stripDriver.SetColor(red, green, blue);
    stripDriver.end();
    
    server.send(200, "application/json", ReturnOK);
  }


#endif

