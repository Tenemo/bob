#ifndef CAMERA_H
#define CAMERA_H

#include "Globals.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <esp_camera.h>

// GPIO pin definitions for the camera
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

/**
 * @brief Initializes the camera module.
 *
 * Configures and initializes the camera with predefined settings. Ensures that
 * the power management chip is properly initialized before setting up the
 * camera.
 *
 * @return `true` if the camera is successfully initialized, `false` otherwise.
 */
bool initializeCamera();

/**
 * @brief Safely deinitializes the camera and its power management.
 *
 * This function performs the following steps:
 * 1. Deinitializes the ESP camera driver
 * 2. Disables camera power via AXP313A
 *
 * @return bool Returns true if deinitialization was successful, false if any
 * errors occurred
 */
bool deinitializeCamera();

/**
 * @brief Captures a photo using the initialized camera.
 *
 * Triggers the camera to capture a single frame and retrieves the framebuffer.
 *
 * @return Pointer to the captured framebuffer (`camera_fb_t`). Returns
 * `nullptr` if the capture fails.
 */
camera_fb_t *capturePhoto();

/**
 * @brief Processes incoming capture requests by capturing a photo and sending
 * it as a response.
 *
 * Handles HTTP requests to capture photos, retrieves the image from the camera,
 * and sends it back to the client as a JPEG image.
 *
 * @param request Pointer to the AsyncWebServerRequest object representing the
 * incoming request.
 * @param doc     Reference to the JsonDocument containing request data (unused
 * in this case).
 */
void processCaptureRequest(AsyncWebServerRequest *request,
                           const JsonDocument &doc);

#endif // CAMERA_H
