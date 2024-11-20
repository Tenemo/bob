#ifndef SERVOS_H
#define SERVOS_H

#include "DFRobot_I2C_Multiplexer.h"
#include "Globals.h"
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

#define SERVOMIN 150  // Minimum pulse length count (out of 4096)
#define SERVOMAX 600  // Maximum pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

#define SDA_PIN 1
#define SCL_PIN 2

#define I2C_MULTIPLEXER_ADDR 0x70
#define SERVO_CONTROLLER_ADDR 0x40

void initializeServos();
void rotateServo(int motorIndex, int degrees);
void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc);

#endif // SERVOS_H
