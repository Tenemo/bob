#ifndef FILE_UPLOAD_HANDLER_H
#define FILE_UPLOAD_HANDLER_H

#include <Arduino.h>

class FileUploadHandler {
  private:
    uint8_t *buffer;
    size_t totalSize;
    size_t currentPosition;
    bool initialized;

  public:
    FileUploadHandler()
        : buffer(nullptr), totalSize(0), currentPosition(0),
          initialized(false) {}

    ~FileUploadHandler() { cleanup(); }

    bool begin(String path, size_t size) {
        cleanup(); // Clean up any existing buffer

        if (size == 0)
            return false;

        // Allocate in PSRAM
        buffer = (uint8_t *)ps_malloc(size);
        if (!buffer)
            return false;

        totalSize = size;
        currentPosition = 0;
        initialized = true;
        return true;
    }

    bool writeChunk(uint8_t *data, size_t len) {
        if (!initialized || !buffer || currentPosition + len > totalSize) {
            return false;
        }

        memcpy(buffer + currentPosition, data, len);
        currentPosition += len;
        return true;
    }

    void cleanup() {
        if (buffer) {
            free(buffer);
            buffer = nullptr;
        }
        totalSize = 0;
        currentPosition = 0;
        initialized = false;
    }

    uint8_t *getBuffer() { return buffer; }
    size_t getSize() { return currentPosition; }
    bool isInitialized() { return initialized; }
};

#endif
