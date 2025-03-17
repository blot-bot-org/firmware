#include <interface.h>
#include <definitions.h>
#include <WiFi.h>

void esp32_exit() {
    WiFi.mode(WIFI_OFF);

    Serial.println("BlotBot could not start due to the error above. The ESP32 will automatically restart in 30 seconds...");
    ESP.deepSleep(1000000 * 30); // sleep for a day (forever basically)
    // the deepSleep() function stops all code execution. So it can be treated as an exit();
}
