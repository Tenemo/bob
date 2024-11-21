#ifndef GLOBALS_H
#define GLOBALS_H

#include "utils/ScreenLogger.h"
#include <ESPAsyncWebServer.h>

extern ScreenLogger logger;
extern AsyncWebServer server;

#define PROCESSING_LED_PIN 10 // A4

#endif // GLOBALS_H
