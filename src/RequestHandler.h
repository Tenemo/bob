#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "Globals.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <functional>

typedef std::function<void(AsyncWebServerRequest *, const JsonDocument &)>
    RequestProcessor;

void handleRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len,
                   size_t index, size_t total, RequestProcessor processor) {
    static String body;

    if (index == 0) {
        body = ""; // Reset the body at the start of a new request
        digitalWrite(PROCESSING_LED_PIN,
                     HIGH); // Turn on LED when processing starts

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
            DynamicJsonDocument doc(512); // Empty JSON doc for GET requests
            processor(request, doc);
            digitalWrite(PROCESSING_LED_PIN, LOW);
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
            DynamicJsonDocument doc(512);

            if (data && len > 0) {
                DeserializationError error = deserializeJson(doc, body);
                if (error) {
                    request->send(400, "application/json",
                                  "{\"error\":\"Invalid JSON\"}");
                    digitalWrite(PROCESSING_LED_PIN, LOW);
                    return;
                }
            }

            // Process the request with the provided processor function
            processor(request, doc);
            digitalWrite(PROCESSING_LED_PIN, LOW);
        }
    }
}

#endif // REQUESTHANDLER_H
