#ifndef SERVOS_H
#define SERVOS_H

#include "Globals.h"
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

// Function prototypes
void initializeServos();
void rotateServo(int motorIndex, int degrees);
void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc);

#endif // SERVOS_H
