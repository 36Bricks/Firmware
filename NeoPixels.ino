#if defined(MODULE_NEOPIXELS)

  #include <Adafruit_NeoPixel.h>
  
  #define NEOPIXELS_PIN D5
  #define NEOPIXELS_NUM 8
  #define NEOPIXELS_INTERVAL_MS 12

  Adafruit_NeoPixel NeoPixels = Adafruit_NeoPixel(NEOPIXELS_NUM, NEOPIXELS_PIN, NEO_GRB + NEO_KHZ800);
  bool pixelsLoopEnabled = true;
  
  void NeoPixelsSetup() {
    Logln("[NFO] NeoPixels initialization");
    NeoPixels.begin();
    server.on("/pixels/rainbow/on", pixelsEnableRainbow);
    server.on("/pixels/rainbow/off", pixelsDisableRainbow);

  }
  
  long NeoPixelsLastUpdt = 0;
  
  uint16_t i, j;
  void rainbowLoop() {
    if (pixelsLoopEnabled) {
      long now = millis();
      if (now - NeoPixelsLastUpdt > NEOPIXELS_INTERVAL_MS) {
        NeoPixelsLastUpdt = now; 
        for(i=0; i< NeoPixels.numPixels(); i++) {
          NeoPixels.setPixelColor(i, Wheel(((i * 256 / NeoPixels.numPixels()) + j) & 255));
        }
        NeoPixels.show();
        j++;
        if (j>=(256*5)) j=0;
      }
    }
  }
 
  void NeoPixelsLoop() {
    rainbowLoop();
  }

  void pixelsEnableRainbow() {
    pixelsLoopEnabled = true;
    server.send(200, "application/json", ReturnOK);
  }
  
  void pixelsDisableRainbow() {
    pixelsLoopEnabled = false;
    server.send(200, "application/json", ReturnOK);
  }

  String NeoPixelsAPI(String req) {
    String JSONoutput = "";
  
    return JSONoutput;
  }

  uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
      return NeoPixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170) {
      WheelPos -= 85;
      return NeoPixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return NeoPixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }

#endif

