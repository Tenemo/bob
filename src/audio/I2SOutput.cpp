
#include "driver/i2s.h"

#include "AudioFile.h"
#include "I2SOutput.h"
#include "WAVFileReader.h"

// number of frames to try and send at once (a frame is a left and right sample)
#define NUM_FRAMES_TO_SEND 512

void i2sWriterTask(void *param) {
    I2SOutput *output = (I2SOutput *)param;
    WAVFileReader *wav = (WAVFileReader *)output->m_sample_generator;
    int availableBytes = 0;
    int buffer_position = 0;
    Frame_t *frames = (Frame_t *)malloc(sizeof(Frame_t) * NUM_FRAMES_TO_SEND);

    output->m_is_running = true;

    while (output->m_is_running && !wav->isComplete()) {
        i2s_event_t evt;
        if (xQueueReceive(output->m_i2sQueue, &evt, portMAX_DELAY) == pdPASS) {
            if (evt.type == I2S_EVENT_TX_DONE) {
                size_t bytesWritten = 0;
                do {
                    if (availableBytes == 0) {
                        wav->getFrames(frames, NUM_FRAMES_TO_SEND);
                        availableBytes = NUM_FRAMES_TO_SEND * sizeof(Frame_t);
                        buffer_position = 0;
                    }

                    if (availableBytes > 0) {
                        i2s_write(output->m_i2sPort,
                                  buffer_position + (uint8_t *)frames,
                                  availableBytes, &bytesWritten, portMAX_DELAY);
                        availableBytes -= bytesWritten;
                        buffer_position += bytesWritten;
                    }
                } while (bytesWritten > 0 && !wav->isComplete());
            }
        }
    }

    // Clean up
    free(frames);
    i2s_zero_dma_buffer(output->m_i2sPort);
    output->m_is_running = false;
    vTaskDelete(NULL);
}

void I2SOutput::start(i2s_port_t i2sPort, i2s_pin_config_t &i2sPins,
                      AudioFile *sample_generator) {
    m_sample_generator = sample_generator;
    // i2s config for writing both channels of I2S
    i2s_config_t i2sConfig = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = static_cast<uint32_t>(m_sample_generator->sampleRate()),
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)(0x01),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 1024};

    m_i2sPort = i2sPort;
    // install and start i2s driver
    i2s_driver_install(m_i2sPort, &i2sConfig, 4, &m_i2sQueue);
    // set up the i2s pins
    i2s_set_pin(m_i2sPort, &i2sPins);
    // clear the DMA buffers
    i2s_zero_dma_buffer(m_i2sPort);
    // start a task to write samples to the i2s peripheral
    TaskHandle_t writerTaskHandle;
    xTaskCreate(i2sWriterTask, "i2s Writer Task", 4096, this, 1,
                &writerTaskHandle);
}

void I2SOutput::stop() {
    m_is_running = false;
    i2s_driver_uninstall(m_i2sPort);
}
