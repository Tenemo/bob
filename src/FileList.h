#ifndef FILELIST_H
#define FILELIST_H

#include "Globals.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

/**
 * @brief Formats a file size in bytes to a human-readable string.
 *
 * Converts a file size from bytes to a string representation in B, KB, MB, or
 * GB.
 *
 * @param bytes The size of the file in bytes.
 * @return A `String` representing the formatted file size.
 */
String formatFileSize(size_t bytes);

/**
 * @brief Processes file list requests by retrieving and sending the list of
 * files in SPIFFS.
 *
 * Handles HTTP requests to list files stored in the SPIFFS file system. Gathers
 * file information such as name, size, and directory status, and sends it back
 * as a JSON response.
 *
 * @param request Pointer to the AsyncWebServerRequest object representing the
 * incoming request.
 * @param doc     Reference to the JsonDocument containing request data (unused
 * in this case).
 */
void processFileListRequest(AsyncWebServerRequest *request,
                            const JsonDocument &doc);

#endif // FILELIST_H
