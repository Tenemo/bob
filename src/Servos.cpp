#include "Servos.h"
#include "I2CScanner.h"
#include <Wire.h>
#include <driver/i2c.h>

TwoWire servoWire = TwoWire(1);

Adafruit_PWMServoDriver pwm =
    Adafruit_PWMServoDriver(SERVO_CONTROLLER_ADDR, servoWire);

void initializeServos() {
    i2c_driver_delete(I2C_NUM_1);

    servoWire.begin(SERVO_SDA_PIN, SERVO_SCL_PIN, 100000);

    delay(100);

    // Initialize PWM controller with retry mechanism
    int retryCount = 0;
    const int maxRetries = 3;

    while (retryCount < maxRetries) {
        if (pwm.begin()) {
            logger.println("PCA9685 initialized successfully");
            pwm.setOscillatorFrequency(27000000);
            pwm.setPWMFreq(SERVO_FREQ);
            return;
        }
        logger.println("PCA9685 initialization attempt " +
                       String(retryCount + 1) + " failed");
        delay(100);
        retryCount++;
    }

    logger.println("PCA9685 initialization failed after " + String(maxRetries) +
                   " attempts");
}

void rotateServo(int motorIndex, int degrees) {
    if (motorIndex < 0 || motorIndex > 15) {
        logger.println("Invalid motorIndex. Must be between 0 and 15.");
        return;
    }

    if (degrees < 0 || degrees > 180) {
        logger.println("Invalid degrees. Must be between 0 and 180.");
        return;
    }

    // Map degrees to PWM value
    int pwmValue = map(degrees, 0, 180, SERVOMIN, SERVOMAX);

    // Set the PWM signal for the servo
    pwm.setPWM(motorIndex, 0, pwmValue);
}

void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc) {
    // Validate JSON fields
    if (!doc["motorIndex"] || !doc["degrees"]) {
        req->send(400, "application/json",
                  "{\"error\":\"Missing motorIndex or degrees\"}");
        return;
    }

    int motorIndex = doc["motorIndex"];
    int degrees = doc["degrees"];

    // Validate motorIndex
    if (motorIndex < 0 || motorIndex > 15) {
        req->send(
            400, "application/json",
            "{\"error\":\"Invalid motorIndex. Must be between 0 and 15.\"}");
        return;
    }

    // Validate degrees
    if (degrees < 0 || degrees > 180) {
        req->send(
            400, "application/json",
            "{\"error\":\"Invalid degrees. Must be between 0 and 180.\"}");
        return;
    }

    rotateServo(motorIndex, degrees);
    logger.println("Moved servo " + String(motorIndex) + " to " +
                   String(degrees) + " degrees");
    delay(200);
    rotateServo(motorIndex, 0);

    // Prepare response
    JsonDocument responseDoc;
    responseDoc["status"] = "success";
    responseDoc["motorIndex"] = motorIndex;
    responseDoc["degrees"] = degrees;

    String response;
    serializeJson(responseDoc, response);
    req->send(200, "application/json", response);
}
