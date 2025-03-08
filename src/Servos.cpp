#include "Servos.h"
#include <Wire.h>
#include <driver/i2c.h>

TwoWire servoWire = TwoWire(1);

Adafruit_PWMServoDriver servoDriver =
    Adafruit_PWMServoDriver(SERVO_DRIVER_ADDR, servoWire);

// Bottom rotors
#define FRONT_RIGHT_BOTTOM 0
#define MIDDLE_RIGHT_BOTTOM 1
#define BACK_RIGHT_BOTTOM 2
#define BACK_LEFT_BOTTOM 3
#define MIDDLE_LEFT_BOTTOM 4
#define FRONT_LEFT_BOTTOM 5
// Top rotors
#define FRONT_RIGHT_TOP 6
#define MIDDLE_RIGHT_TOP 7
#define BACK_RIGHT_TOP 8
#define BACK_LEFT_TOP 9
#define MIDDLE_LEFT_TOP 10
#define FRONT_LEFT_TOP 11

#define MAX_RANGE_FOR_TOP_SERVOS 20
#define BASE_ANGLE_FOR_BOTTOM_SERVOS 110
#define BASE_ANGLE_FOR_TOP_SERVOS 90

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

    resetServos();
    wiggle();
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

    if (motorIndex >= FRONT_RIGHT_TOP && motorIndex <= FRONT_LEFT_TOP &&
        (degrees > 90 + MAX_RANGE_FOR_TOP_SERVOS ||
         degrees < 90 - MAX_RANGE_FOR_TOP_SERVOS)) {
        logger.println("Top servos can only rotate up to " +
                       String(MAX_RANGE_FOR_TOP_SERVOS) + " degrees.");
        return;
    }

    // Left side servos are mirrored
    if ((motorIndex >= 3 && motorIndex <= 5) ||
        (motorIndex >= 9 && motorIndex <= 11)) {
        degrees = 180 - degrees;
    }

    // Map degrees to pulse length value
    int pulselen = map(degrees, 0, 180, SERVOMIN, SERVOMAX);
    delay(100);

    // Set the PWM signal for the servo
    if (servoDriver.setPWM(motorIndex, 0, pulselen) == 0) {
        Serial.println("Moved servo " + String(motorIndex) + " to " +
                       String(degrees) + " degrees");
    } else {
        logger.println("FAILURE to move servo " + String(motorIndex));
    }
}

void resetServos() {
    for (int motorIndex = 0; motorIndex < 16; motorIndex++) {
        if (motorIndex >= FRONT_RIGHT_TOP && motorIndex <= FRONT_LEFT_TOP) {
            rotateServo(motorIndex, BASE_ANGLE_FOR_TOP_SERVOS);
        } else {
            rotateServo(motorIndex, BASE_ANGLE_FOR_BOTTOM_SERVOS);
        }
        delay(150);
    }
}

void standUp() {
    for (int motorIndex = 0; motorIndex < 6; motorIndex++) {
        rotateServo(motorIndex, 30);
        delay(250);
    }
}

void sitDown() {
    for (int motorIndex = 0; motorIndex < 6; motorIndex++) {
        rotateServo(motorIndex, BASE_ANGLE_FOR_BOTTOM_SERVOS);
        delay(250);
    }
}

void wiggle() {
    int originalPositions[6];

    for (int i = 0; i < 6; i++) {
        int motorIndex = i + FRONT_RIGHT_TOP;
        if (motorIndex >= 9 && motorIndex <= 11) {
            originalPositions[i] = 180 - BASE_ANGLE_FOR_TOP_SERVOS;
        } else {
            originalPositions[i] = BASE_ANGLE_FOR_TOP_SERVOS;
        }
    }
    for (int i = 0; i < 6; i++) {
        int motorIndex = i + FRONT_RIGHT_TOP;
        int baseAngle = motorIndex >= 9 && motorIndex <= 11
                            ? 180 - BASE_ANGLE_FOR_TOP_SERVOS
                            : BASE_ANGLE_FOR_TOP_SERVOS;
        rotateServo(motorIndex, baseAngle - 10);
        delay(25);
        rotateServo(motorIndex, baseAngle + 10);
        delay(25);
        rotateServo(motorIndex, baseAngle);
        delay(25);
    }
}

void processMoveRequest(AsyncWebServerRequest *req, const JsonDocument &doc) {
    if (!doc["type"]) {
        req->send(400, "application/json",
                  "{\"error\":\"Missing motorIndex or degrees\"}");
        return;
    }

    String type = doc["type"];

    if (type == "reset") {
        resetServos();
    } else if (type == "standUp") {
        standUp();
    } else if (type == "sitDown") {
        sitDown();
    } else if (type == "wiggle") {
        wiggle();
    } else {
        req->send(400, "application/json", "{\"error\":\"Invalid type.\"}");
        return;
    }

    // Prepare response
    JsonDocument responseDoc;
    responseDoc["status"] = "success";
    responseDoc["type"] = type;

    String response;
    serializeJson(responseDoc, response);
    req->send(200, "application/json", response);
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
        req->send(400, "application/json",
                  "{\"error\":\"Invalid motorIndex. Must be between 0 "
                  "and 15.\"}");
        return;
    }

    // Validate degrees
    if (degrees < 0 || degrees > 180) {
        req->send(400, "application/json",
                  "{\"error\":\"Invalid degrees. Must be between 0 and "
                  "180.\"}");
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
