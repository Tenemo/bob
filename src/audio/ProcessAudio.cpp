#include "ProcessAudio.h"
#include "AudioFile.h"
#include "Camera.h"
#include "DFRobot_AXP313A.h"
#include "WAVFileReader.h"
#include "utils/ScreenLogger.h"
#include <SPIFFS.h>

volatile bool uploadError = false;
extern DFRobot_AXP313A cameraPowerDriver;

const char *UPLOAD_PATH = "/uploaded_audio.wav";
static int lastReportedProgress = 0;
static String message = "";
static String path = "";

// Global buffer management
static uint8_t *uploadBuffer = nullptr;
static size_t uploadSize = 0;
static size_t currentPosition = 0;
static bool isUploading = false;

void cleanupUpload() {
    if (uploadBuffer != nullptr) {
        free(uploadBuffer);
        uploadBuffer = nullptr;
    }
    uploadSize = 0;
    currentPosition = 0;
    isUploading = false;
}

void ProcessAudioRequest(AsyncWebServerRequest *request,
                         const JsonDocument &doc) {
    if (uploadError) {
        request->send(500, "application/json",
                      "{\"error\":\"File write error.\"}");
    } else {
        request->send(200, "application/json",
                      "{\"status\":\"Upload complete.\"}");
    }
}

void handleAudioUpload(AsyncWebServerRequest *request, String filename,
                       size_t index, uint8_t *data, size_t len, bool final) {
    String clientIP = request->client()->remoteIP().toString();

    if (!index) {
        digitalWrite(PROCESSING_LED_PIN, HIGH);
        uploadError = false;
        logger.println("Audio upload from " + clientIP + " starting...");
        lastReportedProgress = 0;

        // Make sure previous upload is cleaned up
        cleanupUpload();

        path = filename;
        if (path.length() == 0)
            path = "/uploaded_audio.wav";
        if (path[0] != '/')
            path = "/" + path;

        // Allocate new buffer
        uploadSize = request->contentLength();
        uploadBuffer = (uint8_t *)ps_malloc(uploadSize);
        if (!uploadBuffer) {
            uploadError = true;
            logger.println("Failed to allocate PSRAM buffer");
            request->send(500, "application/json",
                          "{\"error\":\"Failed to allocate buffer\"}");
            digitalWrite(PROCESSING_LED_PIN, LOW);
            return;
        }
        currentPosition = 0;
        isUploading = true;
    }

    if (len && uploadBuffer && isUploading) {
        if (currentPosition + len <= uploadSize) {
            memcpy(uploadBuffer + currentPosition, data, len);
            currentPosition += len;

            if (request->contentLength() > 0) {
                int currentProgress =
                    (currentPosition * 100) / request->contentLength();
                int currentFifth = currentProgress / 20;
                int lastFifth = lastReportedProgress / 20;

                if (currentFifth > lastFifth) {
                    message =
                        "Upload progress: " + String(currentProgress) + "%";
                    logger.println(message);
                    lastReportedProgress = currentFifth * 20;
                }
            }
        } else {
            uploadError = true;
            logger.println("Buffer overflow prevented");
            cleanupUpload();
            request->send(500, "application/json",
                          "{\"error\":\"Buffer overflow\"}");
            digitalWrite(PROCESSING_LED_PIN, LOW);
            return;
        }
    }

    if (final) {
        if (!uploadBuffer || !isUploading) {
            uploadError = true;
            logger.println("Upload state error");
            cleanupUpload();
            request->send(500, "application/json",
                          "{\"error\":\"Upload state error\"}");
            digitalWrite(PROCESSING_LED_PIN, LOW);
            return;
        }

        logger.println("Upload complete, starting playback...");

        // Create a copy of the buffer for playback
        uint8_t *playbackBuffer = (uint8_t *)ps_malloc(currentPosition);
        if (!playbackBuffer) {
            uploadError = true;
            logger.println("Failed to allocate playback buffer");
            cleanupUpload();
            request->send(500, "application/json",
                          "{\"error\":\"Playback buffer allocation failed\"}");
            digitalWrite(PROCESSING_LED_PIN, LOW);
            return;
        }

        // Copy the data and clean up upload buffer
        memcpy(playbackBuffer, uploadBuffer, currentPosition);
        size_t finalSize = currentPosition;

        // Clean up upload state
        cleanupUpload();

        // Start playback with the copied buffer
        playAudioFromPSRAM(playbackBuffer, finalSize);

        Serial.println("Upload complete: " + filename + ", size: " +
                       String(finalSize) + " bytes from " + clientIP);

        request->send(200, "application/json",
                      "{\"status\":\"Upload successful\", \"size\":" +
                          String(finalSize) + "}");
        digitalWrite(PROCESSING_LED_PIN, LOW);
    }
}
