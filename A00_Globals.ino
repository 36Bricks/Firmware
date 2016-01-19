/**
 * Globals definitions
 */

#include <elapsedMillis.h>

String TheTime = "BOOT-TIME";     // Local time for logs

#if defined(MODULE_OLED)
  #define BLACK 0
  #define WHITE 1
  #define INVERSE 2
  
  void OLEDwriteln(String text, int textSize=1, unsigned int textColor=WHITE, unsigned int bgColor=BLACK );
#endif

char* host = "36brick";
#define MQTTuser "36Brick"
#define MQTTpassword ""


