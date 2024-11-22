import { RealtimeClient } from '@openai/realtime-api-beta';

export type ConversationItem = {
    type: string;
    role: string;
    content?: string;
    formatted?: { audio: Int16Array };
};

export const MAX_AUDIO_SIZE_BYTES = 8 * 1024 * 1024;

export const playAndUploadAudio = async (
    audioData: Int16Array,
    uploadAudio: (audio: Int16Array) => Promise<unknown>,
    setError: (error: string) => void,
): Promise<void> => {
    const audioSizeBytes = audioData.length * 2;
    if (audioSizeBytes > MAX_AUDIO_SIZE_BYTES) {
        setError(
            `Audio size (${(audioSizeBytes / 1024 / 1024).toFixed(2)}MB) exceeds maximum allowed size (8MB)`,
        );
        return;
    }

    const audioContext = new AudioContext();
    const buffer = audioContext.createBuffer(1, audioData.length, 24000);
    const channelData = buffer.getChannelData(0);

    for (let i = 0; i < audioData.length; i++) {
        channelData[i] = audioData[i] / 32768.0;
    }

    const source = audioContext.createBufferSource();
    source.buffer = buffer;
    source.connect(audioContext.destination);
    source.start();

    try {
        await uploadAudio(audioData);
    } catch (err) {
        setError(err instanceof Error ? err.message : 'Failed to upload audio');
    }
};

export const initializeRealtimeClient = async (
    apiKey: string | undefined,
    setStatus: (status: string) => void,
    setError: (error: string) => void,
    setLastTranscript: (transcript: string) => void,
    uploadAudio: (audio: Int16Array) => Promise<unknown>,
): Promise<RealtimeClient> => {
    if (!apiKey) {
        throw new Error('OpenAI API key not found');
    }

    const client = new RealtimeClient({
        apiKey,
        dangerouslyAllowAPIKeyInBrowser: true,
    });

    client.updateSession({ voice: 'coral' });

    client.on(
        'conversation.item.completed',
        ({ item }: { item: ConversationItem }) => {
            if (item.type === 'message' && item.role === 'assistant') {
                if (item.content) {
                    setLastTranscript(item.content);
                }
                if (item.formatted?.audio) {
                    void playAndUploadAudio(
                        item.formatted.audio,
                        uploadAudio,
                        setError,
                    );
                }
            }
        },
    );

    await client.connect();
    setStatus('Connected');
    setError('');

    return client;
};

export const handleMessageSubmit = (
    client: RealtimeClient | null,
    input: string,
    setError: (error: string) => void,
): void => {
    if (!client || !input.trim()) return;

    try {
        client.sendUserMessageContent([
            { type: 'input_text', text: input.trim() },
        ]);
    } catch (err) {
        setError(err instanceof Error ? err.message : 'Failed to send message');
    }
};
