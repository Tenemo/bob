import { Box, Button, TextField, Typography } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useCallback } from 'react';

import { Transcript, stopAudio, handleMessageSubmit } from './realtimeUtils';

type DebugControlsProps = {
    isDebug: boolean;
    client: RealtimeClient | null;
    stopAudioMutation: () => Promise<unknown>;
    useBobSpeaker: boolean;
    audioUrl: string;
    audioRef: React.RefObject<HTMLAudioElement>;
    lastTranscript?: Transcript[];
    setError: (error: string) => void;
};

const DebugControls = ({
    isDebug,
    client,
    stopAudioMutation,
    useBobSpeaker,
    audioUrl,
    audioRef,
    lastTranscript,
    setError,
}: DebugControlsProps): React.JSX.Element | null => {
    const [input, setInput] = useState<string>('');

    const handleSubmit = useCallback((): void => {
        handleMessageSubmit(client, input, setError);
        setInput('');
    }, [client, input, setError]);

    const handleStopAudio = useCallback((): void => {
        stopAudio(stopAudioMutation, useBobSpeaker);
    }, [stopAudioMutation, useBobSpeaker]);

    if (!isDebug) return null;

    return (
        <>
            <Box sx={{ display: 'flex', gap: 1, alignItems: 'center' }}>
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
            </Box>
            <Button color="error" onClick={handleStopAudio} variant="outlined">
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
    );
};

export default DebugControls;
