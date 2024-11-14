#include "ScreenLogger.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <set>
#include <string>
#include <vector>

ScreenLogger::ScreenLogger() : textSize(2), lineHeight(16), maxLines(20) {}

void ScreenLogger::begin() {
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(textSize);
    logBuffer.clear();
    println("TFT Initialized");
}

// Print a message with a newline
void ScreenLogger::println(const String &message) { addMessage(message); }

// Print a message without a newline
void ScreenLogger::print(const String &message) {
    if (logBuffer.empty()) {
        addMessage(message);
    } else {
        // Append to the last line in the buffer
        String updatedMessage = logBuffer.back() + message;
        logBuffer.pop_back();
        addMessage(updatedMessage);
    }
}

// Add a message to the buffer and handle scrolling
void ScreenLogger::addMessage(const String &message) {
    // Split the message into lines based on screen width
    std::vector<String> lines = splitMessageIntoLines(message);

    // Add lines to the buffer
    for (const auto &line : lines) {
        if (logBuffer.size() >= maxLines) {
            // Remove the oldest line
            logBuffer.erase(logBuffer.begin());
        }
        logBuffer.push_back(line);
    }
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

// Split a message into lines that fit the screen width
std::vector<String> ScreenLogger::splitMessageIntoLines(const String &message) {
    std::vector<String> lines;
    String currentLine = "";
    String word = "";
    int cursorX = 0;

    for (unsigned int i = 0; i < message.length(); i++) {
        char c = message.charAt(i);

        if (c == ' ' || c == '\n') {
            int wordWidth = tft.textWidth(word + " ");
            if (cursorX + wordWidth > tft.width()) {
                // Word doesn't fit in the current line, wrap to next line
                lines.push_back(currentLine);
                currentLine = word + " ";
                cursorX = tft.textWidth(currentLine);
            } else {
                // Add word to current line
                currentLine += word + " ";
                cursorX += wordWidth;
            }
            word = "";
        } else {
            word += c;
        }
    }

    // Add any remaining word
    if (word.length() > 0) {
        int wordWidth = tft.textWidth(word);
        if (cursorX + wordWidth > tft.width()) {
            // Word doesn't fit in the current line, wrap to next line
            lines.push_back(currentLine);
            currentLine = word;
        } else {
            // Add word to current line
            currentLine += word;
        }
    }

    // Add the last line
    if (currentLine.length() > 0) {
        lines.push_back(currentLine);
    }

    return lines;
}
