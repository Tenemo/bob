#include "Servos.h"
#include <Wire.h>
#include <driver/i2c.h>

TwoWire servoWire = TwoWire(1);

Adafruit_PWMServoDriver servoDriver =
    Adafruit_PWMServoDriver(SERVO_DRIVER_ADDR, servoWire);

bool initializeServos() {
    // This line fixes everything.
    // Without it, both camera and servos will not work,
    // no matter which I2C is used, included via a multiplexer.
    i2c_driver_delete(I2C_NUM_1);

    servoWire.begin(SERVO_SDA_PIN, SERVO_SCL_PIN, 100000);

    if (!servoDriver.begin()) {
        logger.println("PCA9685 initialization FAILURE.");
        return false;
    }
    servoDriver.setOscillatorFrequency(24700000);
    servoDriver.setPWMFreq(SERVO_FREQ);
    delay(10);
    Serial.println("PCA9685 initialization SUCCESSFUL. Moving servos to 90 "
                   "degrees, neutral position.");

    // Move all servos to neutral position
    for (int motorIndex = 0; motorIndex < 16; motorIndex++) {
        rotateServo(motorIndex, 90);
    }
    return true;
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

    // Map degrees to pulse length value
    int pulselen = map(degrees, 0, 180, SERVOMIN, SERVOMAX);

    // Set the PWM signal for the servo
    if (servoDriver.setPWM(motorIndex, 0, pulselen) == 0) {
        Serial.println("Moved servo " + String(motorIndex) + " to " +
                       String(degrees) + " degrees");
    } else {
        logger.println("FAILURE to move servo " + String(motorIndex));
    }
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
    // Prepare response
    JsonDocument responseDoc;
    responseDoc["status"] = "success";
    responseDoc["motorIndex"] = motorIndex;
    responseDoc["degrees"] = degrees;

    String response;
    serializeJson(responseDoc, response);
    req->send(200, "application/json", response);
}
