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

  long stripRed, stripGreen, stripBlue;
  
  /***
   * Strip setup : declare HTTP API endpoints
   */
  void stripSetup() {
     Logln("[NFO] RGB Strip initialization");
     server.on("/strip/hue", stripSetColorAPI);
     server.on("/strip/rainbow/on", stripEnableRainbowAPI);
     server.on("/strip/rainbow/off", stripDisableRainbowAPI);
     server.on("/strip/getcolor", stripGetColorAPI);
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
            stripRainbowLastUpdt = now; 
            int WheelPos = 255 - stripRainbowLoopIndex;
            if(WheelPos < 85) {
              stripRed = 255 - WheelPos * 3;
              stripGreen = 0;
              stripBlue = WheelPos * 3; 
            } else if(WheelPos < 170) {
              WheelPos -= 85;
              stripRed = 0;
              stripGreen = WheelPos * 3;
              stripBlue = 255 - WheelPos * 3; 
            } else {
              WheelPos -= 170;
              stripRed = WheelPos * 3;
              stripGreen = 255 - WheelPos * 3;
              stripBlue = 0; 
            }
            stripDriver.begin();
            stripDriver.SetColor(stripRed, stripGreen, stripBlue);
            stripDriver.end();
            stripRainbowLoopIndex++;
            if (stripRainbowLoopIndex>=256) stripRainbowLoopIndex=0;
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
    stripRed = number >> 16;
    stripGreen = number >> 8 & 0xFF;
    stripBlue = number & 0xFF; 
    
    stripRainbowLoopEnabled = false;
    
    stripDriver.begin();
    stripDriver.SetColor(stripRed, stripGreen, stripBlue);
    stripDriver.end();
    
    server.send(200, "application/json", ReturnOK);
  }
  
   /***
   * HTTP JSON API to get current color (HTML format without #)
   */
  void stripGetColorAPI() {
    String JSONoutput = "";
    JSONoutput += "{ \"color\": \"";
    JSONoutput += ((stripRed<16)?"0":"")   + String(stripRed, HEX) + 
                  ((stripGreen<16)?"0":"") + String(stripGreen, HEX) + 
                  ((stripBlue<16)?"0":"")  + String(stripBlue, HEX);
    JSONoutput += "\" }\r\n";
    server.send(200, "application/json", JSONoutput);
  }
  
  /***
   * App section for brick main web page
   */
  String stripMainWebPage(String actualPage) {
      actualPage.replace("<!-- %%APP_ZONE%% -->", readFromSpiffs("/app_strip.html"));   
      actualPage.replace("%%COLOR%%", ((stripRed<16)?"0":"")   + String(stripRed, HEX) + 
                                      ((stripGreen<16)?"0":"") + String(stripGreen, HEX) + 
                                      ((stripBlue<16)?"0":"")  + String(stripBlue, HEX));    // Replace with actual color (HTML format without #)
      return actualPage;
  }



#endif

