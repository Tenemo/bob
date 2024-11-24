#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#include "Camera.h"
#include "Env.h"
#include "Globals.h"
#include "RequestHandler.h"
#include "Servos.h"
#include "Startup.h"
#include "audio/AudioFile.h"
#include "audio/ProcessAudio.h"
#include "audio/WAVFileReader.h"
#include "utils/HealthCheck.h"
#include <ESPAsyncWebServer.h>
#include <FileList.h>
#include <esp_task_wdt.h>

void setup() {
    esp_log_level_set("*", ESP_LOG_ERROR);
    esp_log_level_set("wifi", ESP_LOG_WARN);
    esp_log_level_set("dhcpc", ESP_LOG_INFO);
    // Watchdog timer set to 60 seconds,
    // because it doesn't like file uploads.
    esp_task_wdt_init(60, true);

    server.on("/health-check", HTTP_GET, [](AsyncWebServerRequest *request) {
        handleRequest(request, nullptr, 0, 0, 0, processHealthCheckRequest);
    });
    server.on("/file-list", HTTP_GET, [](AsyncWebServerRequest *request) {
        handleRequest(request, nullptr, 0, 0, 0, processFileListRequest);
    });
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
    server.on(
        "/move", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
           size_t index, size_t total) {
            handleRequest(request, data, len, index, total, processMoveRequest);
        });

    server.on(
        "/audio", HTTP_POST,
        [](AsyncWebServerRequest *request) {
            handleRequest(request, nullptr, 0, 0, 0, ProcessAudioRequest);
        },
        handleAudioUpload);

    initializeStartup();
    // playAudioFile("/sample_music.wav");
    // playAudioFile("/sample_voice.wav");
    // playAudioFile("/uploaded_audio.wav");
}

void loop() {}
