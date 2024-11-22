#include "processAudio.h"
#include "AudioFile.h"
#include "Camera.h"
#include "DFRobot_AXP313A.h"
#include "WAVFileReader.h"
#include "utils/ScreenLogger.h"
#include <SPIFFS.h>

extern DFRobot_AXP313A cameraPowerDriver;
File uploadFile;
const char *UPLOAD_PATH = "/uploaded_audio.wav";
static int lastReportedProgress = 0;
static String message = "";

void processAudioRequest(AsyncWebServerRequest *request,
                         const JsonDocument &doc) {
    request->send(200, "application/json", "{\"status\":\"Upload complete.\"}");
}

void handleAudioUpload(AsyncWebServerRequest *request, String filename,
                       size_t index, uint8_t *data, size_t len, bool final) {
    String clientIP = request->client()->remoteIP().toString();
    // For some reason onRequest fires AFTER upload,
    // so we handle status request LED logic here
    digitalWrite(PROCESSING_LED_PIN, HIGH);
    if (!index) {
        logger.println("Audio upload from " + clientIP + " began.");
        lastReportedProgress = 0;
        // Deinitializing camera
        // deinitializeCamera();
        // Serial.println("Camera deinitialized.");

        // Close any existing file
        if (uploadFile) {
            uploadFile.close();
        }

        // Remove existing file
        if (SPIFFS.exists(UPLOAD_PATH)) {
            SPIFFS.remove(UPLOAD_PATH);
        }

        uploadFile = SPIFFS.open(UPLOAD_PATH, FILE_WRITE);
        if (!uploadFile) {
            Serial.println("Failed to open file for writing");
            request->send(500, "application/json",
                          "{\"error\":\"Failed to open file for writing\"}");
            return;
        }
    }
    if (len && request->contentLength() > 0) {
        // Calculate current progress percentage
        int currentProgress = ((index + len) * 100) / request->contentLength();

        // Check if we've reached a new 20% milestone
        int currentFifth = currentProgress / 20;
        int lastFifth = lastReportedProgress / 20;

        if (currentFifth > lastFifth) {
            int progressToReport = currentFifth * 20;
            message = "Upload progress: " + String(currentProgress) + "%";
            logger.println(message);
            ;
            lastReportedProgress = progressToReport;
        }

        if (uploadFile.write(data, len) != len) {
            Serial.println("Failed to write data to file");
            request->send(500, "application/json",
                          "{\"error\":\"Failed to write file data\"}");
            uploadFile.close();
            return;
        }
    }

    if (final) {
        Serial.println("Upload Complete: " + filename + ", Size: " +
                       String(index + len) + " bytes from " + clientIP);
        uploadFile.close();

        // initializeCamera();

        // Send response before attempting playback
        request->send(200, "application/json",
                      "{\"status\":\"Upload successful\", \"size\":" +
                          String(index + len) + "}");
        // delay(50);
        // playAudioFile(UPLOAD_PATH);
        digitalWrite(PROCESSING_LED_PIN, LOW);
    }
}
