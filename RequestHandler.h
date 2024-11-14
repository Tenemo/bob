#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "Globals.h"
#include "ScreenLogger.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <functional>

typedef std::function<void(AsyncWebServerRequest *, const JsonDocument &)>
    RequestProcessor;

void handleRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len,
                   size_t index, size_t total, RequestProcessor processor,
                   ScreenLogger &screen) {
    static String body;

    if (index == 0) {
        body = ""; // Reset the body at the start of a new request
        digitalWrite(LED_PIN, HIGH); // Turn on LED when processing starts
    }

    // Append incoming data to the body
    body += String((char *)data).substring(0, len);

    if ((index + len) == total) { // Check if all data has been received
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, body);
        if (error) {
            request->send(400, "application/json",
                          "{\"error\":\"Invalid JSON\"}");
            digitalWrite(LED_PIN, LOW); // Turn off LED on error
            return;
        }

        // Log each property, one per line
        JsonObject obj = doc.as<JsonObject>();
        for (JsonPair kv : obj) {
            String key = kv.key().c_str();
            String value;

            if (kv.value().is<String>()) {
                value = kv.value().as<String>();
            } else if (kv.value().is<double>()) {
                value = String(kv.value().as<double>());
            } else if (kv.value().is<bool>()) {
                value = kv.value().as<bool>() ? "true" : "false";
            } else {
                value = "Unsupported Type";
            }

            screen.println(key + ": " + value);
        }

        // Process the request with the provided processor function
        processor(request, doc);

        digitalWrite(LED_PIN, LOW); // Turn off LED after processing
    }
}

#endif // REQUESTHANDLER_H
