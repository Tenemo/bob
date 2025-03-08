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
} from './realtimeUtils';
import { WavPacker } from './wav_packer';
import { WavRecorder } from './wav_recorder';

import { useAppSelector } from 'app/hooks';
import { selectIsDebug } from 'features/Bob/bobSlice';
import { getPrompt } from 'features/Bob/getPrompt';
import {
    useUploadAudioMutation,
    useHealthcheckQueryState,
} from 'features/BobApi/bobApi';

const INITIAL_PROMPT: string = getPrompt('initial-start');
const VISION_GUIDANCE_PROMPT: string = getPrompt('initial-vision');
const NEW_PHOTO_PROMPT: string = getPrompt('new-photo');

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
    const wavRecorderRef = useRef<WavRecorder>(
        new WavRecorder({ sampleRate: 24000 }),
    );
    const { data: healthcheckData } = useHealthcheckQueryState(undefined);
    const isDebug = useAppSelector(selectIsDebug);

    const clientRef = useRef<RealtimeClient>(
        new RealtimeClient({
            apiKey: healthcheckData?.apiKey ?? 'MISSING API KEY',
            // We aren't actually building the page with the key.
            // It's received from Bob during runtime and stored there.
            dangerouslyAllowAPIKeyInBrowser: true,
        }),
    );
    const [isConnected, setIsConnected] = useState<boolean>(false);
    const [isConnectInProgress, setIsConnectInProgress] =
        useState<boolean>(false);

    const connectConversation = useCallback(
        async (photoDescription: string): Promise<void> => {
            const client = clientRef.current;
            const wavRecorder = wavRecorderRef.current;
            setIsConnected(true);
            onConnect(client);
            await wavRecorder.begin();
            await client.connect();
            client.sendUserMessageContent([
                {
                    type: `input_text`,
                    text: `\n${INITIAL_PROMPT}${photoDescription}\n${VISION_GUIDANCE_PROMPT}`,
                },
            ]);
        },
        [onConnect],
    );

    const disconnectConversation = useCallback(async (): Promise<void> => {
        setIsConnected(false);
        onDisconnect();
        const client = clientRef.current;
        client.disconnect();
        const wavRecorder = wavRecorderRef.current;
        await wavRecorder.end();
    }, [onDisconnect]);

    const startRecording = async (): Promise<void> => {
        setIsRecording(true);
        const client = clientRef.current;
        const wavRecorder = wavRecorderRef.current;
        await wavRecorder.record((data) => client.appendInputAudio(data.mono));
    };

    const stopRecording = async (): Promise<void> => {
        setIsRecording(false);
        const client = clientRef.current;
        const wavRecorder = wavRecorderRef.current;
        await wavRecorder.pause();
        client.createResponse();
    };

    useEffect((): (() => void) => {
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
                        );
                    }
                }
            },
        );
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
            const item = clientRef.current.conversation.getItems().slice(-1)[0];
            if (item.formatted.audio) {
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
        if (isConnected) {
            setIsConnectInProgress(true);
            await disconnectConversation();
            setIsConnectInProgress(false);
            return;
        }
        setIsConnectInProgress(true);
        try {
            const photoDescription = await getPhotoDescription();
            await connectConversation(photoDescription);
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
        getPhotoDescription,
        connectConversation,
    ]);

    const handleSharePictureClick = useCallback(async (): Promise<void> => {
        try {
            const description = await getPhotoDescription();
            clientRef.current.sendUserMessageContent([
                {
                    type: 'input_text',
                    text: `${NEW_PHOTO_PROMPT}\n${description}`,
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
                mt: 4,
                display: 'flex',
                flexDirection: 'column',
                gap: 2,
                alignItems: 'flex-start',
            }}
        >
            <Button
                disabled={!isConnected}
                onClick={(): void => {
                    void handleSharePictureClick();
                }}
                variant="contained"
            >
                Share picture
            </Button>
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
                                handleSubmit();
                            }}
                            variant="contained"
                        >
                            Send
                        </Button>
                    </>
                )}
                {isConnected && (
                    <Button
                        disabled={!isConnected}
                        onMouseDown={(): void => {
                            void startRecording();
                        }}
                        onMouseUp={(): void => {
                            void stopRecording();
                        }}
                    >
                        {isRecording ? 'release to send' : 'push to talk'}
                    </Button>
                )}
                <div className="spacer" />
            </Box>
            <Button
                disabled={showSpinner || !healthcheckData?.apiKey}
                onClick={(): void => {
                    void handleConnectClick();
                }}
                variant="contained"
            >
                {showSpinner ? (
                    <CircularProgress size={24} />
                ) : isConnected ? (
                    'disconnect from voice'
                ) : (
                    'connect to voice'
                )}
            </Button>
            {isDebug && (
                <>
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
