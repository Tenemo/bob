#ifndef GLOBALS_H
#define GLOBALS_H

#include "DFRobot_AXP313A.h"
#include "utils/ScreenLogger.h"
#include <ESPAsyncWebServer.h>

/**
 * @brief GPIO pin number for the processing LED.
 */
#define PROCESSING_LED_PIN 10 // A4

/**
 * @brief Global instance of the screen logger.
 */
extern ScreenLogger logger;

/**
 * @brief Global instance of the asynchronous web server.
 */
extern AsyncWebServer server;

/**
 * @brief Global instance of the camera wire instance.
 */
extern TwoWire cameraWire;

/**
 * @brief Global instance of the power management chip.
 */
extern DFRobot_AXP313A cameraPowerDriver;

/**
 * @brief Maximum allowed file size (8MB)
 * It's 8MB because the ESP32-S3 has 8MB of PSRAM
 */
extern const size_t MAX_FILE_SIZE;

#endif // GLOBALS_H
