import { Box, TextField, Button, Typography } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import { Mic, MicOff } from 'lucide-react';
import React, { useState, useRef, useEffect } from 'react';

import {
    handleMessageSubmit,
    initializeRealtimeClient,
    startRecording,
    stopRecording,
    Transcript,
} from './realtimeUtils';
import { WavPacker } from './wav_packer';

import { useUploadAudioMutation } from 'features/BobApi/bobApi';

const Realtime = (): React.JSX.Element => {
    const [input, setInput] = useState<string>('');
    const [status, setStatus] = useState<string>('Disconnected');
    const [error, setError] = useState<string>('');
    const [lastTranscript, setLastTranscript] = useState<Transcript[]>();
    const [isRecording, setIsRecording] = useState<boolean>(false);
    const [audioUrl, setAudioUrl] = useState<string>('');
    const clientRef = useRef<RealtimeClient | null>(null);
    const [uploadAudio] = useUploadAudioMutation();
    const audioRef = useRef<HTMLAudioElement | null>(null);

    console.log(lastTranscript);

    const handleInit = async (): Promise<void> => {
        try {
            clientRef.current = await initializeRealtimeClient(
                process.env.OPENAI_API_KEY ?? '',
                setStatus,
                setError,
                setLastTranscript,
                uploadAudio,
            );
        } catch (err) {
            setError(
                err instanceof Error ? err.message : 'Failed to initialize',
            );
            setStatus('Error');
        }
    };

    const handleSubmit = (): void => {
        handleMessageSubmit(clientRef.current, input, setError);
        setInput('');
    };

    const toggleRecording = async (): Promise<void> => {
        if (isRecording) {
            await stopRecording(clientRef.current, setError);
        } else {
            await startRecording(clientRef.current, setError);
        }
        setIsRecording(!isRecording);
    };

    useEffect(() => {
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
            <Typography>Status: {status}</Typography>

            {!clientRef.current && (
                <Button
                    disabled={status === 'Connected'}
                    onClick={() => void handleInit()}
                    variant="contained"
                >
                    Initialize Realtime Client
                </Button>
            )}

            {status === 'Connected' && (
                <Box
                    sx={{
                        display: 'flex',
                        gap: 1,
                        alignItems: 'center',
                    }}
                >
                    <TextField
                        fullWidth
                        onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                            setInput(e.target.value);
                        }}
                        onKeyDown={(
                            e: React.KeyboardEvent<HTMLInputElement>,
                        ) => {
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
                    <Button
                        color={isRecording ? 'error' : 'primary'}
                        onClick={() => void toggleRecording()}
                        startIcon={isRecording ? <MicOff /> : <Mic />}
                        sx={{
                            minWidth: 'auto',
                        }}
                        variant="contained"
                    >
                        {isRecording ? 'Stop' : 'Record'}
                    </Button>
                </Box>
            )}

            {audioUrl && (
                <Box sx={{ mt: 2 }}>
                    <audio controls ref={audioRef} src={audioUrl} />
                </Box>
            )}

            {lastTranscript && (
                <Typography
                    sx={{
                        mt: 2,
                        p: 2,
                        bgcolor: 'background.paper',
                        borderRadius: 1,
                    }}
                    variant="body1"
                >
                    Last response: {lastTranscript[0].transcript}
                </Typography>
            )}

            {error && <Typography color="error">Error: {error}</Typography>}
        </Box>
    );
};

export default Realtime;
