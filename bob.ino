#include "env.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "src/Camera.h"
#include "src/Globals.h"
#include "src/RequestHandler.h"
#include "src/Startup.h"
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

#define SERVOMIN 150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN                                                                  \
    600 // This is the rounded 'minimum' microsecond length based on the minimum
        // pulse of 150
#define USMAX                                                                  \
    2400 // This is the rounded 'maximum' microsecond length based on the
         // maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

ScreenLogger logger;
AsyncWebServer server(80);

void processRotateRequest(AsyncWebServerRequest *req, const JsonDocument &doc) {
    const char *direction = doc["direction"];
    int degrees = doc["degrees"];

    if (!direction || (strcmp(direction, "clockwise") != 0 &&
                       strcmp(direction, "counterclockwise") != 0)) {
        req->send(400, "application/json", "{\"error\":\"Invalid direction\"}");
        return;
    }

    // TODO: Implement rotation logic based on direction and degrees

    DynamicJsonDocument responseDoc(200);
    responseDoc["status"] = "success";
    responseDoc["direction"] = direction;
    responseDoc["degrees"] = degrees;
    String response;
    serializeJson(responseDoc, response);
    req->send(200, "application/json", response);
}

void setup() {
    // Initialize logger, camera, and Wi-Fi
    initializeStartup();

    if (!pwm.begin()) {
        logger.println("Failed to initialize PCA9685!");
        while (1)
            ;
    }
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates

    int pwm0;
    int SER0 = 0;
    for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {
        // Determine PWM pulse width
        pwm0 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX);
        // Write to PCA9685
        pwm.setPWM(SER0, 0, pwm0);
        delay(30);
    }
    logger.println("Servo 0 rotated to 180 degrees");

    // Move Motor 0 from 180 to 0 degrees
    for (int posDegrees = 180; posDegrees >= 0; posDegrees--) {

        // Determine PWM pulse width
        pwm0 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX);
        // Write to PCA9685
        pwm.setPWM(SER0, 0, pwm0);
        delay(30);
    }
    logger.println("Servo 0 rotated to 0 degrees");

    server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request) {
        handleRequest(request, nullptr, 0, 0, 0, processCaptureRequest);
    });

    server.on(
        "/rotate", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total) {
            handleRequest(request, data, len, index, total,
                          processRotateRequest);
        });

    server.begin();
    logger.println("Server started");
}

void loop() {
    delay(5);
    LEDTimer::update();
}
