
//-----------------------------------------------------------
//----------------- FIRMWARE CONFIGURATION ------------------
//-----------------------------------------------------------
//-- Enable or disable some modules and option             --
//-----------------------------------------------------------
#define MODULE_OUTPUTS
#define MODULE_TELEINFO_EDF
#define MODULE_OLED
#define MODULE_PHOTO
#define MODULE_DHT22
#define MODULE_NEOPIXELS
#define MODULE_STRIP
//#define MODULE_SWITCH_RETROFIT

#define OPTION_MQTT
#define OPTION_NTP

//-----------------------------------------------------------
//----------------------- MODULE TYPE -----------------------
//-----------------------------------------------------------
//-- Select one of the following module types              --
//-----------------------------------------------------------
#define BRICK_TYPE "MultiSensor"
//#define BRICK_TYPE "MultiSensorPlus"
//#define BRICK_TYPE "Plug"
//#define BRICK_TYPE "DualPlug"
//#define BRICK_TYPE "Muscle"

//-----------------------------------------------------------
//---------------------- END OF CONFIG ----------------------
//-----------------------------------------------------------
//-- Nothing to edit bellow this point                     --
//-----------------------------------------------------------
#define FIRMWARE_VERSION "36Brick Firmvare v0.31"
#define HTTP_API_PORT 80

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(HTTP_API_PORT);

bool wifiOK = false;

/**
 * Main setup : setups all chosen modules and options 
 */
void setup() {
  Serial.begin(115200);           // Starts serial communication for log and debug purposes
  
  Logln("");
  #if defined(MODULE_OLED)        // OLED module setup
    OLEDsetup();
  #endif

  Logln("[NFO] Power ON");
  Logln("[NFO] Serial started");  
  
  ReadStoredConfig();             // Read config from EEPROM

  #if defined(MODULE_NEOPIXELS)   // NeoPixels module setup
    neoPixelsSetup();
  #endif
  
  #if defined(MODULE_STRIP)       // RGB Led strip module setup
    stripSetup();
  #endif

  #if defined(OPTION_MQTT)        // MQTT option setup
    MQTTsetup();
  #endif

  #if defined(MODULE_OUTPUTS)     // Outputs module setup
    OutputSetup();
  #endif

  #if defined(MODULE_PHOTO)       // Photosensor module setup
    PhotoSetup();
  #endif

  #if defined(MODULE_DHT22)       // DHT22 module setup
    Dht22Setup();
  #endif

  #if defined(MODULE_TELEINFO_EDF)  // Teleinfo EDF module setup
    EDFsetup();
  #endif

  #if defined(MODULE_SWITCH_RETROFIT) // SwitchRetrofit module setup
    switchRetrofitSetup();
  #endif

  setupWifi();                // Connects to user wifi
  setupConfigFromWifi();      // setups the config web page
  wifiUpdateSetup();          // setups the firmware update web page
  
  server.begin();             // Starts the web server to handle all HTTP requests
  MDNS.addService("http", "tcp", 80);
  
  #if defined(OPTION_NTP)     // NTP time sync service setup
    ntpSetup();     
  #endif

}

/**
 * Main loop : calls all modules and options loops
 */
void loop() {
  
  #if defined(OPTION_NTP)       // NTP time sync loop
    ntpLoop();
  #endif

  #if defined(MODULE_NEOPIXELS) // NeoPixels module loop
    neoPixelsLoop();
  #endif
  
  #if defined(MODULE_STRIP)     // RGB Led Strip module loop
    stripLoop();
  #endif

  #if defined(OPTION_MQTT)      // MQTT option loop
    MQTTloop();
  #endif
  
  #if defined(MODULE_PHOTO)     // Photosensor module loop
    PhotoLoop();
  #endif

  #if defined(MODULE_DHT22)     // DHT22 module loop
    Dht22Loop();
  #endif

  #if defined(MODULE_TELEINFO_EDF)  // Teleinfo EDF module loop
    EDFloop();
  #endif

  #if defined(MODULE_OLED)      // OLED screen module loop
    OLEDloop();
  #endif

  #if defined(MODULE_SWITCH_RETROFIT) // SwitchRetrofit module loop
    switchRetrofitLoop();
  #endif

  server.handleClient();      // Handle all clients connected to server

}
