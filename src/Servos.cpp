#include "Servos.h"
#include "utils/I2CScanner.h"
#include <Wire.h>

#define SERVOMIN 150  // Minimum pulse length count (out of 4096)
#define SERVOMAX 600  // Maximum pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// Use I2C port 1 for the servo driver
TwoWire servoWire = TwoWire(1); // Use I2C bus number 1

// Global pointer to the servo driver
Adafruit_PWMServoDriver *pwm;

void initializeServos() {
    // Initialize the I2C bus for the servo driver with frequency
    servoWire.begin(SDA_PIN, SCL_PIN, 400000); // SDA, SCL, Frequency 400kHz

    int servoAddress = scanI2CBus(servoWire);

    if (servoAddress == -1) {
        logger.println(
            "PCA9685 initialization FAILURE. No I2C devices found on "
            "the servo bus.");
        while (1)
            ; // Halt execution
    }

    Serial.print("Using servo driver at address 0x");
    if (servoAddress < 16) {
        Serial.print("0");
    }
    Serial.println(String(servoAddress, HEX));

    // Initialize the servo driver with the found address
    pwm = new Adafruit_PWMServoDriver(servoAddress, servoWire);

    if (!pwm->begin()) {
        logger.println("PCA9685 initialization FAILURE. Aborting.");
        return;
    }
    logger.println("PCA9685 initialized.");
    pwm->setOscillatorFrequency(27000000);
    pwm->setPWMFreq(SERVO_FREQ); // Set PWM frequency

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
    pwm->setPWM(motorIndex, 0, pwmValue);

    logger.println("Moved servo " + String(motorIndex) + " to " +
                   String(degrees) + " degrees");
}

void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc) {
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
    delay(200);
    rotateServo(motorIndex, 0);

    // Prepare response
    DynamicJsonDocument responseDoc(200);
    responseDoc["status"] = "success";
    responseDoc["motorIndex"] = motorIndex;
    responseDoc["degrees"] = degrees;

    String response;
    serializeJson(responseDoc, response);
    req->send(200, "application/json", response);
}
