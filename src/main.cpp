#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include "Camera.h"
#include "Globals.h"
#include "RequestHandler.h"
#include "Servos.h"
#include "Startup.h"
#include "audio/AudioFile.h"
#include "audio/I2SOutput.h"
#include "audio/WAVFileReader.h"
#include "env.h"
#include <SPIFFS.h>

ScreenLogger logger;
AsyncWebServer server(80);

i2s_pin_config_t i2sPins = {.bck_io_num = GPIO_NUM_12,   // GPIO_NUM_27
                            .ws_io_num = GPIO_NUM_13,    // GPIO_NUM_14
                            .data_out_num = GPIO_NUM_14, // GPIO_NUM_26
                            .data_in_num = -1};

I2SOutput *output;
AudioFile *sample;

void setup() {
    esp_log_level_set("*", ESP_LOG_ERROR);
    esp_log_level_set("wifi", ESP_LOG_WARN);
    esp_log_level_set("dhcpc", ESP_LOG_INFO);
    // Initialize camera, Wi-Fi, and servos
    initializeStartup();

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
    SPIFFS.begin();
    sample = new WAVFileReader("/sample_music.wav");
    output = new I2SOutput();
    output->start(I2S_NUM_1, i2sPins, sample);
}

void loop() {
    delay(5);
    LEDTimer::update();
}
