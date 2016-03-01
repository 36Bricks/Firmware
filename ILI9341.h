/**
 * ILI9341 : Drives a 2.2" SPI LCD screen
 */

#define BUFFPIXEL 80

#define ILI9341_MODE_BOOT 0
#define ILI9341_MODE_LOG  1
#define ILI9341_MODE_APP  2

#define ILI9341_VERYDARKGREY 0x4208

class ili9341Module : public Module {
    private:
        int dcPin, csPin;
        Adafruit_ILI9341 *tft;
        long lastUpdt    = 0;
        uint8_t rotation = 0;
        int tftWidth  = 320, tftHeight = 240;
        String logLines[ILI9341_NB_LOG];
        int displayMode = ILI9341_MODE_BOOT;

    public:
        /**
        * Screen is initialized as soon as possible to display startup logs
        */
        ili9341Module(int dcPin, int csPin) {
            this->dcPin = dcPin;
            this->csPin = csPin;
            this->tft = new Adafruit_ILI9341(this->csPin, this->dcPin);
            for (int i=0; i<ILI9341_NB_LOG; i++) {
                this->logLines[i] = String("");
            }
        }
        
        /***
        * ILI9341 setup
        */
        void setup() {
            Log::Logln("[NFO] SPI Screen initialization");
            this->displayMode = ILI9341_MODE_APP;
            this->tft->fillScreen(ILI9341_BLACK);
            this->displayHeader();
        }
        
        /***
        * ILI9341 earlySetup
        */
        void earlySetup() {
            Log::Logln("[NFO] SPI Screen EARLY initialization");
            this->tft->begin();
            this->tft->setRotation(3);
            this->displayMode = ILI9341_MODE_LOG;
            this->displayHeader();
        }

        void displayHeader() {
            if (this->displayMode == ILI9341_MODE_APP) {
                
                // - Time
                this->tft->setCursor(255, 2);
                this->tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
                this->tft->setTextSize(2);
                this->tft->println(String(TheTime).substring(0,5));

                // - Signal
                int rssi = WiFi.RSSI();
                this->tft->fillRect(228, 9, 3, 8,  (rssi>-71 ? ILI9341_WHITE : ILI9341_VERYDARKGREY));
                this->tft->fillRect(232, 7, 3, 10, (rssi>-61 ? ILI9341_WHITE : ILI9341_VERYDARKGREY));
                this->tft->fillRect(236, 5, 3, 12, (rssi>-51 ? ILI9341_WHITE : ILI9341_VERYDARKGREY));
                this->tft->fillRect(240, 3, 3, 14, (rssi>-41 ? ILI9341_WHITE : ILI9341_VERYDARKGREY));
                this->tft->fillRect(244, 1, 3, 16, (rssi>-31 ? ILI9341_WHITE : ILI9341_VERYDARKGREY));
                this->tft->setCursor(145, 2);
                this->tft->setTextSize(1);
                this->tft->println(String(WiFi.RSSI())+"dB");

                // - Icon
                this->bmpDraw("/36.bmp");
                
            } else {
                this->tft->fillScreen(ILI9341_BLACK);
                this->tft->setTextSize(2);
                this->tft->setTextColor(ILI9341_GREEN);
                this->tft->setCursor(0, 0);
                this->tft->println("36Bricks - " + String(BRICK_TYPE));
            }
            
        }

       void displayLogLines() {
            if (this->displayMode == ILI9341_MODE_LOG) {
                this->tft->fillRect(0, 16, tftWidth, tftHeight-16, ILI9341_BLACK);
                this->tft->setCursor(0, 16);
                
                this->tft->setTextSize(1);
                for (int i=ILI9341_NB_LOG-1; i>=0; i--) {
                    if (this->logLines[i].indexOf("[ERR]")>=0) {
                        this->tft->setTextColor(ILI9341_RED);
                    } else if (this->logLines[i].indexOf("[WRN]")>=0) {
                        this->tft->setTextColor(ILI9341_YELLOW);
                    } else if (this->logLines[i].indexOf("[EVT]")>=0) {
                        this->tft->setTextColor(ILI9341_GREEN);
                    } else {
                        this->tft->setTextColor(ILI9341_WHITE);
                    }
                    this->tft->println(this->logLines[i]);
                }
            }

       }

       void addLogLine(String logLine) {
            // Move lines one line up
            for (int i=ILI9341_NB_LOG-1; i>=0; i--) {
                this->logLines[i] = this->logLines[i-1];
            }

            // Add the new line
            this->logLines[0] = String(logLine);

            // Display all
            this->displayLogLines();
        }
        
        /***
        * ILI9341 main loop
        */
        void loop() {
            long now = millis();
            if (now - this->lastUpdt > ILI9341_INTERVAL_MS) {
                this->lastUpdt = now; 
                this->displayHeader();
                ringMeter(WiFi.RSSI(), -88, -20, 15, 21, 55, "dBm", 5);
                ringMeter(WiFi.RSSI(), -88, -20, 175, 21, 55, "dBm", 5);
            }
        }
        
        /***
        * ILI9341 section for brick main web page
        */
        String mainWebPage(String actualPage) {
            return actualPage;
        }
        
        void bmpDraw(const char *filename, uint8_t x=0, uint16_t y=0) {
        
            File     bmpFile;
            int      bmpWidth, bmpHeight;   // W+H in pixels
            uint8_t  bmpDepth;              // Bit depth (currently must be 24)
            uint32_t bmpImageoffset;        // Start of image data in file
            uint32_t rowSize;               // Not always = bmpWidth; may have padding
            uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
            uint16_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
            boolean  goodBmp = false;       // Set to true on valid header parse
            boolean  flip    = true;        // BMP is stored bottom-to-top
            int      w, h, row, col;
            uint8_t  r, g, b;
            uint32_t pos = 0, startTime = millis();
            
            uint16_t awColors[320];  // hold colors for one row at a time...
            
            if((x >= this->tft->width()) || (y >= this->tft->height())) return;
            if (!(bmpFile = SPIFFS.open(filename, "r"))) {
                Serial.print(F("File not found"));
                return;
            }
            
            // Parse BMP header
            if(read16(bmpFile) == 0x4D42) { // BMP signature
                read32(bmpFile);
                (void)read32(bmpFile); // Read & ignore creator bytes
                bmpImageoffset = read32(bmpFile); // Start of image data
                read32(bmpFile);
                bmpWidth  = read32(bmpFile);
                bmpHeight = read32(bmpFile);
                if(read16(bmpFile) == 1) {
                    bmpDepth = read16(bmpFile);
                    if((bmpDepth == 24) && (read32(bmpFile) == 0)) {
                        goodBmp = true;
                        rowSize = (bmpWidth * 3 + 3) & ~3;
                        
                        if(bmpHeight < 0) {
                            bmpHeight = -bmpHeight;
                            flip      = false;
                        }
                        
                        w = bmpWidth;
                        h = bmpHeight;
                        if((x+w-1) >= this->tft->width())  w = this->tft->width()  - x;
                        if((y+h-1) >= this->tft->height()) h = this->tft->height() - y;
                        
                        for (row=0; row<h; row++) { 
                            if(flip) 
                                pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                            else     
                                pos = bmpImageoffset + row * rowSize;
                            if(bmpFile.position() != pos) {
                                bmpFile.seek(pos, SeekSet);
                                buffidx = sizeof(sdbuffer);
                            }
                            
                            for (col=0; col<w; col++) { 
                                if (buffidx >= sizeof(sdbuffer)) {
                                    bmpFile.read(sdbuffer, sizeof(sdbuffer));
                                    buffidx = 0;
                                }
                                b = sdbuffer[buffidx++];
                                g = sdbuffer[buffidx++];
                                r = sdbuffer[buffidx++];
                                awColors[col] = this->tft->color565(r,g,b);
                            }
                            writeRect(0, row, w, 1, awColors);
                        }
                    }
                }
            }
            bmpFile.close();
        }

        void writeRect(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors) {
            tft->setAddrWindow(x, y, x+w-1, y+h-1);
            tft->writecommand(ILI9341_RAMWR);
            for(y=h; y>0; y--) {
                for(x=w; x>1; x--) {
                    tft->pushColor(*pcolors++);
                }
                tft->pushColor(*pcolors++);
            }
        }

        uint16_t read16(File &f) {
          uint16_t result;
          ((uint8_t *)&result)[0] = f.read(); // LSB
          ((uint8_t *)&result)[1] = f.read(); // MSB
          return result;
        }
    
        uint32_t read32(File &f) {
          uint32_t result;
          ((uint8_t *)&result)[0] = f.read(); // LSB
          ((uint8_t *)&result)[1] = f.read();
          ((uint8_t *)&result)[2] = f.read();
          ((uint8_t *)&result)[3] = f.read(); // MSB
          return result;
        }

        int ringMeter(int value, int vmin, int vmax, int x, int y, int r, char *units, byte scheme) {
            x += r; y += r;   // Calculate coords of centre of ring
            int w = r / 4;    // Width of outer ring is 1/4 of radius
            int angle = 150;  // Half the sweep angle of meter (300 degrees)
            int text_colour = 0; // To hold the text colour
            int v = map(value, vmin, vmax, -angle, angle); // Map the value to an angle v
            
            byte seg = 5; // Segments are 5 degrees wide = 60 segments for 300 degrees
            byte inc = 5; // Draw segments every 5 degrees, increase to 10 for segmented ring
            
            // Draw colour blocks every inc degrees
            for (int i = -angle; i < angle; i += inc) {
                int colour = 0;
                switch (scheme) {
                    case 0: colour = ILI9341_RED;   break;
                    case 1: colour = ILI9341_GREEN; break;
                    case 2: colour = ILI9341_BLUE;  break;
                    case 3: colour = rainbow(map(i, -angle, angle, 0, 127));  break; // Full spectrum blue to red
                    case 4: colour = rainbow(map(i, -angle, angle, 63, 127)); break; // Green to red (high temperature etc)
                    case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break; // Red to green (low battery etc)
                    default: colour = ILI9341_BLUE; break;
                }
                
                // Calculate pair of coordinates for segment start
                float sx = cos((i - 90) * 0.0174532925);
                float sy = sin((i - 90) * 0.0174532925);
                uint16_t x0 = sx * (r - w) + x;
                uint16_t y0 = sy * (r - w) + y;
                uint16_t x1 = sx * r + x;
                uint16_t y1 = sy * r + y;
                
                // Calculate pair of coordinates for segment end
                float sx2 = cos((i + seg - 90) * 0.0174532925);
                float sy2 = sin((i + seg - 90) * 0.0174532925);
                int x2 = sx2 * (r - w) + x;
                int y2 = sy2 * (r - w) + y;
                int x3 = sx2 * r + x;
                int y3 = sy2 * r + y;
                
                if (i < v) {
                    this->tft->fillTriangle(x0, y0, x1, y1, x2, y2, colour);
                    this->tft->fillTriangle(x1, y1, x2, y2, x3, y3, colour);
                    text_colour = colour; // Save the last colour drawn
                } else {
                    this->tft->fillTriangle(x0, y0, x1, y1, x2, y2, ILI9341_VERYDARKGREY);
                    this->tft->fillTriangle(x1, y1, x2, y2, x3, y3, ILI9341_VERYDARKGREY);
                }
            }
            
            
            this->tft->setTextSize(2);
            this->tft->setTextColor(text_colour, ILI9341_BLACK);
            this->drawCentreString(String(value), x, y-20);
            this->tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
            this->drawCentreString(String(units), x, y+5);
            return x + r;
        }

        void drawCentreString(String string, int dX, int poY, int size=2) {
            this->tft->setTextSize(size);
            string.trim();
            switch (size) {
                case 1:
                    dX -= ((string.length()*6)-1)/2;
                    break;
                
                default:
                case 2:
                    dX -= ((string.length()*11)-1)/2;
                    break;
                
                case 3:
                    dX -= ((string.length()*16)-1)/2;
                    break;
                
            }
            this->tft->setCursor(dX, poY);
            this->tft->println(string);    
        }

        unsigned int rainbow(byte value) {
            byte red = 0, green = 0, blue = 0;
            byte quadrant = value / 32;
            
            if (quadrant == 0) {
                blue = 31;
                green = 2 * (value % 32);
                red = 0;
            }
            if (quadrant == 1) {
                blue = 31 - (value % 32);
                green = 63;
                red = 0;
            }
            if (quadrant == 2) {
                blue = 0;
                green = 63;
                red = value % 32;
            }
            if (quadrant == 3) {
                blue = 0;
                green = 63 - 2 * (value % 32);
                red = 31;
            }
            return (red << 11) + (green << 5) + blue;
        }

};


