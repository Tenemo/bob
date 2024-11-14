#include "ScreenLogger.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <set>
#include <string>
#include <vector>

ScreenLogger::ScreenLogger()
    : textSize(2), lineHeight(16),
      maxLines((240 / 16) - 1) // Adjust based on your font and text size
{}

void ScreenLogger::begin() {
    tft.init();
    tft.setRotation(0); // Adjust rotation if needed
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(
        TFT_WHITE, TFT_BLACK); // Set text color to white with black background
    tft.setTextSize(textSize); // Set text size
    logBuffer.clear();
    addLine("TFT Initialized");
}

// Print a message with a newline
void ScreenLogger::println(const String &message) { addLine(message); }

// Print a message without a newline
void ScreenLogger::print(const String &message) {
    if (logBuffer.empty()) {
        addLine(message);
    } else {
        // Append to the last line
        logBuffer.back() += message;
        redrawScreen();
    }
}

// Add a line to the buffer and handle scrolling
void ScreenLogger::addLine(const String &message) {
    if (logBuffer.size() >= maxLines) {
        // Remove the oldest message
        logBuffer.erase(logBuffer.begin());
    }
    logBuffer.push_back(message);
    Serial.println(message);
    redrawScreen();
}

// Redraw the entire screen based on the buffer
void ScreenLogger::redrawScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    for (const auto &line : logBuffer) {
        tft.println(line);
    }
}
