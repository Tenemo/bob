#ifndef FILE_UPLOAD_HANDLER_H
#define FILE_UPLOAD_HANDLER_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <vector>

/**
 * @class FileUploadHandler
 * @brief Handles file uploads by buffering data in PSRAM and writing to SPIFFS
 * upon completion.
 *
 * This class buffers incoming file data in memory (PSRAM) during the upload
 * process. Once the upload is complete, it writes the buffered data to the
 * specified SPIFFS path, overwriting any existing file with the same name.
 */
class FileUploadHandler {
  private:
    std::vector<uint8_t> buffer; /**< Buffer to store uploaded data in PSRAM */
    String filePath;             /**< Destination SPIFFS path */

  public:
    /**
     * @brief Begins the file upload by initializing the buffer and setting the
     * file path.
     *
     * If a file with the same path already exists in SPIFFS, it will be removed
     * to allow overwriting.
     *
     * @param path The SPIFFS path where the file will be stored.
     * @param contentLength The total size of the file being uploaded.
     * @return `true` if initialization is successful, `false` otherwise.
     */
    bool begin(const String &path, size_t contentLength) {
        filePath = path;
        buffer.reserve(contentLength);
        return true;
    }

    /**
     * @brief Writes a chunk of data to the buffer.
     *
     * @param data Pointer to the data buffer.
     * @param len Length of the data buffer.
     * @return `true` if data is successfully buffered, `false` otherwise.
     */
    bool writeChunk(uint8_t *data, size_t len) {
        if (data && len > 0) {
            try {
                buffer.insert(buffer.end(), data, data + len);
                return true;
            } catch (std::bad_alloc &) {
                return false; // Buffer overflow or memory allocation failure
            }
        }
        return false;
    }

    /**
     * @brief Finalizes the upload by writing the buffered data to SPIFFS.
     *
     * This function writes all buffered data to the specified file path in
     * SPIFFS, effectively overwriting any existing file.
     *
     * @return `true` if the data is successfully written to SPIFFS, `false`
     * otherwise.
     */
    bool finish() {
        if (SPIFFS.exists(filePath)) {
            SPIFFS.remove(filePath); // Remove existing file to allow overwrite
        }
        if (buffer.empty())
            return false;
        File file = SPIFFS.open(filePath, FILE_WRITE);
        if (!file)
            return false;
        size_t written = file.write(buffer.data(), buffer.size());
        file.close();
        return written == buffer.size();
    }

    /**
     * @brief Cleans up the upload by clearing the buffer and resetting the file
     * path.
     */
    void cleanup() {
        buffer.clear();
        filePath = "";
    }
};

#endif // FILE_UPLOAD_HANDLER_H
