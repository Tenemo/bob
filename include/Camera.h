#ifndef CAMERA_H
#define CAMERA_H

#include "Globals.h"
// #include "memorysaver.h"
// #include <ArduCAM.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

#define CS_CAMERA 14 // D10
#define SPI_SCK 17
#define SPI_MISO 16
#define SPI_MOSI 15
#define SDA_PIN 1
#define SCL_PIN 2

// ArduCAM myCAM;

void initializeCamera();

void processCaptureRequest(AsyncWebServerRequest *request,
                           const JsonDocument &doc);

#endif // CAMERA_H
