#ifndef __audio_file_h__
#define __audio_file_h__

#include <Arduino.h>

/**
 * @struct Frame_t
 * @brief Represents a single audio frame with left and right channels.
 */
typedef struct {
    int16_t left;  /**< Left channel sample */
    int16_t right; /**< Right channel sample */
} Frame_t;

/**
 * @class AudioFile
 * @brief Abstract base class for audio sample generators.
 *
 * This class defines the interface for audio sample generators, which provide
 * audio frames to be processed or played.
 */
class AudioFile {
  public:
    /**
     * @brief Retrieves the sample rate of the audio file.
     *
     * @return Sample rate in Hz.
     */
    virtual int sampleRate() = 0;

    /**
     * @brief Fills the provided buffer with audio frames.
     *
     * This method should populate the provided frames buffer with the specified
     * number of audio frames. Each frame contains left and right channel
     * samples.
     *
     * @param frames Pointer to the buffer where audio frames will be stored.
     * @param number_frames Number of frames to generate.
     */
    virtual void getFrames(Frame_t *frames, int number_frames) = 0;
};

#endif
