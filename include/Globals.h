#ifndef GLOBALS_H
#define GLOBALS_H

#include "DFRobot_AXP313A.h"
#include "utils/ScreenLogger.h"
#include <ESPAsyncWebServer.h>

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
 * @brief GPIO pin number for the processing LED.
 */
#define PROCESSING_LED_PIN 10 // A4

#endif // GLOBALS_H
