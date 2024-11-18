// Contents of: "src/Servos.cpp"
#include "Servos.h"
#include <Wire.h> // Include Wire.h

#define SERVOMIN 150  // Minimum pulse length count (out of 4096)
#define SERVOMAX 600  // Maximum pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// Create a new TwoWire instance for the servo driver
TwoWire servoWire = TwoWire(1); // Use I2C bus number 1

// Global pointer to the servo driver
Adafruit_PWMServoDriver *pwm;

// I2C Scanner Function
int scanI2CBus(TwoWire &wire) {
    byte error, address;
    int nDevices = 0;
    int foundAddress = -1;

    Serial.println("Scanning I2C bus...");

    for (address = 1; address < 127; address++) {
        wire.beginTransmission(address);
        error = wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                logger.print("0");
            }
            logger.println(String(address, HEX));
            nDevices++;
            if (foundAddress == -1) {
                foundAddress = address; // Grab the first found address
            }
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(String(address, HEX));
        }
    }
    Serial.println("I2C scan done.\n");
    return foundAddress; // Return the found address or -1 if none
}

void initializeServos() {
    // Initialize the new I2C bus for the servo driver
    servoWire.begin(SDA_PIN, SCL_PIN);

    // Scan the new I2C bus and get the address
    int servoAddress = scanI2CBus(servoWire);

    // Check if a device was found
    if (servoAddress == -1) {
        logger.println("PCA9685 initialization FAILURE. No I2C devices found "
                       "on the servo I2C bus.");
        while (1)
            ; // Halt execution
    }

    Serial.print("Using servo driver at address 0x");
    if (servoAddress < 16) {
        logger.print("0");
    }
    Serial.println(String(servoAddress, HEX));

    // Initialize the servo driver with the found address
    pwm = new Adafruit_PWMServoDriver(servoAddress, servoWire);

    if (!pwm->begin()) {
        logger.println("PCA9685 initialization FAILURE.");
        while (1)
            ; // Halt execution
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
