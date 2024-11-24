#ifndef SERVOS_H
#define SERVOS_H

#include "Globals.h"
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

// Servo configuration constants
#define SERVOMIN 100  /**< Minimum pulse length count (out of 4096) */
#define SERVOMAX 600  /**< Maximum pulse length count (out of 4096) */
#define SERVO_FREQ 50 /**< Servo frequency in Hz */

#define SERVO_DRIVER_ADDR 0x40 /**< I2C address of the servo driver */
#define SERVO_SDA_PIN 1        /**< SDA pin for servo I2C communication */
#define SERVO_SCL_PIN 2        /**< SCL pin for servo I2C communication */

/**
 * @brief Initializes the servo motors.
 *
 * Sets up the I2C communication with the servo driver, initializes the PWM
 * settings, and verifies successful initialization.
 *
 * @return `true` if servos are successfully initialized, `false` otherwise.
 */
bool initializeServos();

/**
 * @brief Rotates a specified servo to a given angle.
 *
 * Maps the desired angle in degrees to a PWM value and updates the servo's
 * position.
 *
 * @param motorIndex Index of the servo motor to rotate (0-15).
 * @param degrees    Target angle in degrees (0-180).
 */
void rotateServo(int motorIndex, int degrees);
void resetServos();

/**
 * @brief Processes servo rotation requests by validating input and performing
 * the rotation.
 *
 * Handles HTTP POST requests to rotate a servo. Validates the provided motor
 * index and degrees, performs the rotation, and sends a JSON response
 * indicating success or failure.
 *
 * @param req  Pointer to the AsyncWebServerRequest object representing the
 * incoming request.
 * @param doc  Reference to the JsonDocument containing request data.
 */
void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc);

/**
 * @brief Processes servo rotation requests by validating input and performing
 * the rotation.
 *
 * Handles HTTP POST requests to rotate a servo. Validates the provided motor
 * index and degrees, performs the rotation, and sends a JSON response
 * indicating success or failure.
 *
 * @param req  Pointer to the AsyncWebServerRequest object representing the
 * incoming request.
 * @param doc  Reference to the JsonDocument containing request data.
 */
void processMoveRequest(AsyncWebServerRequest *req, const JsonDocument &doc);

#endif // SERVOS_H
