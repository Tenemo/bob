#include "processAudio.h"
#include "AudioFile.h"
#include "DFRobot_AXP313A.h"
#include "WAVFileReader.h"
#include "utils/ScreenLogger.h"
#include <SPIFFS.h>

extern DFRobot_AXP313A axp;
File uploadFile;
const char *UPLOAD_PATH = "/uploaded_audio.wav";
static int lastReportedProgress = 0;

void processAudioRequest(AsyncWebServerRequest *request,
                         const JsonDocument &doc) {
    request->send(200, "application/json", "{\"status\":\"Upload complete.\"}");
    // playAudioFile(UPLOAD_PATH);
}

void handleAudioUpload(AsyncWebServerRequest *request, String filename,
                       size_t index, uint8_t *data, size_t len, bool final) {
    String clientIP = request->client()->remoteIP().toString();
    // For some reason onRequest fires AFTER upload,
    // so we handle status request LED logic here
    digitalWrite(PROCESSING_LED_PIN, HIGH);
    if (!index) {
        Serial.println("Upload Start: " + filename + " from " + clientIP);
        lastReportedProgress = 0;

        // axp.disablePower();
        // Serial.println("Camera power shut down for audio upload.");

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
        // Calculate current progress percentage using the request's total
        // content length
        int currentProgress = ((index + len) * 100) / request->contentLength();

        // Check if we've reached a new 10% milestone
        int currentTenth = currentProgress / 10;
        int lastTenth = lastReportedProgress / 10;

        if (currentTenth > lastTenth) {
            Serial.printf("Upload progress: %d%%\n", currentTenth * 10);
            lastReportedProgress = currentTenth * 10;
        }
    }

    if (final) {
        Serial.println("Upload Complete: " + filename + ", Size: " +
                       String(index + len) + " bytes from " + clientIP);
        uploadFile.close();

        // axp.enableCameraPower(axp.eOV2640);
        // Serial.println("Camera powered back on after audio upload.");

        // Send response before attempting playback
        request->send(200, "application/json",
                      "{\"status\":\"Upload successful\", \"size\":" +
                          String(index + len) + "}");
        // delay(50);
        // playAudioFile(UPLOAD_PATH);
        digitalWrite(PROCESSING_LED_PIN, LOW);
    }
}
