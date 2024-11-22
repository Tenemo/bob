#ifndef __wav_file_reader_h__
#define __wav_file_reader_h__

#include "AudioFile.h"
#include "I2SOutput.h"
#include <FS.h>

class WAVFileReader : public AudioFile {
  private:
    int m_num_channels;
    int m_sample_rate;
    bool m_is_complete;
    bool m_using_psram;
    uint8_t *m_psram_buffer;
    size_t m_buffer_size;
    size_t m_current_position;
    File m_file;
    size_t m_data_start;
    size_t m_data_length;

  public:
    WAVFileReader(const char *file_name);
    WAVFileReader(uint8_t *buffer, size_t size);
    ~WAVFileReader();
    int sampleRate() { return m_sample_rate; }
    void getFrames(Frame_t *frames, int number_frames);
    bool isComplete() { return m_is_complete; }
};

void playAudioFile(const char *filename, const bool announcePlayback = true);
void playAudioFromPSRAM(uint8_t *buffer, size_t size);
void stopPlayback(void);

#endif
