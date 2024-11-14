#ifndef SCREENLOGGER_H
#define SCREENLOGGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <set>
#include <vector>

class ScreenLogger {
  public:
    ScreenLogger();

    // Initialize the TFT display
    void begin();

    // Print a message with a newline
    void println(const String &message);

    // Print a message without a newline
    void print(const String &message);

  private:
    TFT_eSPI tft;         // TFT instance
    const int textSize;   // Text size multiplier
    const int lineHeight; // Height of each text line in pixels
    const int maxLines;   // Maximum number of lines that can be displayed
    std::vector<String> logBuffer; // Buffer to store log lines

    // Add a message to the buffer and handle scrolling
    void addMessage(const String &message);

    // Redraw the entire screen based on the buffer
    void redrawScreen();

    // Split a message into lines that fit the screen width
    std::vector<String> splitMessageIntoLines(const String &message);
};

#endif // SCREENLOGGER_H
