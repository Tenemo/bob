#include "env.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "src/Camera.h"
#include "src/Globals.h"
#include "src/RequestHandler.h"
#include "src/Startup.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

ScreenLogger logger;
AsyncWebServer server(80);

void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc) {
    const char *direction = doc["direction"];
    int degrees = doc["degrees"];

    if (!direction || (strcmp(direction, "clockwise") != 0 &&
                       strcmp(direction, "counterclockwise") != 0)) {
        req->send(400, "application/json", "{\"error\":\"Invalid direction\"}");
        return;
    }

    // TODO: Implement rotation logic based on direction and degrees

    DynamicJsonDocument responseDoc(200);
    responseDoc["status"] = "success";
    responseDoc["direction"] = direction;
    responseDoc["degrees"] = degrees;
    String response;
    serializeJson(responseDoc, response);
    req->send(200, "application/json", response);
}

void setup() {
    // Initialize logger, camera, and Wi-Fi
    initializeStartup();

    // Define the /capture endpoint
    server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request) {
        handleRequest(request, nullptr, 0, 0, 0, processCaptureRequest);
    });

    // Define the /rotate endpoint
    server.on(
        "/rotate", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total) {
            handleRequest(request, data, len, index, total,
                          processRotateRequest);
        });

    server.begin();
    logger.println("Server started");
}

void loop() {
    delay(5);
    logger.update();
    LEDTimer::update();
}
