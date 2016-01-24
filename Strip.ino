/**
 * STRIP : Drives a non-adressable RGB Led Strip through an RGB driver module
 */
 // TODO : HTTP API endpoint to fade ON/OFF
 // TODO : HTTP API endpoint to flash a color (alarm)
 
#if defined(MODULE_STRIP)
  #include "RGBdriver.h"
  
  #define STRIP_CLK D7              // CLK pin
  #define STRIP_DIO D6              // DIO Pin
  #define STRIP_INTERVAL_MS 12      // RGB Strip loop interval

  RGBdriver stripDriver(STRIP_CLK,STRIP_DIO);
  long stripRainbowLastUpdt = 0;
  
  int stripRainbowLoopIndex=0;
  bool stripRainbowLoopEnabled = true;
  
  /***
   * Strip setup : declare HTTP API endpoints
   */
  void stripSetup() {
     Logln("[NFO] RGB Strip initialization");
     server.on("/strip/hue", stripSetColorAPI);
     server.on("/strip/rainbow/on", stripEnableRainbowAPI);
     server.on("/strip/rainbow/off", stripDisableRainbowAPI);
  }

  /***
   * Strip main loop
   */
  void stripLoop() {
    stripRainbowLoop();
  }
  
  /***
   * Strip rainbow loop : loops through colors 
   */
  void stripRainbowLoop() {
    if (stripRainbowLoopEnabled) {
        long now = millis();
        if (now - stripRainbowLastUpdt > STRIP_INTERVAL_MS) {
            stripDriver.begin();
            stripRainbowLastUpdt = now; 
            int WheelPos = 255 - stripRainbowLoopIndex;
            if(WheelPos < 85) {
              stripDriver.SetColor(255 - WheelPos * 3, 0, WheelPos * 3);
            } else if(WheelPos < 170) {
              WheelPos -= 85;
              stripDriver.SetColor(0, WheelPos * 3, 255 - WheelPos * 3);
            } else {
              WheelPos -= 170;
              stripDriver.SetColor(WheelPos * 3, 255 - WheelPos * 3, 0);
            }
            stripRainbowLoopIndex++;
            if (stripRainbowLoopIndex>=256) stripRainbowLoopIndex=0;
            stripDriver.end();
        }
    }
  }
  
   /***
   * HTTP JSON API to enable rainbow loop 
   */
  void stripEnableRainbowAPI() {
    stripRainbowLoopEnabled = true;
    server.send(200, "application/json", ReturnOK);
  }
  
   /***
   * HTTP JSON API to disable rainbow loop 
   */
  void stripDisableRainbowAPI() {
    stripRainbowLoopEnabled = false;
    server.send(200, "application/json", ReturnOK);
  }
  
   /***
   * HTTP JSON API to set a color
   */
  void stripSetColorAPI() {
    String newColor = server.arg("color");
    newColor.replace("#","");
    
    long number = strtol( newColor.c_str(), NULL, 16);
    long red = number >> 16;
    long green = number >> 8 & 0xFF;
    long blue = number & 0xFF; 
    
    stripRainbowLoopEnabled = false;
    
    stripDriver.begin();
    stripDriver.SetColor(red, green, blue);
    stripDriver.end();
    
    server.send(200, "application/json", ReturnOK);
  }
  
  /***
   * App section for brick main web page
   */
  String stripMainWebPage(String actualPage) {
      actualPage.replace("<!-- %%APP_ZONE%% -->", readFromFlash("app_strip.html"));   
      actualPage.replace("%%COLOR%%", String("123456"));    // Replace actual color 
      return actualPage;
  }



#endif

