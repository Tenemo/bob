// In src/audio/WAVFileReader.h:
#ifndef __wav_file_reader_h__
#define __wav_file_reader_h__

#include "AudioFile.h"
#include "I2SOutput.h"
#include <FS.h>

class WAVFileReader : public AudioFile {
  private:
    int m_num_channels;
    int m_sample_rate;
    File m_file;

  public:
    WAVFileReader(const char *file_name);
    ~WAVFileReader();
    int sampleRate() { return m_sample_rate; }
    void getFrames(Frame_t *frames, int number_frames);
};

void playAudioFile(const char *filename);

#endif
