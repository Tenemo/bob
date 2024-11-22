#include "WAVFileReader.h"
#include "AudioFile.h"
#include "Globals.h"
#include "I2SOutput.h"
#include <FS.h>
#include <SPIFFS.h>

#pragma pack(push, 1)
typedef struct {
    char riff_header[4];
    int wav_size;
    char wave_header[4];
    char fmt_header[4];
    int fmt_chunk_size;
    short audio_format;
    short num_channels;
    int sample_rate;
    int byte_rate;
    short sample_alignment;
    short bit_depth;
    char data_header[4];
    int data_bytes;
} wav_header_t;
#pragma pack(pop)

static I2SOutput *currentOutput = nullptr;
static WAVFileReader *currentWav = nullptr;
static volatile bool isPlaying = false;

WAVFileReader::WAVFileReader(const char *file_name)
    : m_is_complete(false), m_using_psram(false), m_psram_buffer(nullptr) {
    if (!SPIFFS.exists(file_name)) {
        Serial.println(
            "Failed to open file! Have you uploaded the file system?");
        return;
    }
    m_file = SPIFFS.open(file_name, "r");

    wav_header_t wav_header;
    m_file.read((uint8_t *)&wav_header, sizeof(wav_header_t));

    if (wav_header.bit_depth != 16) {
        Serial.printf("ERROR: bit depth %d is not supported\n",
                      wav_header.bit_depth);
    }

    m_num_channels = wav_header.num_channels;
    m_sample_rate = wav_header.sample_rate;
    m_data_length = wav_header.data_bytes;
    m_data_start = m_file.position();
}

WAVFileReader::WAVFileReader(uint8_t *buffer, size_t size)
    : m_is_complete(false), m_using_psram(true), m_psram_buffer(buffer),
      m_buffer_size(size), m_current_position(0) {
    wav_header_t wav_header;
    memcpy(&wav_header, buffer, sizeof(wav_header_t));

    if (wav_header.bit_depth != 16) {
        Serial.printf("ERROR: bit depth %d is not supported\n",
                      wav_header.bit_depth);
    }

    m_num_channels = wav_header.num_channels;
    m_sample_rate = wav_header.sample_rate;
    m_data_length = wav_header.data_bytes;
    m_data_start = sizeof(wav_header_t);
    m_current_position = m_data_start;
}

WAVFileReader::~WAVFileReader() {
    if (!m_using_psram && m_file) {
        m_file.close();
    }
    if (m_using_psram && m_psram_buffer) {
        free(m_psram_buffer);
        m_psram_buffer = nullptr;
    }
}

void WAVFileReader::getFrames(Frame_t *frames, int number_frames) {
    if (m_is_complete || !isPlaying) {
        for (int i = 0; i < number_frames; i++) {
            frames[i].left = 0;
            frames[i].right = 0;
        }
        return;
    }

    for (int i = 0; i < number_frames; i++) {
        if ((m_using_psram && m_current_position >= m_buffer_size) ||
            (!m_using_psram && m_file.available() == 0) || !isPlaying) {
            m_is_complete = true;
            for (; i < number_frames; i++) {
                frames[i].left = 0;
                frames[i].right = 0;
            }
            return;
        }

        if (m_using_psram) {
            memcpy(&frames[i].left, m_psram_buffer + m_current_position,
                   sizeof(int16_t));
            m_current_position += sizeof(int16_t);
            if (m_num_channels == 1) {
                frames[i].right = frames[i].left;
            } else {
                memcpy(&frames[i].right, m_psram_buffer + m_current_position,
                       sizeof(int16_t));
                m_current_position += sizeof(int16_t);
            }
        } else {
            m_file.read((uint8_t *)(&frames[i].left), sizeof(int16_t));
            if (m_num_channels == 1) {
                frames[i].right = frames[i].left;
            } else {
                m_file.read((uint8_t *)(&frames[i].right), sizeof(int16_t));
            }
        }
    }
}

void playAudioFile(const char *filename, const bool announcePlayback) {
    stopPlayback();
    delay(50);

    if (announcePlayback) {
        logger.println("Playing audio file: " + String(filename));
    }
    currentWav = new WAVFileReader(filename);
    currentOutput = new I2SOutput();

    i2s_pin_config_t pins = getDefaultI2SPins();
    isPlaying = true;
    currentOutput->start(I2S_NUM_1, pins, currentWav);
}

void playAudioFromPSRAM(uint8_t *buffer, size_t size) {
    stopPlayback();
    delay(50);

    currentWav = new WAVFileReader(buffer, size);
    currentOutput = new I2SOutput();

    i2s_pin_config_t pins = getDefaultI2SPins();
    isPlaying = true;
    currentOutput->start(I2S_NUM_1, pins, currentWav);
}

void stopPlayback() {
    isPlaying = false;

    if (currentOutput != nullptr) {
        currentOutput->stop();
        delay(50);
        delete currentOutput;
        currentOutput = nullptr;
    }

    if (currentWav != nullptr) {
        delete currentWav;
        currentWav = nullptr;
    }
}
