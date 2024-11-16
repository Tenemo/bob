#include "ScreenLogger.h"

ScreenLogger::ScreenLogger()
    : _screen(TFT_DC, TFT_CS, TFT_RST), _textSize(1),
      _textColor(COLOR_RGB565_WHITE), _lineCount(0), _currentLine("") {}

void ScreenLogger::begin() {
    Serial.begin(115200);
    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH); // Turn on backlight
    _screen.begin();
    _screen.setRotation(0);
    _screen.fillScreen(COLOR_RGB565_BLACK);
    _screen.setTextColor(_textColor);
    _screen.setTextSize(_textSize);
    _screen.setCursor(0, 0);
}

void ScreenLogger::print(const String &message) {
    Serial.print(message);
    processMessage(message);
    refreshScreen();
}

void ScreenLogger::println(const String &message) {
    Serial.println(message);
    processMessage(message + "\n");
    refreshScreen();
}

void ScreenLogger::processMessage(const String &message) {
    String msg = message;
    int start = 0;
    int msgLength = msg.length();

    while (start < msgLength) {
        int newlineIndex = msg.indexOf('\n', start);
        String linePart;

        if (newlineIndex != -1) {
            linePart = msg.substring(start, newlineIndex);
            start = newlineIndex + 1;
        } else {
            linePart = msg.substring(start);
            start = msgLength;
        }

        // Append to the current line
        _currentLine += linePart;

        // Wrap and add the current line if a newline was found
        if (newlineIndex != -1) {
            wrapAndAddLine(_currentLine);
            _currentLine = "";
        } else {
            // Check if the current line exceeds the maximum characters per line
            while (_currentLine.length() >= MAX_CHARS_PER_LINE) {
                String subLine = _currentLine.substring(0, MAX_CHARS_PER_LINE);
                wrapAndAddLine(subLine);
                _currentLine = _currentLine.substring(MAX_CHARS_PER_LINE);
            }
        }
    }
}

void ScreenLogger::wrapAndAddLine(const String &line) {
    int lineLength = line.length();
    int start = 0;

    while (start < lineLength) {
        int end = start + MAX_CHARS_PER_LINE;
        if (end > lineLength) {
            end = lineLength;
        }
        String subLine = line.substring(start, end);
        addLineToBuffer(subLine);
        start = end;
    }
}

void ScreenLogger::addLineToBuffer(const String &line) {
    // Add the new line to the buffer
    if (_lineCount < MAX_BUFFER_LINES) {
        _lines[_lineCount++] = line;
    } else {
        // Shift buffer up to make room for the new line
        for (int i = 0; i < MAX_BUFFER_LINES - 1; i++) {
            _lines[i] = _lines[i + 1];
        }
        _lines[MAX_BUFFER_LINES - 1] = line;
    }
}

void ScreenLogger::refreshScreen() {
    // Clear the screen
    _screen.fillScreen(COLOR_RGB565_BLACK);
    _screen.setCursor(0, 0);

    // Set text properties
    _screen.setTextColor(_textColor);
    _screen.setTextSize(_textSize);

    int startLine = 0;
    if (_lineCount > MAX_LINES_ON_SCREEN) {
        startLine = _lineCount - MAX_LINES_ON_SCREEN;
    }

    // Draw only the lines that fit on the screen
    for (int i = startLine; i < _lineCount; i++) {
        _screen.println(_lines[i]);
    }
}

void ScreenLogger::clearScreen() {
    _screen.fillScreen(COLOR_RGB565_BLACK);
    _screen.setCursor(0, 0);

    // Clear the lines buffer
    _lineCount = 0;
    _currentLine = "";
}

void ScreenLogger::setTextSize(uint8_t size) {
    _textSize = size;
    _screen.setTextSize(_textSize);
}

void ScreenLogger::setTextColor(uint16_t color) {
    _textColor = color;
    _screen.setTextColor(_textColor);
}
