#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "Globals.h"
#include "ScreenLogger.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

typedef std::function<void(AsyncWebServerRequest *, const JsonDocument &)>
    RequestProcessor;

void handleRequest(AsyncWebServerRequest *request, uint8_t *data, size_t len,
                   size_t index, size_t total, RequestProcessor processor,
                   ScreenLogger &screen) {
    static String body;

    if (index == 0) {
        body = "";
        digitalWrite(LED_PIN, HIGH); // Turn on LED when processing starts
    }

    body += String((char *)data).substring(0, len);

    if ((index + len) == total) {
        // Log the body
        screen.println(body);

        // Parse JSON
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, body);
        if (error) {
            request->send(400, "application/json",
                          "{\"error\":\"Invalid JSON\"}");
            digitalWrite(LED_PIN, LOW);
            return;
        }

        // Process the request with the provided processor function
        processor(request, doc);

        digitalWrite(LED_PIN, LOW); // Turn off LED after processing
    }
}

#endif // REQUESTHANDLER_H
