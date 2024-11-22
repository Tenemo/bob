/**
 * @file FileUploadHandler.h
 * @brief Handles file uploads using PSRAM buffering before writing to SPIFFS
 */

#ifndef FILE_UPLOAD_HANDLER_H
#define FILE_UPLOAD_HANDLER_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <esp_heap_caps.h>

/**
 * @class FileUploadHandler
 * @brief Manages file uploads by buffering in PSRAM before writing to SPIFFS
 *
 * This class provides a mechanism to handle large file uploads by:
 * 1. Allocating a buffer in PSRAM
 * 2. Accumulating uploaded chunks in PSRAM
 * 3. Writing the complete file to SPIFFS in a single operation
 * This approach reduces filesystem fragmentation and prevents audio glitches
 * during file operations.
 */
class FileUploadHandler {
  private:
    uint8_t *psramBuffer; /**< Pointer to the PSRAM buffer */
    size_t bufferSize;    /**< Total size of the PSRAM buffer */
    size_t currentSize;   /**< Current amount of data in buffer */
    String targetPath;    /**< Target path in SPIFFS */

  public:
    /**
     * @brief Constructs a FileUploadHandler instance
     */
    FileUploadHandler() : psramBuffer(nullptr), bufferSize(0), currentSize(0) {}

    /**
     * @brief Initializes the upload handler with expected file size
     *
     * @param path The target path in SPIFFS where the file will be saved
     * @param expectedSize Expected total size of the file in bytes
     * @return true if PSRAM allocation successful, false otherwise
     */
    bool begin(const String &path, size_t expectedSize) {
        targetPath = path;
        currentSize = 0;

        // Allocate PSRAM buffer
        bufferSize = expectedSize;
        psramBuffer =
            (uint8_t *)heap_caps_malloc(bufferSize, MALLOC_CAP_SPIRAM);

        return psramBuffer != nullptr;
    }

    /**
     * @brief Writes a chunk of data to the PSRAM buffer
     *
     * @param data Pointer to the data chunk
     * @param len Length of the data chunk in bytes
     * @return true if write successful, false if buffer full or not initialized
     */
    bool writeChunk(uint8_t *data, size_t len) {
        if (!psramBuffer || (currentSize + len > bufferSize)) {
            return false;
        }

        memcpy(psramBuffer + currentSize, data, len);
        currentSize += len;
        return true;
    }

    /**
     * @brief Finalizes the upload by writing the buffered data to SPIFFS
     *
     * @return true if file was successfully written to SPIFFS, false otherwise
     */
    bool finish() {
        if (!psramBuffer)
            return false;

        // Open SPIFFS file for writing
        File file = SPIFFS.open(targetPath, FILE_WRITE);
        if (!file) {
            cleanup();
            return false;
        }

        // Write entire buffer to SPIFFS
        size_t written = file.write(psramBuffer, currentSize);
        file.close();

        cleanup();
        return written == currentSize;
    }

    /**
     * @brief Cleans up allocated resources
     *
     * Frees the PSRAM buffer and resets internal state
     */
    void cleanup() {
        if (psramBuffer) {
            heap_caps_free(psramBuffer);
            psramBuffer = nullptr;
        }
        currentSize = 0;
        bufferSize = 0;
    }

    /**
     * @brief Destructor ensures cleanup of resources
     */
    ~FileUploadHandler() { cleanup(); }
};

#endif // FILE_UPLOAD_HANDLER_H
