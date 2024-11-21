#ifndef __i2s_output_h__
#define __i2s_output_h__

#include "driver/i2s.h"
#include <Arduino.h>

inline i2s_pin_config_t getDefaultI2SPins() {
    i2s_pin_config_t pins = {
        .bck_io_num = GPIO_NUM_12,   // BCK (Bit Clock)
        .ws_io_num = GPIO_NUM_13,    // LRCK (Left/Right Clock)
        .data_out_num = GPIO_NUM_14, // DATA (Serial Data)
        .data_in_num = -1            // Not used for output
    };
    return pins;
}

class AudioFile;

class I2SOutput {
  private:
    TaskHandle_t m_i2sWriterTaskHandle;
    QueueHandle_t m_i2sQueue;
    i2s_port_t m_i2sPort;
    AudioFile *m_sample_generator;
    volatile bool m_is_running;

  public:
    I2SOutput() : m_is_running(false) {}
    void start(i2s_port_t i2sPort, i2s_pin_config_t &i2sPins,
               AudioFile *sample_generator);
    void stop();
    bool isRunning() { return m_is_running; }

    friend void i2sWriterTask(void *param);
};

#endif
