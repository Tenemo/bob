#include "Camera.h"
#include "DFRobot_AXP313A.h"
#include <Arduino.h>

DFRobot_AXP313A axp(0x36);

bool initializeCamera() {
    // Initialize the AXP313A power management chip
    const int maxRetries = 10;
    int retries = 0;
    while (axp.begin() != 0) {
        Serial.println("AXP313A init error");
        delay(1000);
        retries++;
        if (retries >= maxRetries) {
            logger.println("AXP313A initialization FAILURE after " +
                           String(maxRetries) + " attempts.");
            return false;
        }
    }
    // Enable the power for camera
    axp.enableCameraPower(axp.eOV2640);

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
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20 * 1000 * 1000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.fb_location = CAMERA_FB_IN_PSRAM;

    if (psramFound()) {
        Serial.println("PSRAM found.");
        Serial.printf("Total PSRAM Size: %u bytes\n", ESP.getPsramSize());
        Serial.printf("Free PSRAM: %u bytes\n", ESP.getFreePsram());
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10; // 0-63 lower number means higher quality
        config.fb_count = 2;
        config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
        logger.println("Camera initialization FAILURE. PSRAM not found, "
                       "aborting camera initialization.");
        return false;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        logger.println("Camera initialization FAILURE with error:");
        logger.println(esp_err_to_name(err));
        Serial.print("Camera initialization failed with error: 0x");
        Serial.println(err, HEX);
        return false;
    }
    Serial.println("Camera initialization SUCCESSFUL.");
    return true;
}

camera_fb_t *capturePhoto() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        logger.println("Camera capture failed.");
    }
    return fb;
}

void processCaptureRequest(AsyncWebServerRequest *request,
                           const JsonDocument &doc) {
    camera_fb_t *fb = capturePhoto();
    if (!fb) {
        request->send(500, "text/plain", "Camera capture failed.");
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
