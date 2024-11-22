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
    // Check if we've already completed playback
    if (m_is_complete) {
        // Fill the buffer with silence (zeros)
        for (int i = 0; i < number_frames; i++) {
            frames[i].left = 0;
            frames[i].right = 0;
        }
        return;
    }
    // Fill the buffer with data from the file
    for (int i = 0; i < number_frames; i++) {
        // If we've reached the end of the file, fill remaining buffer with
        // silence and mark playback as complete
        if (m_file.available() == 0) {
            m_is_complete = true;
            // Fill the rest of the buffer with silence
            for (; i < number_frames; i++) {
                frames[i].left = 0;
                frames[i].right = 0;
            }
            return;
        }
        // Read in the next sample to the left channel
        m_file.read((uint8_t *)(&frames[i].left), sizeof(int16_t));
        // if we only have one channel duplicate the sample for the right
        // channel
        if (m_num_channels == 1) {
            frames[i].right = frames[i].left;
        } else {
            // otherwise read in the right channel sample
            m_file.read((uint8_t *)(&frames[i].right), sizeof(int16_t));
        }
    }
}
void playAudioFile(const char *filename) {
    if (currentOutput != nullptr) {
        logger.println("Playback already in progress. Cancelling to play " +
                       String(filename));
        currentOutput->stop();
        delete currentOutput;
        currentOutput = nullptr;
    }
    if (currentWav != nullptr) {
        delete currentWav;
        currentWav = nullptr;
    }
    currentWav = new WAVFileReader(filename);
    currentOutput = new I2SOutput();
    i2s_pin_config_t pins = getDefaultI2SPins();
    logger.println("Playing audio file: " + String(filename));
    currentOutput->start(I2S_NUM_1, pins, currentWav);
}

void stopPlayback() {
    if (currentOutput != nullptr) {
        currentOutput->stop();
        delete currentOutput;
        currentOutput = nullptr;
    }
    if (currentWav != nullptr) {
        delete currentWav;
        currentWav = nullptr;
    }
}
