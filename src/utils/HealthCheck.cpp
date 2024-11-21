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
    // Create a JSON response indicating the server status
    StaticJsonDocument<100> responseDoc;
    responseDoc["status"] = "OK";
    responseDoc["message"] = "Server is running.";

    String response;
    serializeJson(responseDoc, response);

    // Send the response with status 200 and content type application/json
    request->send(200, "application/json", response);
}
