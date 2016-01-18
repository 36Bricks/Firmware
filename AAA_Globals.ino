//-----------------------------------------------------------
//---------------------- GLOBALS ----------------------------
//-----------------------------------------------------------

#include <elapsedMillis.h>

String TheTime = "BOOT-TIME";

#if defined(MODULE_OLED)
  #define BLACK 0
  #define WHITE 1
  #define INVERSE 2
  
  void OLEDwriteln(String text, int textSize=1, unsigned int textColor=WHITE, unsigned int bgColor=BLACK );
#endif

String ReturnOK = "{ \"ret\": \"OK\" }\r\n";

String ConfigPage = "<!DOCTYPE html><html>"
"<head>    <meta charset='UTF-8'/>   <meta name='viewport' content='width=device-width/2, initial-scale=2.0'>    <title>36 Brick Setup</title> </head>"
"<body style='background-color:#565656; color:#dddddd; font-family:Verdana, Geneva, sans-serif;'> "
"<style>   input, select {width:100%;} </style>  "
"<div style='float:right; text-align:right;  line-height:15px;'>36Brick<br />%%TYPE%%</div>  "
"<form method='GET' action='/saveCFG' style='clear:both;'>   <label>Brick Name<br />     <input name='name' type='text' value='%%NAME%%' />    </label><br />    <br />    <label>WIFI SSID<br />      <select name='ssid' >%%SSID%%</select>    </label><br />    <label>WIFI Password<br />        <input name='pass' type='password' value='%%PASS%%' />    </label><br />    <br />    <label>MQTT Server<br />      <input name='mqttserv' type='text' value='%%MQTT_SERV%%' />   </label><br />    <label>MQTT Port<br />      <input name='mqttport' type='text' value='%%MQTT_PORT%%' />   </label><br />    <br />    <input type='submit' /> </form></body>";

String ThxPage = "<!DOCTYPE html><html>  <head>    <meta charset='UTF-8'/>   <meta name='viewport' content='width=device-width/2, initial-scale=2.0'>    <title>36 Brick Setup</title> </head><body style='background-color:#565656; color:#dddddd; font-family:Verdana, Geneva, sans-serif;'> <style>   input, select {width:100%;} </style>  <div style='float:right; text-align:right;  line-height:15px;'>36Brick<br />%%TYPE%%</div>  <div style='clear:both'><br />Config saved, you can reset the brick !</a></body>";

const char* serverUpdate = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";



char* host = "36brick";
#define MQTTuser "36Brick"
#define MQTTpassword ""

//--------------- EEPROM ------------------
#include <EEPROM.h>

int eepromSSIDOffset = 0;
int eepromSSIDValidOffset = 54;

int eepromPASSOffset = 64;
int eepromPASSValidOffset = 118;

int eepromNameOffset = 128;
int eepromNameValidOffset = 182;

struct SaveSSID { char ssid[48]; }; bool ssidOK;
struct SavePASS { char pass[48]; }; bool passOK;
struct SaveName { char name[48]; }; bool nameOK;

SaveSSID retreivedSSID;
SavePASS retreivedPASS;
SaveName retreivedName;

#if defined(OPTION_MQTT)
  int eepromMQTTservOffset = 192;
  int eepromMQTTservValidOffset = 246;
  
  int eepromMQTTportOffset = 256;
  int eepromMQTTportValidOffset = 310;
  
  struct SaveMQTTserv { char serv[48]; }; bool servOK;
  struct SaveMQTTport { char port[10]; }; bool portOK;
  
  SaveMQTTserv retreivedMQTTserv;
  SaveMQTTport retreivedMQTTport;
#endif

int eepromLength = 320;

struct SaveValid { char valid[5]; };
const char* saveValid = "true";

