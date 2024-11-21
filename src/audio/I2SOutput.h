#ifndef __sampler_base_h__
#define __sampler_base_h__

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

/**
 * Base Class for both the ADC and I2S sampler
 **/
class I2SOutput {
  private:
    // I2S write task
    TaskHandle_t m_i2sWriterTaskHandle;
    // i2s writer queue
    QueueHandle_t m_i2sQueue;
    // i2s port
    i2s_port_t m_i2sPort;
    // src of files for us to play
    AudioFile *m_sample_generator;

  public:
    void start(i2s_port_t i2sPort, i2s_pin_config_t &i2sPins,
               AudioFile *sample_generator);

    friend void i2sWriterTask(void *param);
};

#endif
