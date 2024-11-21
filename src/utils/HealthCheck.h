#ifndef HEALTH_CHECK_H
#define HEALTH_CHECK_H

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

/**
 * @brief Handles the health-check request by sending a 200 OK response.
 *
 * This function processes incoming health-check HTTP requests and responds with
 * a JSON object indicating the server's status.
 *
 * @param request Pointer to the AsyncWebServerRequest object.
 * @param doc     Reference to the JsonDocument containing request data (unused
 * in this case).
 */
void processHealthCheckRequest(AsyncWebServerRequest *request,
                               const JsonDocument &doc);

#endif // HEALTH_CHECK_H
