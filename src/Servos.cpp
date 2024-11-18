#include "Servos.h"

#define SERVOMIN 150  // Minimum pulse length count (out of 4096)
#define SERVOMAX 600  // Maximum pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// Create PWM driver instance
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void initializeServos() {
    if (!pwm.begin()) {
        logger.println("Failed to initialize PCA9685!");
        while (1)
            ; // Halt execution
    }
    logger.println("PCA9685 initialized.");
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ); // Set PWM frequency

    // Will be needed later
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
