#include "Startup.h"
#include "Camera.h"
#include "Globals.h"
#include "Servos.h"
#include "env.h"
#include <SPIFFS.h>

void initializeStartup() {
    logger.begin();
    logger.println("Starting...");
    pinMode(PROCESSING_LED_PIN, OUTPUT);
    digitalWrite(PROCESSING_LED_PIN, LOW);

    // WiFi
    // Camera
    // Servos
    // SPIFFS
    // Web server
    const int totalSubsystems = 5;
    int successCount = 0;

    if (connectToWiFi()) {
        successCount++;
    }
    if (initializeCamera()) {
        successCount++;
    }
    if (initializeServos()) {
        successCount++;
    }
    if (SPIFFS.begin(true)) {
        Serial.println("Mounting SPIFFS SUCCESSFUL.");
        successCount++;
    } else {
        logger.println("Mounting SPIFFS FAILURE.");
    }
    if (successCount == totalSubsystems) {
        logger.println("ALL " + String(successCount) + "/" +
                       String(totalSubsystems) +
                       " subsystems initialized\nSUCCESSFULLY!");
    } else {
        logger.println("Failures occurred, initialized " +
                       String(successCount) + "/" + String(totalSubsystems) +
                       " subsystems successfully.");
    }
    server.begin();
    logger.println("Web server started.");
    successCount++;
}
bool connectToWiFi() {
    WiFi.mode(WIFI_STA);
    logger.print("Connecting to WiFi: " + String(WIFI_SSID) + "...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();
    const unsigned long wifiTimeout = 10000; // 10 seconds

    // Wait for connection or timeout
    while (WiFi.status() != WL_CONNECTED &&
           millis() - startAttemptTime < wifiTimeout) {
        delay(250);
        logger.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        IPAddress ip = WiFi.localIP();
        String ipStr = ip.toString();
        logger.println("\nConnected to " + String(WIFI_SSID) +
                       ". IP Address: " + ipStr);
        return true;
    } else {
        logger.println("\nFailed to connect to WiFi: " + String(WIFI_SSID));
        return false;
    }
}
