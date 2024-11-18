#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "Globals.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <functional>

typedef std::function<void(AsyncWebServerRequest *, const JsonDocument &)>
    RequestProcessor;

class LEDTimer {
  private:
    static unsigned long ledOnTime;
    static const unsigned long MIN_LED_DURATION = 200; // 200ms minimum ON time

  public:
    static void startProcessing() {
        digitalWrite(PROCESSING_LED_PIN, HIGH);
        ledOnTime = millis();
    }

    static void endProcessing() {
        // Only turn off LED if minimum duration has passed
        unsigned long currentTime = millis();
        if (currentTime - ledOnTime >= MIN_LED_DURATION) {
            digitalWrite(PROCESSING_LED_PIN, LOW);
        }
        // Otherwise, let loop() handle it
    }

    static void update() {
        if (digitalRead(PROCESSING_LED_PIN) == HIGH) {
            unsigned long currentTime = millis();
            if (currentTime - ledOnTime >= MIN_LED_DURATION) {
                digitalWrite(PROCESSING_LED_PIN, LOW);
            }
        }
    }
};

unsigned long LEDTimer::ledOnTime = 0;

void handleRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len,
                   size_t index, size_t total, RequestProcessor processor) {
    static String body;

    if (index == 0) {
        body = ""; // Reset the body at the start of a new request
        LEDTimer::startProcessing(); // Start LED timing

        // Log the request method and endpoint
        String method;
        switch (request->method()) {
        case HTTP_GET:
            method = "GET";
            break;
        case HTTP_POST:
            method = "POST";
            break;
        case HTTP_PUT:
            method = "PUT";
            break;
        case HTTP_DELETE:
            method = "DELETE";
            break;
        default:
            method = "UNKNOWN";
            break;
        }
        logger.println(method + " " + request->url());

        // For GET requests, process immediately without waiting for body
        if (request->method() == HTTP_GET) {
            JsonDocument doc; // Empty JSON doc for GET requests
            processor(request, doc);
            LEDTimer::endProcessing(); // End LED timing
            return;
        }
    }

    // Only handle body for non-GET requests
    if (request->method() != HTTP_GET) {
        // Append incoming data to the body
        if (data) {
            body += String((char *)data).substring(0, len);
        }

        if (index + len == total) { // Check if all data has been received
            JsonDocument doc;

            if (data && len > 0) {
                DeserializationError error = deserializeJson(doc, body);
                if (error) {
                    request->send(400, "application/json",
                                  "{\"error\":\"Invalid JSON\"}");
                    LEDTimer::endProcessing(); // End LED timing
                    return;
                }
            }

            // Process the request with the provided processor function
            processor(request, doc);
            LEDTimer::endProcessing(); // End LED timing
        }
    }
}

#endif // REQUESTHANDLER_H
