#include "processAudio.h"
#include "AudioFile.h"
#include "WAVFileReader.h"
#include "utils/ScreenLogger.h"
#include <SPIFFS.h>

File uploadFile;
const char *UPLOAD_PATH = "/uploaded_audio.wav";

void processAudioRequest(AsyncWebServerRequest *request,
                         const JsonDocument &doc) {
    request->send(200, "application/json", "{\"status\":\"Ready for upload\"}");
}

void handleAudioUpload(AsyncWebServerRequest *request, String filename,
                       size_t index, uint8_t *data, size_t len, bool final) {
    Serial.println("Handling audio upload");
    if (index == 0) {
        Serial.println("Starting audio upload");
        // Start of file upload
        if (len > MAX_FILE_SIZE) {
            request->send(413, "application/json",
                          "{\"error\":\"File too large\"}");
            return;
        }

        // Check if there's enough space on SPIFFS
        size_t availableSpace = SPIFFS.totalBytes() - SPIFFS.usedBytes();
        if (availableSpace < MAX_FILE_SIZE) {
            request->send(507, "application/json",
                          "{\"error\":\"Not enough storage space\"}");
            return;
        }
        Serial.println("File size: " + String(len) + " bytes");

        // Open file for writing
        uploadFile = SPIFFS.open(UPLOAD_PATH, FILE_WRITE);
        if (!uploadFile) {
            request->send(500, "application/json",
                          "{\"error\":\"Failed to open file for writing\"}");
            return;
        }

        logger.println("Started uploading: " + filename);
    }

    if (uploadFile) {
        Serial.println("Writing data to file...");
        // Write the received chunk to the file
        size_t written = uploadFile.write(data, len);
        if (written != len) {
            uploadFile.close();
            request->send(500, "application/json",
                          "{\"error\":\"Failed to write file data\"}");
            logger.println("Failed to write data to file.");
            return;
        }
    }

    if (final) {
        Serial.println("Finalizing upload...");
        // End of file upload
        if (uploadFile) {
            uploadFile.close();
            logger.println("Upload complete: " + filename);
            request->send(200, "application/json",
                          "{\"status\":\"Upload successful\"}");
        } else {
            request->send(500, "application/json",
                          "{\"error\":\"Failed to finalize file upload\"}");
        }
        // playAudioFile(UPLOAD_PATH);
    }
}
