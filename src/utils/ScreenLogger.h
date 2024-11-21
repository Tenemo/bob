#ifndef SCREENLOGGER_H
#define SCREENLOGGER_H

#include "DFRobot_GDL.h"
#include <Arduino.h>

/**
 * @brief Data/Command pin for the TFT display.
 */
#define TFT_DC 3 // Data/Command pin

/**
 * @brief Chip Select pin for the TFT display.
 */
#define TFT_CS 18 // Chip Select pin

/**
 * @brief Reset pin for the TFT display.
 */
#define TFT_RST 38 // Reset pin

/**
 * @brief Backlight control pin for the TFT display.
 */
#define LCD_BL 21 // Backlight pin

/**
 * @brief Maximum number of lines that can be displayed on the screen at once.
 */
#define MAX_LINES_ON_SCREEN 16 // Maximum number of lines on the screen

/**
 * @brief Maximum number of characters per line on the screen.
 */
#define MAX_CHARS_PER_LINE 21 // Maximum number of characters per line

/**
 * @brief Maximum number of lines that can be stored in the buffer.
 */
#define MAX_BUFFER_LINES 100 // Maximum number of lines stored in the buffer

/**
 * @class ScreenLogger
 * @brief Handles logging messages to both the serial monitor and a TFT screen.
 *
 * The ScreenLogger class provides methods to print messages to the serial
 * monitor and display them on a connected TFT screen. It manages a buffer of
 * messages to ensure that the display remains up-to-date with the latest logs.
 */
class ScreenLogger {
  public:
    /**
     * @brief Constructs a ScreenLogger instance.
     *
     * Initializes member variables and prepares the TFT screen for logging.
     */
    ScreenLogger();

    /**
     * @brief Initializes the screen logger.
     *
     * Sets up the serial communication, initializes the TFT screen, and
     * configures text properties for display.
     */
    void begin();

    /**
     * @brief Prints a message to the serial monitor and updates the screen.
     *
     * @param message The message to print.
     */
    void print(const String &message);

    /**
     * @brief Prints a message followed by a newline to the serial monitor and
     * updates the screen.
     *
     * @param message The message to print.
     */
    void println(const String &message);

    /**
     * @brief Template method to print a message of any type to the serial
     * monitor and update the screen.
     *
     * @tparam T The type of the message.
     * @param message The message to print.
     */
    template <typename T> void print(T message);

    /**
     * @brief Template method to print a message of any type followed by a
     * newline to the serial monitor and update the screen.
     *
     * @tparam T The type of the message.
     * @param message The message to print.
     */
    template <typename T> void println(T message);

  private:
    DFRobot_ST7735_128x160_HW_SPI _screen; /**< TFT screen instance */
    uint8_t _textSize;                     /**< Text size for display */
    uint16_t _textColor;                   /**< Text color for display */

    /**
     * @brief Refreshes the TFT screen with the latest log messages.
     *
     * Clears the screen and redraws the buffered log messages.
     */
    void refreshScreen();

    /**
     * @brief Adds a single line to the log buffer.
     *
     * @param line The line to add to the buffer.
     */
    void addLineToBuffer(const String &line);

    /**
     * @brief Wraps a long line into multiple lines based on the maximum
     * characters per line.
     *
     * @param line The line to wrap and add to the buffer.
     */
    void wrapAndAddLine(const String &line);

    /**
     * @brief Processes incoming messages by handling newlines and wrapping.
     *
     * @param message The message to process.
     */
    void processMessage(const String &message);

    String _lines[MAX_BUFFER_LINES]; /**< Buffer storing log lines */
    int _lineCount;      /**< Current number of lines in the buffer */
    String _currentLine; /**< Current line being processed */
};

template <typename T> void ScreenLogger::print(T message) {
    Serial.print(message);
    processMessage(String(message));
    refreshScreen();
}

template <typename T> void ScreenLogger::println(T message) {
    Serial.println(message);
    processMessage(String(message) + "\n");
    refreshScreen();
}

#endif // SCREENLOGGER_H
