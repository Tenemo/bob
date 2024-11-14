#include "Globals.h"
#include "RequestHandler.h"
#include "ScreenLogger.h"
#include "env.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

AsyncWebServer server(80);
ScreenLogger screen;

// Core processing function for /rotate endpoint
void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc) {
    const char *direction = doc["direction"];
    int degrees = doc["degrees"];

    // Validate direction
    if (!direction || (strcmp(direction, "clockwise") != 0 &&
                       strcmp(direction, "counterclockwise") != 0)) {
        req->send(400, "application/json", "{\"error\":\"Invalid direction\"}");
        return;
    }

    // TODO: Implement rotation logic based on direction and degrees

    // Send success response
    DynamicJsonDocument responseDoc(200);
    responseDoc["status"] = "success";
    responseDoc["direction"] = direction;
    responseDoc["degrees"] = degrees;
    String response;
    serializeJson(responseDoc, response);
    req->send(200, "application/json", response);
}

void setup() {
    Serial.begin(115200);
    screen.begin();
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    screen.println("LED initialized to OFF");

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    screen.println("Connecting to WiFi: " + String(WIFI_SSID) + "...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        screen.print(".");
    }

    screen.println("");
    screen.println("WiFi connected!");
    screen.print("IP Address: ");

    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) +
                   "." + String(ip[3]);
    screen.println(ipStr);
    screen.println("");

    server.on(
        "/rotate", HTTP_POST,
        [](AsyncWebServerRequest *request) {
            // Empty onRequest handler
        },
        NULL,
        [&](AsyncWebServerRequest *request, uint8_t *data, size_t len,
            size_t index, size_t total) {
            handleRequest(request, data, len, index, total,
                          processRotateRequest, screen);
        });

    server.begin();
    screen.println("Server started");
}

void loop() {}
