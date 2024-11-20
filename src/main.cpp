#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include "Camera.h"
#include "Globals.h"
#include "RequestHandler.h"
#include "Servos.h"
#include "Startup.h"
#include "env.h"
#include <ArduinoJson.h>

ScreenLogger logger;
AsyncWebServer server(80);

void setup() {
    esp_log_level_set("*", ESP_LOG_ERROR);
    esp_log_level_set("wifi", ESP_LOG_WARN);
    esp_log_level_set("dhcpc", ESP_LOG_INFO);
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
