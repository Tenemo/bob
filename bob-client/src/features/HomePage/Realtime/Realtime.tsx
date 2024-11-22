import { Box, TextField, Button, Typography } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useRef } from 'react';

const Realtime = (): React.JSX.Element => {
    const [input, setInput] = useState('');
    const [status, setStatus] = useState('Disconnected');
    const [error, setError] = useState('');
    const clientRef = useRef<RealtimeClient | null>(null);

    const playAudio = (audioData: Int16Array): void => {
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
    };

    const initializeClient = async (): Promise<void> => {
        try {
            if (!process.env.OPENAI_API_KEY) {
                throw new Error('OpenAI API key not found');
            }

            clientRef.current = new RealtimeClient({
                apiKey: process.env.OPENAI_API_KEY,
                dangerouslyAllowAPIKeyInBrowser: true,
            });
            clientRef.current.updateSession({ voice: 'coral' });

            clientRef.current.on(
                'conversation.item.completed',
                ({
                    item,
                }: {
                    item: {
                        type: string;
                        role: string;
                        formatted?: { audio: Int16Array };
                    };
                }) => {
                    if (
                        item.type === 'message' &&
                        item.role === 'assistant' &&
                        item.formatted?.audio
                    ) {
                        playAudio(item.formatted.audio);
                    }
                },
            );

            await clientRef.current.connect();
            setStatus('Connected');
            setError('');
        } catch (err) {
            setError(
                err instanceof Error ? err.message : 'Failed to initialize',
            );
            setStatus('Error');
        }
    };

    const handleSubmit = (): void => {
        if (!clientRef.current || !input.trim()) return;

        try {
            clientRef.current.sendUserMessageContent([
                { type: 'input_text', text: input.trim() },
            ]);
            setInput('');
        } catch (err) {
            setError(
                err instanceof Error ? err.message : 'Failed to send message',
            );
        }
    };

    return (
        <Box sx={{ mt: 4, display: 'flex', flexDirection: 'column', gap: 2 }}>
            <Typography>Status: {status}</Typography>

            {!clientRef.current && (
                <Button
                    disabled={status === 'Connected'}
                    onClick={() => void initializeClient()}
                    variant="contained"
                >
                    Initialize Realtime Client
                </Button>
            )}

            {status === 'Connected' && (
                <Box sx={{ display: 'flex', gap: 1 }}>
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
                </Box>
            )}

            {error && <Typography color="error">Error: {error}</Typography>}
        </Box>
    );
};

export default Realtime;
