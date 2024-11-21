#ifndef STARTUP_H
#define STARTUP_H

#include <Arduino.h>
#include <WiFi.h>

// Update the function signature to return a bool
bool connectToWiFi();
void initializeStartup();
bool performHealthCheck();

#endif // STARTUP_H
