#include "Globals.h"

ScreenLogger logger;
AsyncWebServer server(80);
TwoWire cameraWire(0);
DFRobot_AXP313A cameraPowerDriver(0x36, &cameraWire);
const size_t MAX_FILE_SIZE = 8 * 1024 * 1024;
