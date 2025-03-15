import { Box, Typography } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useRef, useEffect } from 'react';

import AudioRecording from './AudioRecording';
import ConnectionButton from './ConnectionButton';
import DebugControls from './DebugControls';
import { Transcript } from './realtimeUtils';
import SharePictureButton from './SharePictureButton';
import { useRealtimeConnection } from './useRealtimeConnection';
import { WavPacker } from './wav_packer';

import { useAppSelector } from 'app/hooks';
import { selectIsDebug, selectUseBobSpeaker } from 'features/Bob/bobSlice';
import {
    useUploadAudioMutation,
    useStopAudioMutation,
    useLazyHealthcheckQuery,
} from 'features/BobApi/bobApi';

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
    const [error, setError] = useState<string>('');
    const [audioUrl, setAudioUrl] = useState<string>('');
    const [lastTranscript, setLastTranscript] = useState<Transcript[]>();
    const audioRef = useRef<HTMLAudioElement | null>(null);
    const [uploadAudio] = useUploadAudioMutation();
    const [stopAudioMutation] = useStopAudioMutation();
    const [triggerHealthcheck, { data: healthcheckData }] =
        useLazyHealthcheckQuery();
    const isDebug = useAppSelector(selectIsDebug);
    const useBobSpeaker = useAppSelector(selectUseBobSpeaker);

    // We need to keep the same reference, because
    // conversation.item.completed otherwise has a reference
    // to the old value and a new reference is created for the new value.
    const useBobSpeakerRef = useRef<boolean>(useBobSpeaker);
    useEffect(() => {
        useBobSpeakerRef.current = useBobSpeaker;
    }, [useBobSpeaker]);

    const {
        clientRef,
        wavRecorderRef,
        isConnected,
        isConnectInProgress,
        handleConnectClick,
    } = useRealtimeConnection({
        onConnect,
        onDisconnect,
        getPhotoDescription,
        uploadAudio,
        triggerHealthcheck,
        healthcheckData,
        setError,
        setLastTranscript,
        useBobSpeakerRef,
    });

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
    }, [clientRef, lastTranscript]);

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
            <SharePictureButton
                client={clientRef.current}
                getPhotoDescription={getPhotoDescription}
                isConnected={isConnected}
                setError={setError}
            />

            <DebugControls
                audioRef={audioRef}
                audioUrl={audioUrl}
                client={clientRef.current}
                isDebug={isDebug}
                lastTranscript={lastTranscript}
                setError={setError}
                stopAudioMutation={stopAudioMutation}
                useBobSpeaker={useBobSpeaker}
            />

            {isConnected && (
                <AudioRecording
                    client={clientRef.current}
                    isConnected={isConnected}
                    stopAudioMutation={stopAudioMutation}
                    useBobSpeaker={useBobSpeaker}
                    wavRecorderRef={wavRecorderRef}
                />
            )}

            <ConnectionButton
                handleConnectClick={handleConnectClick}
                isConnectInProgress={isConnectInProgress}
                isConnected={isConnected}
                isVisionLoading={isVisionLoading}
                stopAudioMutation={stopAudioMutation}
                useBobSpeaker={useBobSpeaker}
            />

            {error && <Typography color="error">Error: {error}</Typography>}
        </Box>
    );
};

export default Realtime;
