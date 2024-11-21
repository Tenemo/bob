#include "HealthCheck.h"
#include <ArduinoJson.h>

/**
 * @brief Handles the health-check request by sending a 200 OK response.
 *
 * @param request Pointer to the AsyncWebServerRequest object.
 * @param doc     Reference to the JsonDocument (unused in this case).
 */
void processHealthCheckRequest(AsyncWebServerRequest *request,
                               const JsonDocument &doc) {
    JsonDocument responseDoc;
    responseDoc["status"] = "OK";
    responseDoc["message"] = "Server is running.";

    String response;
    serializeJson(responseDoc, response);
    request->send(200, "application/json", response);
}
