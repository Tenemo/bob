#include "processAudio.h"
#include "AudioFile.h"
#include "Camera.h"
#include "DFRobot_AXP313A.h"
#include "WAVFileReader.h"
#include "utils/ScreenLogger.h"
#include <SPIFFS.h>

volatile bool uploadError = false;
extern DFRobot_AXP313A cameraPowerDriver;
File uploadFile;
const char *UPLOAD_PATH = "/uploaded_audio.wav";
static int lastReportedProgress = 0;
static String message = "";

void processAudioRequest(AsyncWebServerRequest *request,
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
    // For some reason onRequest fires AFTER upload,
    // so we handle status request LED logic here
    digitalWrite(PROCESSING_LED_PIN, HIGH);
    if (!index) {
        uploadError = false; // Reset error flag at start
        logger.println("Audio upload from " + clientIP + " began.");
        lastReportedProgress = 0;

        deinitializeCamera();

        // Close any existing file
        if (uploadFile) {
            uploadFile.close();
        }

        // Handle "fileName" parameter with default and leading slash
        String path = filename;
        if (path.length() == 0)
            path = "/uploaded_audio.wav";
        if (path[0] != '/')
            path = "/" + path;

        // Generate timestamp
        time_t now = time(nullptr);
        struct tm *timeinfo = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "_%m%d%H%M%S", timeinfo);

        // Append timestamp before the file extension
        int extIndex = path.lastIndexOf('.');
        if (extIndex != -1) {
            path = path.substring(0, extIndex) + String(timestamp) +
                   path.substring(extIndex);
        } else {
            path += String(timestamp);
        }

        // Remove existing file
        if (SPIFFS.exists(path.c_str())) {
            SPIFFS.remove(path.c_str());
        }
        Serial.println("Uploading file: " + path);

        // Open the file for writing
        uploadFile = SPIFFS.open(path.c_str(), FILE_WRITE);
        if (!uploadFile) {
            uploadError = true;
            Serial.println("Failed to open file for writing");
            request->send(500, "application/json",
                          "{\"error\":\"Failed to open file for writing\"}");
            digitalWrite(PROCESSING_LED_PIN, LOW);
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
            uploadError = true;
            Serial.println("Failed to write data to file");
            request->send(500, "application/json",
                          "{\"error\":\"Failed to write file data\"}");
            uploadFile.close();
            digitalWrite(PROCESSING_LED_PIN, LOW);
            return;
        }
    }

    if (final) {
        Serial.println("Upload Complete: " + filename + ", Size: " +
                       String(index + len) + " bytes from " + clientIP);
        uploadFile.close();

        initializeCamera();

        // Send response before attempting playback
        request->send(200, "application/json",
                      "{\"status\":\"Upload successful\", \"size\":" +
                          String(index + len) + "}");
        // delay(50);
        // playAudioFile(UPLOAD_PATH);
        digitalWrite(PROCESSING_LED_PIN, LOW);
    }
}
