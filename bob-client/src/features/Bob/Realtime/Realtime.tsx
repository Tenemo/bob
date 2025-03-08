import {
    Box,
    TextField,
    Button,
    Typography,
    CircularProgress,
} from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useRef, useEffect, useCallback } from 'react';

import {
    handleMessageSubmit,
    ConversationItem,
    Transcript,
    playAndUploadAudio,
    stopAudio,
} from './realtimeUtils';
import { WavPacker } from './wav_packer';
import { WavRecorder } from './wav_recorder';

import { useAppSelector } from 'app/hooks';
import { selectIsDebug, selectUseBobSpeaker } from 'features/Bob/bobSlice';
import { getPrompt } from 'features/Bob/getPrompt';
import {
    useUploadAudioMutation,
    useStopAudioMutation,
    useLazyHealthcheckQuery,
} from 'features/BobApi/bobApi';

const INITIAL_PROMPT: string = getPrompt('initial-start');
const VISION_PROMPT: string = getPrompt('vision');

type RealtimeProps = {
    onConnect: (client: RealtimeClient) => void;
    onDisconnect: () => void;
    getPhotoDescription: () => Promise<string>;
    isVisionLoading: boolean;
};

const Realtime = ({
    onConnect,
    onDisconnect,
    getPhotoDescription,
    isVisionLoading,
}: RealtimeProps): React.JSX.Element => {
    const [input, setInput] = useState<string>('');
    const [error, setError] = useState<string>('');
    const [isRecording, setIsRecording] = useState<boolean>(false);
    const [audioUrl, setAudioUrl] = useState<string>('');
    const [lastTranscript, setLastTranscript] = useState<Transcript[]>();
    const audioRef = useRef<HTMLAudioElement | null>(null);
    const [uploadAudio] = useUploadAudioMutation();
    const [stopAudioMutation] = useStopAudioMutation();
    const wavRecorderRef = useRef<WavRecorder>(
        new WavRecorder({ sampleRate: 24000 }),
    );
    const [triggerHealthcheck, { data: healthcheckData }] =
        useLazyHealthcheckQuery();
    const isDebug = useAppSelector(selectIsDebug);
    const useBobSpeaker = useAppSelector(selectUseBobSpeaker);

    const clientRef = useRef<RealtimeClient | null>(null);
    const [isConnected, setIsConnected] = useState<boolean>(false);
    const [isConnectInProgress, setIsConnectInProgress] =
        useState<boolean>(false);

    // We need to keep the same reference, because
    // conversation.item.completed otherwise has a reference
    // to the old value and a new reference is created for the new value.
    const useBobSpeakerRef = useRef<boolean>(useBobSpeaker);
    useEffect(() => {
        useBobSpeakerRef.current = useBobSpeaker;
    }, [useBobSpeaker]);

    const connectConversation = useCallback(
        async (apiKey?: string): Promise<void> => {
            if (!healthcheckData?.apiKey && !apiKey) {
                throw new Error('API key missing');
            }
            clientRef.current = new RealtimeClient({
                apiKey: healthcheckData?.apiKey ?? apiKey,
                // We aren't actually building the page with the key.
                // It's received from Bob during runtime and stored there.
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
        [healthcheckData?.apiKey, onConnect, uploadAudio],
    );

    const disconnectConversation = useCallback(async (): Promise<void> => {
        setIsConnected(false);
        onDisconnect();
        const client = clientRef.current;
        client?.disconnect();
        const wavRecorder = wavRecorderRef.current;
        await wavRecorder.end();
    }, [onDisconnect]);

    const startRecording = async (): Promise<void> => {
        setIsRecording(true);
        const client = clientRef.current;
        const wavRecorder = wavRecorderRef.current;
        await wavRecorder.record((data) => client?.appendInputAudio(data.mono));
    };

    const stopRecording = async (): Promise<void> => {
        setIsRecording(false);
        const client = clientRef.current;
        const wavRecorder = wavRecorderRef.current;
        await wavRecorder.pause();
        client?.createResponse();
    };

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

    const handleSubmit = (): void => {
        handleMessageSubmit(clientRef.current, input, setError);
        setInput('');
    };

    useEffect((): void => {
        if (lastTranscript) {
            const item = clientRef.current?.conversation
                .getItems()
                .slice(-1)[0];
            if (item?.formatted.audio) {
                const wavPacker = new WavPacker();
                const audioBlob = wavPacker.pack(24000, {
                    bitsPerSample: 16,
                    channels: [new Float32Array(item.formatted.audio)],
                    data: new Int16Array(item.formatted.audio),
                }).blob;
                const url = URL.createObjectURL(audioBlob);
                setAudioUrl(url);
            }
        }
    }, [lastTranscript]);

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
    ]);

    const handleSharePictureClick = useCallback(async (): Promise<void> => {
        try {
            const description = await getPhotoDescription();
            clientRef.current?.sendUserMessageContent([
                {
                    type: 'input_text',
                    text: `${VISION_PROMPT}\n${description}`,
                },
            ]);
        } catch (err) {
            setError(
                err instanceof Error ? err.message : 'Failed to share picture',
            );
        }
    }, [getPhotoDescription]);

    const showSpinner = isConnectInProgress || isVisionLoading;

    return (
        <Box
            sx={{
                mt: 1,
                display: 'flex',
                flexDirection: 'column',
                gap: 2,
                alignItems: 'flex-start',
            }}
        >
            {isConnected && (
                <Button
                    disabled={!isConnected}
                    onClick={(): void => {
                        void handleSharePictureClick();
                    }}
                    variant="outlined"
                >
                    Share picture
                </Button>
            )}
            <Box sx={{ display: 'flex', gap: 1, alignItems: 'center' }}>
                {isDebug && (
                    <>
                        <TextField
                            fullWidth
                            onChange={(e): void => {
                                setInput(e.target.value);
                            }}
                            onKeyDown={(e): void => {
                                if (e.key === 'Enter') handleSubmit();
                            }}
                            placeholder="Type your message..."
                            value={input}
                        />
                        <Button
                            disabled={!input.trim()}
                            onClick={(): void => {
                                stopAudio(stopAudioMutation, useBobSpeaker);
                                handleSubmit();
                            }}
                            variant="outlined"
                        >
                            Send
                        </Button>
                    </>
                )}
                {isConnected && (
                    <Button
                        disabled={!isConnected}
                        onMouseDown={(): void => {
                            stopAudio(stopAudioMutation, useBobSpeaker);
                            void startRecording();
                        }}
                        onMouseUp={(): void => {
                            void stopRecording();
                        }}
                        variant="contained"
                    >
                        {isRecording ? 'release to send' : 'push to talk'}
                    </Button>
                )}
                <div className="spacer" />
            </Box>
            <Button
                disabled={showSpinner}
                onClick={(): void => {
                    void handleConnectClick();
                }}
                variant="outlined"
            >
                {showSpinner ? (
                    <CircularProgress size={24} />
                ) : isConnected ? (
                    'Disconnect'
                ) : (
                    'Connect to Bob'
                )}
            </Button>
            {isDebug && (
                <>
                    <Button
                        color="error"
                        onClick={() => {
                            stopAudio(stopAudioMutation, useBobSpeaker);
                        }}
                        variant="outlined"
                    >
                        Stop audio
                    </Button>
                    {audioUrl && (
                        <Box sx={{ mt: 2 }}>
                            <audio controls ref={audioRef} src={audioUrl} />
                        </Box>
                    )}
                    {lastTranscript && (
                        <Typography>
                            Last response: {lastTranscript[0].transcript}
                        </Typography>
                    )}
                </>
            )}
            {error && <Typography color="error">Error: {error}</Typography>}
        </Box>
    );
};

export default Realtime;
