#include "env.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "src/Globals.h"
#include "src/RequestHandler.h"
#include "src/Startup.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 45
#define SIOD_GPIO_NUM 1
#define SIOC_GPIO_NUM 2

#define Y9_GPIO_NUM 48
#define Y8_GPIO_NUM 46
#define Y7_GPIO_NUM 8

#define Y6_GPIO_NUM 7
#define Y5_GPIO_NUM 4
#define Y4_GPIO_NUM 41
#define Y3_GPIO_NUM 40
#define Y2_GPIO_NUM 39
#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM 42
#define PCLK_GPIO_NUM 5

ScreenLogger logger;
AsyncWebServer server(80);

void captureHandler(AsyncWebServerRequest *request) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        logger.println("Camera capture failed");
        request->send(500, "text/plain", "Camera capture failed");
        return;
    }

    AsyncWebServerResponse *response = request->beginResponse(
        "image/jpeg", fb->len,
        [fb](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
            size_t len = fb->len - index;
            if (len > maxLen) {
                len = maxLen;
            }
            memcpy(buffer, fb->buf + index, len);
            if (len + index == fb->len) {
                esp_camera_fb_return(fb);
            }
            return len;
        });
    response->addHeader("Content-Disposition", "inline; filename=capture.jpg");
    request->send(response);
}

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
    // Initialize logger, processing pin, and connect to Wi-Fi
    initializeStartup();

    // Initialize the camera
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.fb_location = CAMERA_FB_IN_PSRAM;

    // Set the frame size and buffer count
    if (psramFound()) {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
        config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
        logger.println("PSRAM not found, aborting camera initialization.");
        return;
    }

    // Camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        logger.println("Camera init failed");
        return;
    }

    // Define the /rotate endpoint
    server.on(
        "/rotate", HTTP_POST,
        [](AsyncWebServerRequest *request) {
            // Empty onRequest handler
        },
        NULL,
        [&](AsyncWebServerRequest *request, uint8_t *data, size_t len,
            size_t index, size_t total) {
            handleRequest(request, data, len, index, total,
                          processRotateRequest);
        });
    // Add the new GET endpoint for capturing images
    server.on("/capture", HTTP_GET, captureHandler);

    server.begin();
    logger.println("Server started");
}

void loop() {
    delay(5);
    logger.update();
}
