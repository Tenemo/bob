#ifndef FILE_UPLOAD_HANDLER_H
#define FILE_UPLOAD_HANDLER_H

#include "Globals.h"
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <vector>

/**
 * @class FileUploadHandler
 * @brief Handles file uploads by buffering data in PSRAM and writing to SPIFFS
 * upon completion.
 */
class FileUploadHandler {
  private:
    std::vector<uint8_t> buffer; /**< Buffer to store uploaded data in PSRAM */
    String filePath;             /**< Destination SPIFFS path */

  public:
    bool begin(const String &path, size_t contentLength) {
        if (contentLength == 0) {
            logger.println("Error: Content length is zero");
            return false;
        }

        if (contentLength > MAX_FILE_SIZE) {
            logger.println(
                "Error: File too large (" + String(contentLength / 1024) +
                "KB). Maximum size is " + String(MAX_FILE_SIZE / 1024) + "KB");
            return false;
        }

        if (!path.startsWith("/")) {
            logger.println("Error: Invalid file path (must start with /)");
            return false;
        }

        // Check available space in SPIFFS
        size_t totalBytes = SPIFFS.totalBytes();
        size_t usedBytes = SPIFFS.usedBytes();
        size_t freeSpace = totalBytes - usedBytes;

        if (contentLength > freeSpace) {
            logger.println("Error: Insufficient space in SPIFFS. Need " +
                           String(contentLength / 1024) + "KB, but only " +
                           String(freeSpace / 1024) + "KB available");
            return false;
        }

        try {
            filePath = path;
            buffer.reserve(contentLength);
            return true;
        } catch (const std::bad_alloc &e) {
            logger.println("Error: Failed to allocate buffer memory for " +
                           String(contentLength / 1024) + "KB");
            return false;
        }
    }

    bool writeChunk(uint8_t *data, size_t len) {
        if (!data || len == 0) {
            logger.println("Error: Invalid chunk data");
            return false;
        }

        try {
            buffer.insert(buffer.end(), data, data + len);
            return true;
        } catch (const std::bad_alloc &e) {
            logger.println(
                "Error: Memory allocation failed while writing chunk");
            return false;
        } catch (const std::exception &e) {
            logger.println("Error: Exception while writing chunk: " +
                           String(e.what()));
            return false;
        }
    }

    bool finish() {
        if (buffer.empty()) {
            logger.println("Error: No data to write");
            return false;
        }

        // Remove existing file if it exists
        if (SPIFFS.exists(filePath)) {
            if (!SPIFFS.remove(filePath)) {
                logger.println("Error: Failed to remove existing file: " +
                               filePath);
                return false;
            }
        }

        File file = SPIFFS.open(filePath, FILE_WRITE);
        if (!file) {
            logger.println("Error: Failed to open file for writing: " +
                           filePath);
            return false;
        }

        size_t written = file.write(buffer.data(), buffer.size());
        file.close();

        if (written != buffer.size()) {
            logger.println("Error: Failed to write complete file. Wrote " +
                           String(written) + " of " + String(buffer.size()) +
                           " bytes");
            // Try to clean up the partial file
            SPIFFS.remove(filePath);
            return false;
        }

        logger.println("Successfully wrote " + String(written / 1024) +
                       "KB to " + filePath);
        return true;
    }

    void cleanup() {
        try {
            buffer.clear();
            buffer.shrink_to_fit(); // Release memory back to the system
            filePath = "";
        } catch (const std::exception &e) {
            logger.println("Warning: Exception during cleanup: " +
                           String(e.what()));
        }
    }
};

#endif // FILE_UPLOAD_HANDLER_H
