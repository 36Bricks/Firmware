/***
 * NEOPIXELS : RGB Pixels module, drives up to 1024 adressable rgb leds from one single output pin
 */

 // TODO : HTTP API endpoint to set individual pixels
 
#if defined(MODULE_NEOPIXELS)
  #include <Adafruit_NeoPixel.h>
  
  #define NEOPIXELS_PIN D5                  // NeoPixels output pin
  #define NEOPIXELS_NUM 8                   // How many pixels ?
  #define NEOPIXELS_RAINBOW_INTERVAL_MS 12  // Rainbow loop update interval
  #define NEOPIXELS_COLOR_ORDER NEO_GRB     // Color order of pixels (NEO_GRB or NEO_RGB)
  #define NEOPIXELS_FREQUENCY NEO_KHZ800    // NeoPixels PWM frequency (NEO_KHZ400 or NEO_KHZ800)

  Adafruit_NeoPixel NeoPixels = Adafruit_NeoPixel(NEOPIXELS_NUM, NEOPIXELS_PIN, NEO_GRB + NEO_KHZ800);
  bool neoPixelsRainbowLoopEnabled = true;
  long neoPixelsRainbowLastUpdt = 0;
  uint16_t neoPixelsRainbowColorIndex = 0;
  
  /***
   * NEOPIXELS module setup : initialize NeoPixels lib, and declare HTTP API endpoints
   */
  void neoPixelsSetup() {
    Logln("[NFO] NeoPixels initialization");
    NeoPixels.begin();
    server.on("/pixels/rainbow/on", pixelsEnableRainbowAPI);
    server.on("/pixels/rainbow/off", pixelsDisableRainbowAPI);
  }
  
  /***
   * NEOPIXELS rainbow loop : loops through colors 
   */
  void neoPixelsRainbowLoop() {
    if (neoPixelsRainbowLoopEnabled) {
      long now = millis();
      if (now - neoPixelsRainbowLastUpdt > NEOPIXELS_RAINBOW_INTERVAL_MS) {
        neoPixelsRainbowLastUpdt = now; 
        for(uint16_t i = 0; i < NeoPixels.numPixels(); i++) {
          NeoPixels.setPixelColor(i, Wheel(((i * 256 / NeoPixels.numPixels()) + neoPixelsRainbowColorIndex) & 255));
        }
        NeoPixels.show();
        neoPixelsRainbowColorIndex++;
        if (neoPixelsRainbowColorIndex >= (256*5)) neoPixelsRainbowColorIndex=0;
      }
    }
  }
 
  /***
   * NEOPIXELS main loop
   */
  void neoPixelsLoop() {
    neoPixelsRainbowLoop();
  }

  /***
   * HTTP JSON API to enable rainbow mode
   */
  void pixelsEnableRainbowAPI() {
    neoPixelsRainbowLoopEnabled = true;
    server.send(200, "application/json", ReturnOK);
  }
  
  /***
   * HTTP JSON API to disable rainbow mode
   */
  void pixelsDisableRainbowAPI() {
    neoPixelsRainbowLoopEnabled = false;
    server.send(200, "application/json", ReturnOK);
  }

  /***
   * Converts a single position (0-255) to NeoPixels RGB color 
   */
  uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85)  
      return NeoPixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    else if(WheelPos < 170) {
      WheelPos -= 85;
      return NeoPixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    } else {
      WheelPos -= 170;
      return NeoPixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
  }

#endif

