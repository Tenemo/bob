#ifndef GLOBALS_H
#define GLOBALS_H

#define ARDUINO 10813 // Fixes WProgram.h error
#include <Arduino.h>

#include "src/ScreenLogger.h"

extern ScreenLogger logger;

#define PROCESSING_LED_PIN 9 // D7

#endif // GLOBALS_H
