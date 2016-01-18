#define __VERSION__ "36Brick Firmvare v0.3"

//-----------------------------------------------------------
//--------------- CONFIGURATION DU FIRMWARE -----------------
//-----------------------------------------------------------
//-- Activer les différents modules et options du firmware --
//-----------------------------------------------------------
#define MODULE_OUTPUTS
//#define MODULE_TELEINFO_EDF
#define MODULE_OLED
#define MODULE_PHOTO
#define MODULE_DHT22
#define MODULE_NEOPIXELS
#define MODULE_STRIP

#define OPTION_MQTT
#define OPTION_NTP

//-----------------------------------------------------------
//--------------------- TYPE DE MODULE ----------------------
//-----------------------------------------------------------
//-- Sélectionner un des différents types ci-dessous       --
//-----------------------------------------------------------
#define BRICK_TYPE "MultiSensor"
//#define BRICK_TYPE "MultiSensorPlus"
//#define BRICK_TYPE "Plug"
//#define BRICK_TYPE "DualPlug"
//#define BRICK_TYPE "Muscle"

//--------------- WIFI ------------------
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
ESP8266WebServer server(80);

bool wifiOK = false;

void setup() {
  Serial.begin(115200);
  
  Logln("");
  #if defined(MODULE_OLED)
    OLEDsetup();
  #endif

  Logln("[NFO] Power ON");
  Logln("[NFO] Serial started");  
  ReadStoredConfig();

  #if defined(MODULE_NEOPIXELS)
    NeoPixelsSetup();
  #endif
  
  #if defined(MODULE_STRIP)
    stripSetup();
  #endif

  #if defined(OPTION_MQTT)
    MQTTsetup();
  #endif

  #if defined(MODULE_OUTPUTS)
    OutputSetup();
  #endif

  #if defined(MODULE_PHOTO)
    PhotoSetup();
  #endif

  #if defined(MODULE_DHT22)
    Dht22Setup();
  #endif

  #if defined(MODULE_TELEINFO_EDF)
    EDFsetup();
  #endif


  setupWifi();
  setupConfigFromWifi();
  wifiUpdateSetup();
  
  server.begin();
  MDNS.addService("http", "tcp", 80);
  
  #if defined(OPTION_NTP)
    ntpSetup();
  #endif


}

void loop() {
  
  #if defined(OPTION_NTP)
    ntpLoop();
  #endif

  #if defined(MODULE_NEOPIXELS)
    NeoPixelsLoop();
  #endif
  
  #if defined(MODULE_STRIP)
    stripLoop();
  #endif

  #if defined(OPTION_MQTT)
    MQTTloop();
  #endif
  
  #if defined(MODULE_PHOTO)
    PhotoLoop();
  #endif

  #if defined(MODULE_DHT22)
    Dht22Loop();
  #endif

  #if defined(MODULE_TELEINFO_EDF)
    EDFloop();
  #endif

  #if defined(MODULE_OLED)
    OLEDloop();
  #endif

  server.handleClient();

}

