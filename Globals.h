/**
 * Globals definitions
 */

String TheTime = "BOOT";     // Local time for logs

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
#if defined(MODULE_ILI9341)
    void addLogLineToScreen(String);
#endif

/**
 * Custom log function to print to serial and OLED screen if enable
 */
namespace Log {
    void Logln(String line) {
        Serial.println("["+TheTime+"]"+line);
        #if defined(MODULE_ILI9341)
            addLogLineToScreen("["+TheTime+"]"+line);
        #endif
    }
    void setup() {
        Serial.begin(115200);  // Starts serial communication for log and debug purposes
        Log::Logln("");
    }
}


