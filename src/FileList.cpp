#include "FileList.h"

String formatFileSize(size_t bytes) {
    if (bytes < 1024) {
        return String(bytes) + " B";
    } else if (bytes < (1024 * 1024)) {
        float kb = bytes / 1024.0;
        return String(kb, 1) + " KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        float mb = bytes / (1024.0 * 1024.0);
        return String(mb, 1) + " MB";
    } else {
        float gb = bytes / (1024.0 * 1024.0 * 1024.0);
        return String(gb, 1) + " GB";
    }
}

void processFileListRequest(AsyncWebServerRequest *request,
                            const JsonDocument &doc) {
    JsonDocument responseDoc;

    // Use the newer syntax for creating arrays
    responseDoc["files"].to<JsonArray>();

    File root = SPIFFS.open("/");
    if (!root) {
        request->send(500, "application/json",
                      "{\"error\":\"Failed to open SPIFFS root\"}");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        // Use the newer syntax for adding objects to arrays
        JsonObject fileObj = responseDoc["files"].add<JsonObject>();
        fileObj["name"] = String(file.name());
        fileObj["size"] = file.size();
        fileObj["humanReadableSize"] = formatFileSize(file.size());
        fileObj["isDirectory"] = file.isDirectory();
        file = root.openNextFile();
    }

    String response;
    serializeJson(responseDoc, response);
    request->send(200, "application/json", response);
}
