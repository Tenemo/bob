#ifndef __i2s_output_h__
#define __i2s_output_h__

#include "driver/i2s.h"
#include <Arduino.h>

/**
 * @brief Returns the default I2S pin configuration.
 *
 * @return A `i2s_pin_config_t` structure with default pin assignments.
 */
inline i2s_pin_config_t getDefaultI2SPins() {
    i2s_pin_config_t pins = {
        .bck_io_num = GPIO_NUM_12,   /**< Bit Clock pin */
        .ws_io_num = GPIO_NUM_13,    /**< Left/Right Clock pin */
        .data_out_num = GPIO_NUM_14, /**< Serial Data Out pin */
        .data_in_num = -1            /**< Not used for output */
    };
    return pins;
}

class AudioFile;

/**
 * @class I2SOutput
 * @brief Manages audio output through the I2S peripheral.
 *
 * This class handles the initialization and management of the I2S peripheral
 * for audio output, interfacing with an `AudioFile` sample generator.
 */
class I2SOutput {
  private:
    TaskHandle_t m_i2sWriterTaskHandle; /**< Handle to the I2S writer task */
    QueueHandle_t m_i2sQueue;           /**< Queue for I2S events */
    i2s_port_t m_i2sPort;               /**< I2S port number */
    AudioFile *m_sample_generator; /**< Pointer to the audio sample generator */
    volatile bool m_is_running; /**< Indicates if the I2S output is running */

  public:
    /**
     * @brief Constructs an `I2SOutput` instance.
     *
     * Initializes member variables to default states.
     */
    I2SOutput() : m_is_running(false) {}

    /**
     * @brief Starts the I2S output with the specified configuration.
     *
     * Initializes the I2S driver, sets up the pins, and starts the writer task.
     *
     * @param i2sPort I2S port number to use.
     * @param i2sPins Reference to the I2S pin configuration.
     * @param sample_generator Pointer to an `AudioFile` instance for sample
     * generation.
     */
    void start(i2s_port_t i2sPort, i2s_pin_config_t &i2sPins,
               AudioFile *sample_generator);

    /**
     * @brief Stops the I2S output and cleans up resources.
     *
     * Signals the writer task to stop and deletes it.
     */
    void stop();

    /**
     * @brief Checks if the I2S output is currently running.
     *
     * @return `true` if running, `false` otherwise.
     */
    bool isRunning() { return m_is_running; }

    /**
     * @brief Friend function for the I2S writer task.
     *
     * Allows the `i2sWriterTask` function to access private members of
     * `I2SOutput`.
     *
     * @param param Pointer to the `I2SOutput` instance.
     */
    friend void i2sWriterTask(void *param);
};

#endif
