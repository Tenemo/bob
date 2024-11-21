#ifndef GLOBALS_H
#define GLOBALS_H

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
 * @brief GPIO pin number for the processing LED.
 */
#define PROCESSING_LED_PIN 10 // A4

#endif // GLOBALS_H
