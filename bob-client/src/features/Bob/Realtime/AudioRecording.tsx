import { Button } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useCallback } from 'react';

import { stopAudio } from './realtimeUtils';
import { WavRecorder } from './wav_recorder';

type AudioRecordingProps = {
    isConnected: boolean;
    client: RealtimeClient | null;
    stopAudioMutation: () => Promise<unknown>;
    useBobSpeaker: boolean;
    wavRecorderRef: React.RefObject<WavRecorder>;
};

const AudioRecording = ({
    isConnected,
    client,
    stopAudioMutation,
    useBobSpeaker,
    wavRecorderRef,
}: AudioRecordingProps): React.JSX.Element => {
    const [isRecording, setIsRecording] = useState<boolean>(false);

    const startRecording = useCallback(async (): Promise<void> => {
        setIsRecording(true);
        const wavRecorder = wavRecorderRef.current;
        if (wavRecorder && client) {
            await wavRecorder.record((data) =>
                client.appendInputAudio(data.mono),
            );
        }
    }, [client, wavRecorderRef]);

    const stopRecording = useCallback(async (): Promise<void> => {
        setIsRecording(false);
        const wavRecorder = wavRecorderRef.current;
        if (wavRecorder && client) {
            await wavRecorder.pause();
            client.createResponse();
        }
    }, [client, wavRecorderRef]);
    const handleMouseDown = useCallback((): void => {
        stopAudio(stopAudioMutation, useBobSpeaker);
        void startRecording();
    }, [startRecording, stopAudioMutation, useBobSpeaker]);

    const handleMouseUp = useCallback((): void => {
        void stopRecording();
    }, [stopRecording]);

    const handleTouchStart = useCallback(
        (e: React.TouchEvent): void => {
            e.preventDefault();
            stopAudio(stopAudioMutation, useBobSpeaker);
            void startRecording();
        },
        [startRecording, stopAudioMutation, useBobSpeaker],
    );

    const handleTouchEnd = useCallback(
        (e: React.TouchEvent): void => {
            e.preventDefault();
            void stopRecording();
        },
        [stopRecording],
    );

    return (
        <Button
            disabled={!isConnected}
            onMouseDown={handleMouseDown}
            onMouseUp={handleMouseUp}
            onTouchEnd={handleTouchEnd}
            onTouchStart={handleTouchStart}
            variant="contained"
        >
            {isRecording ? 'release to send' : 'push to talk'}
        </Button>
    );
};

export default AudioRecording;
