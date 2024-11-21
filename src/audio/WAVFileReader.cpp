#include "WAVFileReader.h"
#include "AudioFile.h"
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

WAVFileReader::WAVFileReader(const char *file_name) : m_is_complete(false) {
    if (!SPIFFS.exists(file_name)) {
        Serial.println(
            "****** Failed to open file! Have you uploaded the file system?");
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

WAVFileReader::~WAVFileReader() { m_file.close(); }

void WAVFileReader::getFrames(Frame_t *frames, int number_frames) {
    if (m_is_complete) {
        // Fill with silence if playback is complete
        memset(frames, 0, number_frames * sizeof(Frame_t));
        return;
    }

    size_t current_pos = m_file.position() - m_data_start;
    size_t bytes_remaining = m_data_length - current_pos;
    int frames_to_read = number_frames;

    // If we don't have enough data left for all requested frames
    if (bytes_remaining < (frames_to_read * sizeof(int16_t) * m_num_channels)) {
        frames_to_read = bytes_remaining / (sizeof(int16_t) * m_num_channels);
    }

    // Read available frames
    for (int i = 0; i < frames_to_read; i++) {
        m_file.read((uint8_t *)(&frames[i].left), sizeof(int16_t));
        if (m_num_channels == 1) {
            frames[i].right = frames[i].left;
        } else {
            m_file.read((uint8_t *)(&frames[i].right), sizeof(int16_t));
        }
    }

    // Fill remaining frames with silence if we've reached the end
    if (frames_to_read < number_frames) {
        memset(&frames[frames_to_read], 0,
               (number_frames - frames_to_read) * sizeof(Frame_t));
        m_is_complete = true;
    }
}

void playAudioFile(const char *filename) {
    WAVFileReader *wav = new WAVFileReader(filename);
    I2SOutput *output = new I2SOutput();
    i2s_pin_config_t pins = getDefaultI2SPins();
    output->start(I2S_NUM_1, pins, wav);
}
