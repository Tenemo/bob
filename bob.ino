#include "env.h"
#include "src/Camera.h"
#include "src/Globals.h"
#include "src/RequestHandler.h"
#include "src/Servos.h"
#include "src/Startup.h"
#include <ArduinoJson.h>

ScreenLogger logger;
AsyncWebServer server(80);

void setup() {
    // Initialize camera, Wi-Fi, and servos
    initializeStartup();

    server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request) {
        handleRequest(request, nullptr, 0, 0, 0, processCaptureRequest);
    });

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
    LEDTimer::update();
}
