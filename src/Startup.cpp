#include "Startup.h"
#include "Camera.h"
#include "Globals.h"
#include "ScreenLogger.h"
#include "env.h"

extern ScreenLogger logger;

void initializeStartup() {
    logger.begin();
    logger.println("Starting...");
    pinMode(PROCESSING_LED_PIN, OUTPUT);
    digitalWrite(PROCESSING_LED_PIN, LOW);

    // Initialize Wi-Fi connection
    connectToWiFi();

    // Initialize the camera
    // initializeCamera();
}

void connectToWiFi() {
    WiFi.mode(WIFI_STA);
    logger.print("Connecting to WiFi: " + String(WIFI_SSID) + "...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        logger.print(".");
    }

    IPAddress ip = WiFi.localIP();
    String ipStr = ip.toString();
    logger.println("\nConnected to " + String(WIFI_SSID) +
                   ". IP Address: " + ipStr);
}
