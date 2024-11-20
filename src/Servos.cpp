#include "Servos.h"
#include "I2CScanner.h"
#include <Wire.h>

TwoWire servoWire = TwoWire(1);

Adafruit_PWMServoDriver pwm =
    Adafruit_PWMServoDriver(SERVO_CONTROLLER_ADDR, servoWire);

DFRobot_I2C_Multiplexer multiplexer(&servoWire, I2C_MULTIPLEXER_ADDR);

void initializeServos() {
    servoWire.begin(SERVO_SDA_PIN, SERVO_SCL_PIN, 400000);

    // Initialize the I2C multiplexer
    multiplexer.begin();

    multiplexer.selectPort(0);
    delay(100);

    // Scan the I2C bus to confirm the servo controller is detected
    uint8_t *result = multiplexer.scan(0);
    bool servoDetected = false;
    for (int i = 0; i < 127; i++) {
        if (result[i] == SERVO_CONTROLLER_ADDR) {
            servoDetected = true;
            break;
        }
    }

    if (!servoDetected) {
        logger.println(
            "PCA9685 initialization FAILURE. Servo controller not found.");
        return;
    }

    if (!pwm.begin()) {
        logger.println("PCA9685 initialization FAILURE. Aborting.");
        return;
    }
    logger.println("PCA9685 initialized.");
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ);

    // Optionally, initialize servos to default positions
    // for (int i = 0; i < 16; i++) {
    //     rotateServo(i, 0);
    // }
    // logger.println("Servos positions reset.");
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
