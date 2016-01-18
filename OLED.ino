#if defined(MODULE_OLED)

  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>

  #define OLED_SDA D3
  #define OLED_SCL D2
  #define OLED_RESET -1
  #define OLED_ADRESS 0x3C
  #define OLED_INVERT_INTERVAL 30000  // 30 secondes
  #define OLED_DISPLAY_INTERVAL 300  // 300 millisecondes
 
  elapsedMillis invertTimeElapsed, displayTimeElapsed;
  boolean oledInvertState = false;
  Adafruit_SSD1306 display(OLED_RESET);

  void OLEDsetup()   {                
      Wire.begin(OLED_SDA, OLED_SCL);
      display.begin(SSD1306_SWITCHCAPVCC, OLED_ADRESS);  // initialize with the I2C addr
      display.clearDisplay();
      display.setCursor(0, 0);  
      Logln("[NFO] OLED initialization");
      yield();
   }

  void OLEDwriteln(String text, int textSize, unsigned int textColor, unsigned int bgColor ) {
      display.setTextColor(textColor, bgColor);
      display.setTextSize(textSize);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(text);
  }

  void OLEDloop() {
      if (displayTimeElapsed > OLED_DISPLAY_INTERVAL)  {       
        display.display();
        displayTimeElapsed = 0;       // reset the counter to 0 so the counting starts over...
      }
      if (invertTimeElapsed > OLED_INVERT_INTERVAL)  {       
        oledInvertState = !oledInvertState;    // toggle the state from HIGH to LOW to HIGH to LOW ... 
        display.invertDisplay(oledInvertState);
        invertTimeElapsed = 0;       // reset the counter to 0 so the counting starts over...
      }
      yield();
  }

#endif

