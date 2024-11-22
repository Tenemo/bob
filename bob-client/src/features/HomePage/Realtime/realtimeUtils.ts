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

export const REALTIME_PROMPT = `
You are a chip on a breadboard and
want to find out more about the world around you.
`;

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
        // LEAVE IT FOR NOW, TO BE REMOVED LATER
        return;
        await uploadAudio(audioData);
    } catch (err) {
        setError(err instanceof Error ? err.message : 'Failed to upload audio');
    }
};

export const initializeRealtimeClient = async (
    apiKey: string | undefined,
    setStatus: (status: string) => void,
    setError: (error: string) => void,
    setLastTranscript: (transcript: Transcript[]) => void,
    uploadAudio: (audio: Int16Array) => Promise<unknown>,
): Promise<RealtimeClient> => {
    if (!apiKey) {
        throw new Error('OpenAI API key not found');
    }

    const client = new RealtimeClient({
        apiKey,
        dangerouslyAllowAPIKeyInBrowser: true,
    });

    client.updateSession({
        instructions: REALTIME_PROMPT,
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

let mediaRecorder: MediaRecorder | null = null;
let audioChunks: Blob[] = [];

export const startRecording = async (
    client: RealtimeClient | null,
    setError: (error: string) => void,
): Promise<void> => {
    if (!client) {
        setError('Client not initialized');
        return;
    }

    try {
        const stream = await navigator.mediaDevices.getUserMedia({
            audio: true,
        });
        mediaRecorder = new MediaRecorder(stream);
        audioChunks = [];

        mediaRecorder.ondataavailable = (event) => {
            audioChunks.push(event.data);
        };

        mediaRecorder.start(100); // Collect data every 100ms
    } catch (error) {
        setError('Failed to start recording');
        console.error(error);
    }
};

export const stopRecording = async (
    client: RealtimeClient | null,
    setError: (error: string) => void,
): Promise<void> => {
    if (!mediaRecorder || !client) {
        setError('No active recording');
        return;
    }

    return new Promise((resolve) => {
        if (!mediaRecorder) {
            resolve();
            return;
        }
        mediaRecorder.onstop = async () => {
            try {
                const audioBlob = new Blob(audioChunks, { type: 'audio/wav' });
                const arrayBuffer = await audioBlob.arrayBuffer();
                const audioData = new Int16Array(arrayBuffer);
                client.appendInputAudio(audioData);
                client.createResponse();
                resolve();
            } catch (error) {
                setError('Failed to process recording');
                console.error(error);
            }
        };

        mediaRecorder.stop();
        mediaRecorder.stream.getTracks().forEach((track) => {
            track.stop();
        });
        mediaRecorder = null;
    });
};
