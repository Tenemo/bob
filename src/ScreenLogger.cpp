#include "ScreenLogger.h"

ScreenLogger::ScreenLogger()
    : _screen(TFT_DC, TFT_CS, TFT_RST), _cursorX(0), _cursorY(0), _textSize(1),
      _textColor(COLOR_RGB565_WHITE) {}

void ScreenLogger::begin() {
    Serial.begin(115200);
    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH); // Turn on backlight
    _screen.begin();
    _screen.setRotation(0);
    _screen.fillScreen(COLOR_RGB565_BLACK);
    _screen.setTextColor(_textColor);
    _screen.setTextSize(_textSize);
    _screen.setCursor(_cursorX, _cursorY);
}

void ScreenLogger::print(const String &message) {
    // Print to Serial
    Serial.print(message);

    // Print to screen
    _screen.print(message);

    // Update cursor position
    updateCursor();
    checkScroll();
}

void ScreenLogger::println(const String &message) {
    // Print to Serial
    Serial.println(message);

    // Print to screen
    _screen.println(message);

    // Update cursor position
    updateCursor();
    checkScroll();
}

void ScreenLogger::clearScreen() {
    _screen.fillScreen(COLOR_RGB565_BLACK);
    _cursorX = 0;
    _cursorY = 0;
    _screen.setCursor(_cursorX, _cursorY);
}

void ScreenLogger::setTextSize(uint8_t size) {
    _textSize = size;
    _screen.setTextSize(_textSize);
}

void ScreenLogger::setTextColor(uint16_t color) {
    _textColor = color;
    _screen.setTextColor(_textColor);
}

void ScreenLogger::updateCursor() {
    // Update cursor positions
    _cursorX = _screen.getCursorX();
    _cursorY = _screen.getCursorY();
}

void ScreenLogger::checkScroll() {
    // Calculate the maximum Y position based on text size
    uint16_t maxY = _screen.height() -
                    (_textSize * 8); // Assuming 8 pixels per character height

    if (_cursorY >= maxY) {
        // Scroll up by clearing the screen and resetting the cursor
        clearScreen();
    }
}
