/**
 * Custom log function to print to serial and OLED screen if enable
 */
void Logln(String line) {
  Serial.println("["+TheTime+"]"+line);
}


