import { RealtimeClient } from '@openai/realtime-api-beta';

export type Transcript = {
    type: string;
    transcript: string;
};
export type ConversationItem = {
    type: string;
    role: string;
    content?: Transcript[];
    formatted?: { audio: Int16Array };
};

export const MAX_AUDIO_SIZE_BYTES = 8 * 1024 * 1024;

let activeAudioSource: AudioBufferSourceNode | null = null;
let activeAudioContext: AudioContext | null = null;

export const stopAudio = (): boolean => {
    if (activeAudioSource) {
        activeAudioSource.stop();
        activeAudioSource = null;
        if (activeAudioContext) {
            void activeAudioContext.close();
            activeAudioContext = null;
        }
        return true;
    }
    return false;
};

export const playAndUploadAudio = async (
    audioData: Int16Array,
    uploadAudio: (audio: Int16Array) => Promise<unknown>,
    setError: (error: string) => void,
    useBobSpeaker: boolean,
): Promise<void> => {
    const audioSizeBytes = audioData.length * 2;
    if (audioSizeBytes > MAX_AUDIO_SIZE_BYTES) {
        setError(
            `Audio size (${(audioSizeBytes / 1024 / 1024).toFixed(2)}MB) exceeds maximum allowed size (8MB)`,
        );
        return;
    }

    try {
        if (useBobSpeaker) {
            await uploadAudio(audioData);
        } else {
            stopAudio();

            const audioContext = new AudioContext();
            activeAudioContext = audioContext;
            const buffer = audioContext.createBuffer(
                1,
                audioData.length,
                24000,
            );
            const channelData = buffer.getChannelData(0);

            for (let i = 0; i < audioData.length; i++) {
                channelData[i] = audioData[i] / 32768.0;
            }
            const source = audioContext.createBufferSource();
            activeAudioSource = source;
            source.buffer = buffer;
            source.connect(audioContext.destination);
            source.start();

            source.onended = () => {
                activeAudioSource = null;
                void audioContext.close();
                activeAudioContext = null;
            };
        }
    } catch (err) {
        setError(err instanceof Error ? err.message : 'Failed to upload audio');
    }
};

export const handleMessageSubmit = (
    client: RealtimeClient | null,
    message: string,
    setError: (error: string) => void,
): void => {
    if (!client) {
        setError('Client not initialized');
        return;
    }

    try {
        client.sendUserMessageContent([{ type: 'input_text', text: message }]);
    } catch (error) {
        setError(
            error instanceof Error ? error.message : 'Failed to send message',
        );
    }
};
