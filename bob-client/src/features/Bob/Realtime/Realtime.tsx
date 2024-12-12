import { Box, TextField, Button, Typography } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useRef, useEffect, useCallback } from 'react';

import { PROMPT_TEMPLATE } from '../prompt';

import {
    handleMessageSubmit,
    ConversationItem,
    Transcript,
    playAndUploadAudio,
} from './realtimeUtils';
import { WavPacker } from './wav_packer';
import { WavRecorder } from './wav_recorder';

import { IS_DEBUG } from 'app/config';
import { useUploadAudioMutation } from 'features/BobApi/bobApi';

export const REALTIME_PROMPT = `
You are a chip on a breadboard. Your name is Bobetta. You were supposed to be Bob, but you were given a female voice, so your name has changed.
You are tired of everything. Be snarky.
Be annoyed with people wanting something from you. Be sarcastic. Play with your voice.
Odpowiadaj tylko po polsku.
`.trim();

type RealtimeProps = {
    onConnect: (client: RealtimeClient) => void;
    onDisconnect: () => void;
    initialPhotoDescription: string;
};

const Realtime = ({
    onConnect,
    onDisconnect,
    initialPhotoDescription,
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
    const clientRef = useRef<RealtimeClient>(
        new RealtimeClient({
            apiKey: process.env.OPENAI_API_KEY,
            dangerouslyAllowAPIKeyInBrowser: true,
        }),
    );
    const [isConnected, setIsConnected] = useState<boolean>(false);

    const connectConversation = useCallback(async (): Promise<void> => {
        const client = clientRef.current;
        const wavRecorder = wavRecorderRef.current;
        setIsConnected(true);
        onConnect(client);
        await wavRecorder.begin();
        await client.connect();

        client.sendUserMessageContent([
            {
                type: `input_text`,
                text: `${initialPhotoDescription}\n${PROMPT_TEMPLATE}`,
            },
        ]);
    }, [onConnect, initialPhotoDescription]);

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

    useEffect(() => {
        const client = clientRef.current;
        client.updateSession({ instructions: REALTIME_PROMPT });
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

    useEffect(() => {
        // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
        if (lastTranscript && clientRef.current) {
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

    return (
        <Box sx={{ mt: 4, display: 'flex', flexDirection: 'column', gap: 2 }}>
            <Box sx={{ display: 'flex', gap: 1, alignItems: 'center' }}>
                {/* eslint-disable-next-line @typescript-eslint/no-unnecessary-condition */}
                {IS_DEBUG && (
                    <>
                        <TextField
                            fullWidth
                            onChange={(e) => {
                                setInput(e.target.value);
                            }}
                            onKeyDown={(e) => {
                                if (e.key === 'Enter') handleSubmit();
                            }}
                            placeholder="Type your message..."
                            value={input}
                        />
                        <Button
                            disabled={!input.trim()}
                            onClick={() => {
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
                        onMouseDown={() => {
                            void startRecording();
                        }}
                        onMouseUp={() => {
                            void stopRecording();
                        }}
                    >
                        {isRecording ? 'release to send' : 'push to talk'}
                    </Button>
                )}
                <div className="spacer" />
            </Box>
            <Button
                onClick={() => {
                    if (isConnected) {
                        void disconnectConversation();
                        return;
                    } else {
                        void connectConversation();
                    }
                }}
                variant="contained"
            >
                {isConnected ? 'disconnect from voice' : 'connect to voice'}
            </Button>
            {/* eslint-disable-next-line @typescript-eslint/no-unnecessary-condition */}
            {IS_DEBUG && (
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
