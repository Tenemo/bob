#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "Globals.h"
#include "RequestHandler.h"
#include "env.h"

ScreenLogger logger;

AsyncWebServer server(80);

void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc) {
    const char *direction = doc["direction"];
    int degrees = doc["degrees"];

    if (!direction || (strcmp(direction, "clockwise") != 0 &&
                       strcmp(direction, "counterclockwise") != 0)) {
        req->send(400, "application/json", "{\"error\":\"Invalid direction\"}");
        return;
    }

    // TODO: Implement rotation logic based on direction and degrees

    DynamicJsonDocument responseDoc(200);
    responseDoc["status"] = "success";
    responseDoc["direction"] = direction;
    responseDoc["degrees"] = degrees;
    String response;
    serializeJson(responseDoc, response);
    req->send(200, "application/json", response);
}

void setup() {
    logger.begin();
    logger.println("Starting...");
    pinMode(PROCESSING_LED_PIN, OUTPUT);
    digitalWrite(PROCESSING_LED_PIN, LOW);

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    logger.println("Connecting to WiFi: " + String(WIFI_SSID) + "...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        logger.print(".");
    }

    logger.println("");
    logger.println("WiFi connected.");
    logger.print("IP Address: ");

    IPAddress ip = WiFi.localIP();
    String ipStr = ip.toString();
    logger.println(ipStr);
    logger.println("");

    server.on(
        "/rotate", HTTP_POST,
        [](AsyncWebServerRequest *request) {
            // Empty onRequest handler
        },
        NULL,
        [&](AsyncWebServerRequest *request, uint8_t *data, size_t len,
            size_t index, size_t total) {
            handleRequest(request, data, len, index, total,
                          processRotateRequest);
        });

    server.begin();
    logger.println("Server started");
}

void loop() {}
