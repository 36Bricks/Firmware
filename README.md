## 36_Bricks - IOT building blocks
36_Bricks is a blocks system allowing to build custom connected objects. Based on small low-cost independant bricks, create your own IOT device for any need : home automation and alarm, laboratory, smart garden, multimedia, industry...

From power and sensors to RGB lights, from data collection to processing and control, there's a brick for any need, and everything is available through Wifi. 


## ESP8266 - NodeMCU LUA - ArduinoIDE
The main part of each brick is an ESP8266 ESP12-E. It handles its 10 GPIO and WIFI to connect bricks to the world.

This git project is the embedded software (firmware) for ESP8266, flashed on each brick using the Arduino IDE.

## Requirements
Arduino IDE 1.6.4 or better : https://www.arduino.cc/en/Main/Software

ESP8266 Board Package for Arduino IDE : https://github.com/esp8266/Arduino

Adafruit DHT22 Arduino library : https://github.com/adafruit/DHT-sensor-library

Adafruit NeoPixel Arduino library : https://github.com/adafruit/Adafruit_NeoPixel

AltSoftSerial Library : https://github.com/PaulStoffregen/AltSoftSerial

Arduino Client for MQTT : http://pubsubclient.knolleary.net/

## Installation
Follow this guide to enable and setup ESP8266 in Arduino ide : https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide

First rename the source folder to "_36_Bricks" as Arduino IDE need it to find the main file by name.

Open the project with Arduino IDE, add all needed libraries using "Sketch -> Include library -> Manage libraries ..." and flash it on the ESP.

## Modules
The firmware is devided into modules. Modules can be enabled and disabled according to your needs just before flashing it to the brick.

Actual modules :
#### ConfigFromWifi (mandatory)
Chromecast-like configuration over Wifi. For first-time and failover Wifi configuration, the module creates its own Wifi AP, and serves an HTML configuration page to perform full configuration of the brick : Brick name, Wifi SSID and password, MQTT server and port (if MQTT module enabled)

#### WifiUpdate (mandatory)
Allows to flash new firmware through WiFi.

#### EEPROM (mandatory)
Manages the storage of brick configuration on embedded EEPROM to save them upon reboot and long-term power off.

#### NTP
Time sync of the brick

#### MQTT
Allows posting to any MQTT broker

#### DHT22
Temperature and Humidity sensor, available on HTTP API and over MQTT

#### PhotoLevel
Light level sensor, available on HTTP API and over MQTT

#### TeleInfoEDF
Gather informations from domestic EDF electric counter, available on HTTP API and over MQTT

#### NeoPixels
Drives up to 1024 adressable RGB Leds using HTTP API

#### OLED
Drives a small 128x64px OLED screen (only for debug atm)

#### Outputs
Drives two 220v relays using HTTP API

#### Strip
Drives an RGB LED Strip using HTTP API

#### Motion
Motion sensing using HC-SR501 Infrared PIR Motion Sensor, available on HTTP API and over MQTT


## Plans
More firmware modules will come allowing to build new bricks : Doors, Plants, Gaz sensors, Physical command buttons, Thermostat, Energy monitoring, IP Camera ...

## Author
Alexandre Garcia - PsY4

## License and credits
This code is released under the MIT License.

Arduino IDE is developed and maintained by the Arduino team. The IDE is licensed under GPL.

ESP8266 core files are licensed under LGPL.

ESP8266 core includes an xtensa gcc toolchain, which is also under GPL.

Adafruit DHT22 and NeoPixels Arduino libraries are developed and maintained by the Adafuit team.

AltSoftSerial Arduino library is developed and maintained by Paul Stoffregen

Arduino Client for MQTT Arduino library is developed and maintained by Nick O'Leary, and licensed under the MIT License