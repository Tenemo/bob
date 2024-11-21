#include "processAudio.h"
#include "AudioFile.h"
#include "WAVFileReader.h"
#include "utils/ScreenLogger.h"
#include <SPIFFS.h>

File uploadFile;
const char *UPLOAD_PATH = "/uploaded_audio.wav";

void processAudioRequest(AsyncWebServerRequest *request,
                         const JsonDocument &doc) {
    // This function will be called for the initial POST request
    // The actual file handling is done in handleAudioUpload
    request->send(200);
}

void handleAudioUpload(AsyncWebServerRequest *request, String filename,
                       size_t index, uint8_t *data, size_t len, bool final) {
    String clientIP = request->client()->remoteIP().toString();

    if (!index) {
        logger.println("Starting audio upload from " + clientIP);

        // Open file for writing
        uploadFile = SPIFFS.open(UPLOAD_PATH, FILE_WRITE);
        if (!uploadFile) {
            logger.println("Failed to open file for writing");
            request->send(500, "text/plain", "Failed to open file for writing");
            return;
        }
    }

    if (len) {
        if (uploadFile.write(data, len) != len) {
            logger.println("Failed to write audio data");
            request->send(500, "text/plain", "Failed to write audio data");
            uploadFile.close();
            return;
        }
    }

    if (final) {
        uploadFile.close();
        logger.println("Upload complete: " + String(index + len) + " bytes");

        // Start playing the uploaded audio
        playAudioFile(UPLOAD_PATH);

        // Send success response
        JsonDocument responseDoc;
        responseDoc["status"] = "success";
        responseDoc["message"] = "Audio uploaded and playing";
        responseDoc["size"] = index + len;

        String response;
        serializeJson(responseDoc, response);
        request->send(200, "application/json", response);
    }
}
