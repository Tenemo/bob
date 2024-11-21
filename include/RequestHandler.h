#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "Globals.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <functional>

/**
 * @typedef RequestProcessor
 * @brief A function type for processing web server requests.
 *
 * @param request Pointer to the AsyncWebServerRequest object.
 * @param doc     Reference to the JsonDocument containing request data.
 */
typedef std::function<void(AsyncWebServerRequest *, const JsonDocument &)>
    RequestProcessor;

/**
 * @brief Handles incoming web server requests, managing LED state and JSON
 * processing.
 *
 * This function manages the lifecycle of a web request, including logging,
 * LED indication, and JSON deserialization for non-GET requests.
 *
 * @param request   Pointer to the AsyncWebServerRequest object.
 * @param data      Pointer to the incoming data buffer.
 * @param len       Length of the incoming data chunk.
 * @param index     Current index of the data chunk.
 * @param total     Total length of the incoming data.
 * @param processor Function to process the request with the parsed JSON
 * document.
 */
void handleRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len,
                   size_t index, size_t total, RequestProcessor processor) {
    static String body;

    if (index == 0) {
        body = ""; // Reset the body at the start of a new request
        digitalWrite(PROCESSING_LED_PIN, HIGH); // Turn on LED

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
            digitalWrite(PROCESSING_LED_PIN, LOW); // Turn off LED
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
                    digitalWrite(PROCESSING_LED_PIN, LOW); // Turn off LED
                    return;
                }
            }

            // Process the request with the provided processor function
            processor(request, doc);
            digitalWrite(PROCESSING_LED_PIN, LOW); // Turn off LED
        }
    }
}

#endif // REQUESTHANDLER_H
