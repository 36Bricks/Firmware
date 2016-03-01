//-----------------------------------------------------------
//----------------- FIRMWARE CONFIGURATION ------------------
//-----------------------------------------------------------
//-- Enable or disable some modules and option             --
//-----------------------------------------------------------
//#define MODULE_OUTPUTS
//#define MODULE_TELEINFO_EDF
//#define MODULE_PHOTO
//#define MODULE_DHT22
//#define MODULE_NEOPIXELS
#define MODULE_STRIP
//#define MODULE_SWITCH_RETROFIT
//#define MODULE_MOTION
//#define MODULE_CURRENT
//#define MODULE_ILI9341

#define OPTION_AUTO_UPDATE
#define OPTION_SSDP
#define OPTION_DISCOVERY
#define OPTION_MQTT
#define OPTION_NTP

//-----------------------------------------------------------
//-- Select one of the following brick types              --
//-----------------------------------------------------------
#define BRICK_TYPE "MultiSensor"
//#define BRICK_TYPE "Plug"
//#define BRICK_TYPE "DualPlug"
//#define BRICK_TYPE "Muscle"

#define FIRMWARE_VERSION "36Brick Firmware v0.42.0"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
extern "C" {
    #include "user_interface.h"
}
ADC_MODE(ADC_VCC);

#include "Globals.h"
#include "Settings.h"
#include "Module.h"
#include "Option.h"
#include "FS.h"
#include "SPIFFS.h"
#include "MainWifi.h"
#include "MainServer.h"

#if defined(OPTION_NTP)                         // NTP time sync service setup
    #include <TimeLib.h> 
    #include <WiFiUdp.h>
    #define NTP_SERVER_NAME "fr.pool.ntp.org"   // NTP server (or pool) adress
    #define NTP_INTERVAL_MS 500                 // Local time update interval : 0.5 seconds
    #define NTP_TIME_ZONE 1                     // Central European Time
    #define UDP_LOCAL_PORT 2390                 // Local UDP port for NTP answer
    #include "NTP.h"
    ntpOption NTP;
    #include "getNtpTime.h"
#endif

#if defined(OPTION_MQTT)                    // MQTT option setup
    #include <PubSubClient.h>
    #define MQTTprefix "36brick/"           // MQTT topic prefix for all publishes
    #define MQTT_ESSAIS_MAX 5               // Connection tries before fail
    #include "MQTT.h"
    mqttOption MQTT;
#endif

#if defined(OPTION_AUTO_UPDATE)             // Auto Update option
    #include <ESP8266HTTPClient.h>
    #include <ESP8266httpUpdate.h>
    #define AUTO_UPDATE_URL  "http://192.168.2.8/bricks/update.php" // Update server url
    #include "AutoUpdate.h"
    autoUpdateOption autoUpdate;
#endif

#if defined(OPTION_DISCOVERY)               // Discovery option
    #include <ESP8266Ping.h>                // Ping library
    #include "Discovery.h"
    discoveryOption discovery;
#endif

#if defined(OPTION_SSDP)                    // SSDP option
    #include <ESP8266SSDP.h>
    #include "SSDP.h"
    ssdpOption SSPD;
#endif

#if defined(MODULE_OUTPUTS)                 // Outputs module
    #include "Outputs.h"
    outputModule outputsModule1(D0, D1);    // Change relays pins here
#endif

#if defined(MODULE_DHT22)                   // DHT22 module
    #include <DHT.h>                        // DHT library
    #define DHTTYPE DHT22                   // Sensor type : DHT22
    #define DHT22_INTERVAL_MS 5000          // read interval
    #include "DHT22.h"
    dht22Module dht22Module1(D4, D8);       // Change sensor pins here
#endif
     
#if defined(MODULE_PHOTO)                   // Photosensor module
    #define PHOTOSENSOR_INTERVAL_MS 1000    // update interval
    #include "PhotoLevel.h"
    photoModule photoModule1(A0);           // Change sensor pin here
#endif
   
#if defined(MODULE_CURRENT)                 // Current sensor module 
    #define CURRENT_INTERVAL_MS 1000        // Current sensor update interval
    #define CURRENT_VOLTAGE_SCALE 3300      // 3.3v max scale
    #define CURRENT_MV_PER_AMP    66        // 66mV per amp scale (-30A to 30A range)
    #define CURRENT_MIDDLE_POINT  2500      // analog mV (sensor VCC/2) offset to zero from sensor (tested on proto sensor)
    #include "Current.h"
    currentModule currentModule1(A0);       // Change sensor pin here
#endif 

#if defined(MODULE_MOTION)                  // Motion sensor module setup
    #define MOTIONSENSOR_INTERVAL_MS 100    // Motion sensor update interval
    #include "Motion.h"
    motionModule motionModule1(D2);         // Change sensor pin here
#endif

#if defined(MODULE_NEOPIXELS)                   // NeoPixels module setup
    #include <Adafruit_NeoPixel.h>              // NeoPixels library
    #define NEOPIXELS_NUM 8                     // How many pixels ?
    #define NEOPIXELS_RAINBOW_INTERVAL_MS 12    // Rainbow loop update interval
    #define NEOPIXELS_COLOR_ORDER NEO_GRB       // Color order of pixels (NEO_GRB or NEO_RGB)
    #define NEOPIXELS_FREQUENCY NEO_KHZ800      // NeoPixels PWM frequency (NEO_KHZ400 or NEO_KHZ800)
    #include "NeoPixels.h"
    neoPixelsModule neoPixelsModule1(D5);       // Change pin here
#endif
    
#if defined(MODULE_STRIP)                   // RGB Led strip module setup
    #define STRIP_INTERVAL_MS 12            // RGB Strip loop interval
    #include <RGBdriver.h>
    #include "Strip.h"
    stripModule stripModule1(D7, D6);       // Change strip pins here
#endif

#if defined(MODULE_TELEINFO_EDF)            // Teleinfo EDF module setup
    #include "TeleInfoEDF.h"
    teleinfoModule teleinfoModule1(D5);     // Change pin here
#endif

#if defined(MODULE_SWITCH_RETROFIT)                     // SwitchRetrofit module setup
    #define SW_RETRO_INTERVAL_MS 50                     // Main loop interval time
    #include "SwitchRetrofit.h"
    switchRetrofitModule switchRetrofitModule1(D0, D1); // Change pins here
#endif
     
#if defined(MODULE_ILI9341)                 // ILI9341 module
    #include "SPI.h"
    #include "Adafruit_GFX.h"
    #include "Adafruit_ILI9341.h"
    #define ILI9341_INTERVAL_MS 1000
    #define ILI9341_NB_LOG 28
    #include "ILI9341.h"
    ili9341Module ili9341Module1(2, 5);     // Change tft pins here
    void addLogLineToScreen(String line) {
        ili9341Module1.addLogLine(line);
    }
#endif

/**
 * Main setup : setups all chosen modules and options 
 */
void setup() {

    #if defined(MODULE_ILI9341)
        ili9341Module1.earlySetup();
    #endif
    Log::setup();
    Log::Logln("[NFO] Power ON - " + String(FIRMWARE_VERSION));
    Log::Logln("[NFO] Serial started");  
    
    Settings::read();                       // Read config from EEPROM
    SpiFfs::setup();                        // Initialize SPIFFS 
    MainWifi::setup();                      // Connects to user wifi

    // Main setup of all instantiated options
    for (Option *o = listOptions; o; o = o->nextOption)
        o->setup();
    
    // Main setup of all instantiated modules
    for (Module *r = listModules; r; r = r->nextModule)
        r->setup();
     
    MainServer::setup();                    // setups the main http endpoints
    
    Log::Logln("[NFO] Brick started, beginning operations ...");  
}

/**
 * Main loop : calls all modules and options loops
 */
void loop() {
  
    // Main loop of all instantiated options
    for (Option *o = listOptions; o; o = o->nextOption)
        o->loop();
    
    // Main loop of all instantiated modules
    for (Module *r = listModules; r; r = r->nextModule)
        r->loop();
        
    MainServer::server.handleClient();      // Handle all clients connected to server
}

