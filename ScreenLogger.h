#ifndef SCREENLOGGER_H
#define SCREENLOGGER_H

#include <Arduino.h>

#include "DFRobot_GDL.h"

#define TFT_DC 3   // Data/Command pin
#define TFT_CS 18  // Chip Select pin
#define TFT_RST 38 // Reset pin
#define LCD_BL 21  // Backlight pin

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
    uint16_t _cursorX;
    uint16_t _cursorY;
    uint8_t _textSize;
    uint16_t _textColor;
    void updateCursor();
    void checkScroll();
};

#endif // SCREENLOGGER_H
