/**
 * Globals definitions
 */
#include <elapsedMillis.h>
#define SERVE_FROM_FLASH_PACKET_SIZE 1024

String TheTime = "BOOT-TIME";     // Local time for logs

char* host = "36brick";
#define MQTTuser "36Brick"
#define MQTTpassword ""


/**
 * float to char*, with precision
 */
char *ftoa(char *a, double f, int precision) {
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
 
 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 while (*a != '\0') a++;
 *a++ = '.';
 long desimal = abs((long)((f - heiltal) * p[precision]));
 itoa(desimal, a, 10);
 return ret;
}

/**
 * Custom log function to print to serial and OLED screen if enable
 */
void Logln(String line) {
  Serial.println("["+TheTime+"]"+line);
}

/***
 * JSON constants
 */
#define ReturnOK "{ \"ret\": \"OK\" }\r\n"

