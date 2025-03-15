import { RealtimeClient } from '@openai/realtime-api-beta';
import { useState, useRef, useCallback, useEffect } from 'react';

import {
    ConversationItem,
    Transcript,
    playAndUploadAudio,
} from './realtimeUtils';
import { WavRecorder } from './wav_recorder';

import { getPrompt } from 'features/Bob/getPrompt';

const INITIAL_PROMPT: string = getPrompt('initial-start');

type UseRealtimeConnectionProps = {
    onConnect: (client: RealtimeClient) => void;
    onDisconnect: () => void;
    getPhotoDescription: () => Promise<string>;
    uploadAudio: (audio: Int16Array) => Promise<unknown>;
    triggerHealthcheck: () => Promise<{ data?: { apiKey?: string } }>;
    healthcheckData?: { apiKey?: string };
    setError: (error: string) => void;
    setLastTranscript: (transcript?: Transcript[]) => void;
    useBobSpeakerRef: React.RefObject<boolean>;
};

export const useRealtimeConnection = ({
    onConnect,
    onDisconnect,
    getPhotoDescription,
    uploadAudio,
    triggerHealthcheck,
    healthcheckData,
    setError,
    setLastTranscript,
    useBobSpeakerRef,
}: UseRealtimeConnectionProps): {
    clientRef: React.MutableRefObject<RealtimeClient | null>;
    wavRecorderRef: React.MutableRefObject<WavRecorder>;
    isConnected: boolean;
    isConnectInProgress: boolean;
    handleConnectClick: () => Promise<void>;
} => {
    const clientRef = useRef<RealtimeClient | null>(null);
    const wavRecorderRef = useRef<WavRecorder>(
        new WavRecorder({ sampleRate: 24000 }),
    );
    const [isConnected, setIsConnected] = useState<boolean>(false);
    const [isConnectInProgress, setIsConnectInProgress] =
        useState<boolean>(false);

    const connectConversation = useCallback(
        async (apiKey?: string): Promise<void> => {
            if (!healthcheckData?.apiKey && !apiKey) {
                throw new Error('API key missing');
            }
            clientRef.current = new RealtimeClient({
                // We aren't actually building the page with the key.
                // It's received from Bob during runtime and stored there.
                apiKey: healthcheckData?.apiKey ?? apiKey,
                dangerouslyAllowAPIKeyInBrowser: true,
            });
            const client = clientRef.current;
            client.updateSession({
                input_audio_transcription: { model: 'whisper-1' },
            });
            client.updateSession({ voice: 'shimmer' });

            client.on('error', (event: unknown) => {
                console.error(event);
            });
            client.addTool(
                {
                    name: 'camera_capture',
                    parameters: {},
                    description: getPrompt('camera'),
                },
                async () => {
                    try {
                        const description = await getPhotoDescription();
                        return description;
                    } catch (error) {
                        console.error('Camera capture tool error:', error);
                        throw error;
                    }
                },
            );

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
                                useBobSpeakerRef,
                            );
                        }
                    }
                },
            );
            const wavRecorder = wavRecorderRef.current;
            setIsConnected(true);
            onConnect(client);
            await wavRecorder.begin();
            await client.connect();
            client.sendUserMessageContent([
                {
                    type: `input_text`,
                    text: `\n${INITIAL_PROMPT}`,
                },
            ]);
        },
        [
            getPhotoDescription,
            healthcheckData?.apiKey,
            onConnect,
            uploadAudio,
            setLastTranscript,
            setError,
            useBobSpeakerRef,
        ],
    );

    const disconnectConversation = useCallback(async (): Promise<void> => {
        setIsConnected(false);
        onDisconnect();
        const client = clientRef.current;
        client?.disconnect();
        const wavRecorder = wavRecorderRef.current;
        await wavRecorder.end();
    }, [onDisconnect]);

    const handleConnectClick = useCallback(async (): Promise<void> => {
        setError('');
        if (isConnected) {
            setIsConnectInProgress(true);
            await disconnectConversation();
            setIsConnectInProgress(false);
            return;
        }
        setIsConnectInProgress(true);
        try {
            let apiKey = healthcheckData?.apiKey;
            if (!apiKey) {
                const result = await triggerHealthcheck();
                if (!result.data?.apiKey) {
                    throw new Error('Failed to get API key from healthcheck');
                }
                apiKey = result.data.apiKey;
            }
            await connectConversation(apiKey);
        } catch (connectError) {
            setError(
                connectError instanceof Error
                    ? connectError.message
                    : 'Analysis failed',
            );
        }
        setIsConnectInProgress(false);
    }, [
        isConnected,
        disconnectConversation,
        healthcheckData?.apiKey,
        connectConversation,
        triggerHealthcheck,
        setError,
    ]);

    useEffect((): (() => void) => {
        if (!clientRef.current)
            return () => {
                return;
            };

        const client = clientRef.current;
        return () => {
            client.reset();
        };
    }, [uploadAudio]);

    return {
        clientRef,
        wavRecorderRef,
        isConnected,
        isConnectInProgress,
        handleConnectClick,
    };
};
