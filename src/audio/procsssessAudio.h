#ifndef PROCESS_AUDIO_H
#define PROCESS_AUDIO_H

#include "FileUploadHandler.h"
#include "Globals.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

extern FileUploadHandler uploadHandler;

// Path where uploaded audio will be stored
extern const char *UPLOAD_PATH;

/**
 * @brief Processes an audio upload request.
 *
 * This function handles the initial POST request for audio uploads.
 * The actual file processing is done in handleAudioUpload.
 *
 * @param request Pointer to the AsyncWebServerRequest object.
 * @param doc Reference to the JsonDocument containing request data.
 */
void processAudioRequest(AsyncWebServerRequest *request,
                         const JsonDocument &doc);

/**
 * @brief Handles the file upload process for audio files.
 *
 * Processes incoming WAV file uploads in chunks, validating:
 * - File type (.wav only)
 * - File size (max 10MB)
 * - Available SPIFFS space
 *
 * @param request Pointer to the AsyncWebServerRequest object
 * @param filename Name of the uploaded file
 * @param index Current position in the upload stream
 * @param data Pointer to the current chunk of data
 * @param len Length of the current data chunk
 * @param final Whether this is the final chunk of the upload
 */
void handleAudioUpload(AsyncWebServerRequest *request, String filename,
                       size_t index, uint8_t *data, size_t len, bool final);

/**
 * @brief Stops audio playback.
 */
void stopPlayback();

#endif // PROCESS_AUDIO_H
