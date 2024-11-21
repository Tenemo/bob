#ifndef FILELIST_H
#define FILELIST_H

#include "Globals.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

void processFileListRequest(AsyncWebServerRequest *request,
                            const JsonDocument &doc);

#endif // FILELIST_H
