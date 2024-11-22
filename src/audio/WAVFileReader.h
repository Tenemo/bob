#ifndef __wav_file_reader_h__
#define __wav_file_reader_h__

#include "AudioFile.h"
#include "I2SOutput.h"
#include <FS.h>

/**
 * @class WAVFileReader
 * @brief Reads and parses WAV audio files from the file system.
 *
 * This class provides functionality to read WAV files from SPIFFS, parse their
 * headers, and supply audio frames to an `AudioFile` interface.
 */
class WAVFileReader : public AudioFile {
  private:
    int m_num_channels;   /**< Number of audio channels */
    int m_sample_rate;    /**< Sample rate of the audio file */
    File m_file;          /**< File handle for the WAV file */
    bool m_is_complete;   /**< Indicates if the entire file has been read */
    size_t m_data_start;  /**< Start position of audio data in the file */
    size_t m_data_length; /**< Total length of audio data in bytes */

  public:
    /**
     * @brief Constructs a `WAVFileReader` instance and opens the specified
     * file.
     *
     * Parses the WAV file header to extract audio format information.
     *
     * @param file_name Path to the WAV file in SPIFFS.
     */
    WAVFileReader(const char *file_name);

    /**
     * @brief Destructs the `WAVFileReader` instance and closes the file.
     */
    ~WAVFileReader();

    /**
     * @brief Retrieves the sample rate of the WAV file.
     *
     * @return Sample rate in Hz.
     */
    int sampleRate() { return m_sample_rate; }

    /**
     * @brief Fills the provided buffer with audio frames from the WAV file.
     *
     * Reads the specified number of frames from the file and populates the
     * buffer. If the end of the file is reached, remaining frames are filled
     * with silence.
     *
     * @param frames Pointer to the buffer where audio frames will be stored.
     * @param number_frames Number of frames to read.
     */
    void getFrames(Frame_t *frames, int number_frames);

    /**
     * @brief Checks if the entire WAV file has been read.
     *
     * @return `true` if the file has been completely read, `false` otherwise.
     */
    bool isComplete() { return m_is_complete; }
};

/**
 * @brief Initiates playback of the specified WAV audio file.
 *
 * Creates a `WAVFileReader` and `I2SOutput` instance to play the audio file.
 *
 * @param filename Path to the WAV file in SPIFFS.
 * @param announcePlayback Flag to indicate if audio playback should be
 * announced via ogger.
 */
void playAudioFile(const char *filename, const bool announcePlayback = true);

#endif
