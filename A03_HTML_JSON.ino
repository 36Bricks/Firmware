/***
 * HTML and JSON constants and templates
 */

// TODO : Make a beautifull firmware update page

#define ReturnOK "{ \"ret\": \"OK\" }\r\n"

#define ConfigPage "<!DOCTYPE html><html>" \
"<head><meta charset='UTF-8'/><meta name='viewport' content='width=device-width/2, initial-scale=2.0'><title>36 Brick Setup</title></head>" \
"<body style='background-color:#565656;color:#dddddd; font-family:Verdana,Geneva,sans-serif'>" \
"<style>input,select{width:100%}</style>" \
"<div style='float:right;text-align:right;line-height:15px;'>36Brick<br>%%TYPE%%</div>" \
"<form method='GET' action='/saveCFG' style='clear:both'><label>Brick Name<br><input name='name' type='text' value='%%NAME%%' /></label><br><br><label>WIFI SSID<br><select name='ssid' >%%SSID%%</select></label><br><label>WIFI Password<br><input name='pass' type='password' value='%%PASS%%' /></label><br><br><label>MQTT Server<br><input name='mqttserv' type='text' value='%%MQTT_SERV%%'/></label><br><label>MQTT Port<br><input name='mqttport' type='text' value='%%MQTT_PORT%%' /></label><br><br><input type='submit'/></form></body>"

#define ThxPage "<!DOCTYPE html><html><head><meta charset='UTF-8'/><meta name='viewport' content='width=device-width/2, initial-scale=2.0'><title>36 Brick Setup</title></head><body style='background-color:#565656;color:#dddddd;font-family:Verdana,Geneva,sans-serif;'><style>input,select{width:100%;}</style><div style='float:right;text-align:right; line-height:15px'>36Brick<br>%%TYPE%%</div><div style='clear:both'><br>Config saved, you can reset the brick !</a></body>"

#define serverUpdate "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>"



