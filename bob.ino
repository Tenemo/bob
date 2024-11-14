#include "Globals.h"
#include "ScreenLogger.h"
#include "env.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

AsyncWebServer server(80);
ScreenLogger screen;

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

    screen.println("\nWiFi connected!");
    screen.print("IP Address: ");
    screen.println(WiFi.localIP().toString());

    server.on(
        "/rotate", HTTP_POST,
        [](AsyncWebServerRequest *request) {
            // Empty handler
        },
        NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total) {
            static String body;
            if (index == 0) {
                body = "";
                digitalWrite(LED_PIN,
                             HIGH); // Turn on LED when processing starts
            }

            body += String((char *)data).substring(0, len);

            if ((index + len) == total) {
                // Parse JSON
                DynamicJsonDocument doc(256);
                DeserializationError error = deserializeJson(doc, body);
                if (error) {
                    request->send(400, "application/json",
                                  "{\"error\":\"Invalid JSON\"}");
                    digitalWrite(LED_PIN, LOW);
                    return;
                }

                const char *direction = doc["direction"];
                int degrees = doc["degrees"];

                // Validate direction
                if (!direction ||
                    (strcmp(direction, "clockwise") != 0 &&
                     strcmp(direction, "counterclockwise") != 0)) {
                    request->send(400, "application/json",
                                  "{\"error\":\"Invalid direction\"}");
                    digitalWrite(LED_PIN, LOW);
                    return;
                }

                // Display request on screen
                String displayMsg = "Direction: " + String(direction) +
                                    ", Degrees: " + String(degrees);
                screen.println(displayMsg);

                // TODO: Implement rotation logic based on direction and degrees

                // Send success response
                DynamicJsonDocument responseDoc(200);
                responseDoc["status"] = "success";
                responseDoc["direction"] = direction;
                responseDoc["degrees"] = degrees;
                String response;
                serializeJson(responseDoc, response);
                request->send(200, "application/json", response);

                digitalWrite(LED_PIN, LOW); // Turn off LED after processing
            }
        });

    server.begin();
    screen.println("Server started");
}

void loop() {
    // No actions needed in loop
}
