import { Box, TextField, Button, Typography } from '@mui/material';
import type { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useRef } from 'react';

import { handleMessageSubmit, initializeRealtimeClient } from './realtimeUtils';

import { useUploadAudioMutation } from 'features/BobApi/bobApi';

const Realtime = (): React.JSX.Element => {
    const [input, setInput] = useState('');
    const [status, setStatus] = useState('Disconnected');
    const [error, setError] = useState('');
    const [lastTranscript, setLastTranscript] = useState('');
    const clientRef = useRef<RealtimeClient | null>(null);
    const [uploadAudio] = useUploadAudioMutation();

    const handleInit = async (): Promise<void> => {
        try {
            clientRef.current = await initializeRealtimeClient(
                process.env.OPENAI_API_KEY,
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
                        onClick={handleSubmit}
                        variant="contained"
                    >
                        Send
                    </Button>
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
                    Last response: {lastTranscript}
                </Typography>
            )}

            {error && <Typography color="error">Error: {error}</Typography>}
        </Box>
    );
};

export default Realtime;
