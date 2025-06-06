/**
 * Converts an Int16Array to a WAV file blob
 * @param audioData - The audio data as Int16Array
 * @param sampleRate - The sample rate of the audio (default: 24000)
 * @returns Blob - WAV file blob
 */
export const convertToWav = (
    audioData: Int16Array,
    sampleRate = 24000,
): Blob => {
    const buffer = new ArrayBuffer(44 + audioData.length * 2);
    const view = new DataView(buffer);

    // Write WAV header
    // "RIFF" identifier
    view.setUint8(0, 0x52); // R
    view.setUint8(1, 0x49); // I
    view.setUint8(2, 0x46); // F
    view.setUint8(3, 0x46); // F

    // File length minus RIFF identifier length and file description length
    view.setUint32(4, 36 + audioData.length * 2, true);

    // "WAVE" identifier
    view.setUint8(8, 0x57); // W
    view.setUint8(9, 0x41); // A
    view.setUint8(10, 0x56); // V
    view.setUint8(11, 0x45); // E

    // "fmt " chunk identifier
    view.setUint8(12, 0x66); // f
    view.setUint8(13, 0x6d); // m
    view.setUint8(14, 0x74); // t
    view.setUint8(15, 0x20); // " "

    // Chunk length
    view.setUint32(16, 16, true);
    // Sample format (raw)
    view.setUint16(20, 1, true);
    // Channel count
    view.setUint16(22, 1, true);
    // Sample rate
    view.setUint32(24, sampleRate, true);
    // Byte rate (sample rate * block align)
    view.setUint32(28, sampleRate * 2, true);
    // Block align (channel count * bytes per sample)
    view.setUint16(32, 2, true);
    // Bits per sample
    view.setUint16(34, 16, true);

    // "data" chunk identifier
    view.setUint8(36, 0x64); // d
    view.setUint8(37, 0x61); // a
    view.setUint8(38, 0x74); // t
    view.setUint8(39, 0x61); // a

    // Chunk length
    view.setUint32(40, audioData.length * 2, true);

    // Write audio data
    for (let i = 0; i < audioData.length; i++) {
        view.setInt16(44 + i * 2, audioData[i], true);
    }

    return new Blob([buffer], { type: 'audio/wav' });
};
