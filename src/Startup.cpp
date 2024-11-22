#include "Startup.h"
#include "Camera.h"
#include "Globals.h"
#include "Servos.h"
#include "audio/WAVFileReader.h"
#include "env.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
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
    // Workaround to stop speaker popping
    // 10ms of silence
    playAudioFile("/silence.wav");
    delay(10);
    server.begin();
    Serial.println("Web server started.");

    // Perform health check for the server
    const int maxRetries = 3;
    const int retryDelay = 200;
    bool healthCheckPassed = false;

    for (int attempt = 1; attempt <= maxRetries; attempt++) {
        Serial.println("Health check attempt " + String(attempt) + " of " +
                       String(maxRetries));
        if (performHealthCheck()) {
            healthCheckPassed = true;
            successCount++; // Increment successCount only if health check
                            // passes
            break;
        }
        delay(retryDelay);
    }

    if (healthCheckPassed) {
        Serial.println("Web server initialization SUCCESSFULL.");
    } else {
        logger.println("Web server initialization FAILURE.");
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

bool performHealthCheck() {
    if (WiFi.status() != WL_CONNECTED) {
        logger.println("Cannot perform health check: WiFi not connected.");
        return false;
    }

    IPAddress ip = WiFi.localIP();
    String url = "http://" + ip.toString() + "/health-check";
    Serial.println("Performing health check at: " + url);

    HTTPClient http;
    http.begin(url);
    http.setTimeout(500); // 500 ms timeout

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        logger.println("Health check failed. HTTP response code: " +
                       String(httpCode));
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    // Parse JSON response
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        logger.println("Health check JSON parse error: " +
                       String(error.c_str()));
        return false;
    }

    const char *status = doc["status"];
    if (status && String(status) == "OK") {
        Serial.println("Health check successful: Server is running.");
        return true;
    } else {
        logger.println("Health check FAILURE: Unexpected status.");
        return false;
    }
}
