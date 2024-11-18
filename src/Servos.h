#ifndef SERVOS_H
#define SERVOS_H

#include "Globals.h"
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

#define SDA_PIN 13 // D11
#define SCL_PIN 14 // D10

// Function prototypes
void initializeServos();
void rotateServo(int motorIndex, int degrees);
void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc);

#endif // SERVOS_H
