#ifndef HEALTH_CHECK_H
#define HEALTH_CHECK_H

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

/**
 * @brief Handles the health-check request by sending a 200 OK response.
 *
 * @param request Pointer to the AsyncWebServerRequest object.
 * @param doc     Reference to the JsonDocument (unused in this case).
 */
void processHealthCheckRequest(AsyncWebServerRequest *request,
                               const JsonDocument &doc);

#endif // HEALTH_CHECK_H
