#ifndef PROCESS_AUDIO_H
#define PROCESS_AUDIO_H

#include "Globals.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

/**
 * @brief Processes an audio upload request and initiates playback.
 *
 * Handles the upload of WAV audio files to SPIFFS storage and starts playback
 * once the upload is complete.
 *
 * @param request Pointer to the AsyncWebServerRequest object.
 * @param doc Reference to the JsonDocument containing request data.
 */
void processAudioRequest(AsyncWebServerRequest *request,
                         const JsonDocument &doc);

/**
 * @brief Handles the file upload process for audio files.
 *
 * Manages the upload of audio data in chunks, writes to SPIFFS, and initiates
 * playback when complete.
 *
 * @param request Pointer to the AsyncWebServerRequest object.
 * @param filename Name of the uploaded file.
 * @param index Current position in the upload stream.
 * @param data Pointer to the current chunk of data.
 * @param len Length of the current data chunk.
 * @param final Whether this is the final chunk of the upload.
 */
void handleAudioUpload(AsyncWebServerRequest *request, String filename,
                       size_t index, uint8_t *data, size_t len, bool final);

#endif // PROCESS_AUDIO_H
