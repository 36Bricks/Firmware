/***
 * OLED : Module to drive an I2C OLED screen.
 */

 // TODO : HTTP API endpoint to siplay a message on the screen 
 
#if defined(MODULE_OLED)

  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>

  #define OLED_SDA D3       // SDA Pin
  #define OLED_SCL D2       // SCL Pin
  #define OLED_RESET -1     // Reset Pin (-1 to ignore)
  
  #define OLED_ADRESS 0x3C  // Screen I2C adress
  
  #define OLED_INVERT_INTERVAL 30000  // Invert screen loop interval : 30 secondes
  #define OLED_DISPLAY_INTERVAL 300   // Screen update interval : 300 millisecondes
 
  Adafruit_SSD1306 display(OLED_RESET);
  
  elapsedMillis invertTimeElapsed, displayTimeElapsed;
  
  boolean oledInvertState = false;

  /***
   * OLED module setup : initialize I2C communication, and clear the display
   */
  void OLEDsetup()   {                
      Wire.begin(OLED_SDA, OLED_SCL);
      display.begin(SSD1306_SWITCHCAPVCC, OLED_ADRESS);  // initialize with the I2C addr
      display.clearDisplay();
      display.setCursor(0, 0);  
      Logln("[NFO] OLED initialization");
   }

  /***
   * Writes a line of text on the display
   */
  void OLEDwriteln(String text, int textSize, unsigned int textColor, unsigned int bgColor ) {
      display.setTextColor(textColor, bgColor);
      display.setTextSize(textSize);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(text);
  }

  /***
   * OLED module main loop
   */
  void OLEDloop() {
      OLEDdisplayLoop();
      OLEDinvertLoop();
      yield();
  }

  /***
   * OLED module display loop : refreshes the OLED screen with updated content
   */
  void OLEDdisplayLoop() {
    if (displayTimeElapsed > OLED_DISPLAY_INTERVAL)  {       
        display.display();
        displayTimeElapsed = 0;       // reset the counter to 0 so the counting starts over...
    }
  }

   /***
   * OLED module invert loop : inverts screen to prevent OLED burn
   */
  void OLEDinvertLoop() {
    if (invertTimeElapsed > OLED_INVERT_INTERVAL)  {       
        oledInvertState = !oledInvertState;    // toggle the state from HIGH to LOW to HIGH to LOW ... 
        display.invertDisplay(oledInvertState);
        invertTimeElapsed = 0;       // reset the counter to 0 so the counting starts over...
    }
  }

#endif

