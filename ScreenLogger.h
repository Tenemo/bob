#ifndef SCREENLOGGER_H
#define SCREENLOGGER_H

#include <TFT_eSPI.h>
#include <vector>
#include <string>

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
    TFT_eSPI tft;                           // TFT instance
    const int textSize;                     // Text size multiplier
    const int lineHeight;                   // Height of each text line in pixels
    const int maxLines;                     // Maximum number of lines that can be displayed
    std::vector<std::string> logBuffer;     // Buffer to store log messages
  
    // Add a line to the buffer and handle scrolling
    void addLine(const String &message);
  
    // Redraw the entire screen based on the buffer
    void redrawScreen();
};

#endif // SCREENLOGGER_H
