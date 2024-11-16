#ifndef SCREENLOGGER_H
#define SCREENLOGGER_H

#include "DFRobot_GDL.h"
#include <Arduino.h>

#define TFT_DC 3   // Data/Command pin
#define TFT_CS 18  // Chip Select pin
#define TFT_RST 38 // Reset pin
#define LCD_BL 21  // Backlight pin

#define MAX_LINES_ON_SCREEN 16 // Maximum number of lines on the screen
#define MAX_CHARS_PER_LINE 21  // Maximum number of characters per line
#define MAX_BUFFER_LINES 100   // Maximum number of lines stored in the buffer

class ScreenLogger {
  public:
    ScreenLogger();
    void begin();
    void print(const String &message);
    void println(const String &message);
    void clearScreen();
    void setTextSize(uint8_t size);
    void setTextColor(uint16_t color);

  private:
    DFRobot_ST7735_128x160_HW_SPI _screen;
    uint8_t _textSize;
    uint16_t _textColor;

    void refreshScreen();
    void addLineToBuffer(const String &line);
    void wrapAndAddLine(const String &line);
    void processMessage(const String &message);

    String _lines[MAX_BUFFER_LINES];
    int _lineCount;
    String _currentLine;
};

#endif // SCREENLOGGER_H
